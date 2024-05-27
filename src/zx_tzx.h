// [ref] https://www.alessandrogrussu.it/tapir/tzxform120.html
// [ref] https://www.alessandrogrussu.it/loading/schemes/Schemes.html
//
// @todo
// find 1bas-then-1code pairs within tapes. provide a prompt() call if there are more than 1 pair in the tape
//     then, deprecate 0x28 block
// csw starbike1, starbike2
// tzx(gdb) basil great mouse, bc quest for tires, book of the dead part 1 (crl), dan dare 2 mekon, world cup carnival
// tzx(flow) hollywood poker, bubble bobble (the hit squad)
// tzx(noise) leaderboard par 3, tai-pan, wizball
// tzx(voc) catacombs of balachor
// tzx(???) bathyscape

#include <stdbool.h>

int csw_load(byte *fp, int len) {
    return 0;

    mic_reset();

    if( len < 0x20 || memcmp(fp, "Compressed Square Wave\x1a", 0x17) )
        return 0;

    byte *eot = fp + len;

    fp += 0x17;

    byte major = *fp++;
    byte minor = *fp++;
    int version = major * 100 + minor;

    dword rate = 0, pulses = 0;
    byte comp = 0, flags = 0;

    /**/ if( version == 101 ) {
        rate  = (*fp++); rate |= (*fp++)*0x100;
        comp = *fp++; // 1 rle
        flags = *fp++;
        fp += 3; // reserved
    }
    else if( version = 200 ) {
        rate  = (*fp++);
        rate |= (*fp++)*0x100;
        rate |= (*fp++)*0x10000;
        rate |= (*fp++)*0x1000000;

        pulses  = (*fp++);
        pulses |= (*fp++)*0x100;
        pulses |= (*fp++)*0x10000;
        pulses |= (*fp++)*0x1000000;

        comp = *fp++; // 1 rle, 2 zrle
        flags = *fp++;

        byte hdr = *fp++;
        fp += 16; // skip application description
        fp += hdr; // skip header
    }
    else {
        warning("error: unknown .csw version");
        return 0;
    }

    if( comp != 0x1 ) {
        warning("error: unsupported compression method");
        return 0;
    }

    printf("rate:%u\n", rate);

    int level = 0;

    if( comp == 0x1 )
    while( fp < eot ) {
        unsigned p = *fp++;
        if( p == 0 ) {
            p  = (*fp++);
            p |= (*fp++)*0x100;
            p |= (*fp++)*0x10000;
            p |= (*fp++)*0x1000000;
        }

        // '0' saved as a x2 pulses of 855 T. since 1T = 1/3500 ms
        // '0' is saved in 2*0.244ms

        // 22050 or 44100 Hz (158 or 79 T-states/sample)

        // 5s DELAY_HEADER = 8063
        // 2s DELAY_DATA = 3223

        // normalize pulse to 69888 ticks
        int64_t pulses = ( 69888 * p ) / rate;
        int limit = PILOT * 1.5;

        pulses += !pulses;

        static int i = 0; if(++i < 1000) printf("%u->%u,",p,(unsigned)pulses);

        while(pulses > 0) {
            mic_render_polarity(level ? HIGH : LOW), level ^= 1;
            mic_render_sync(limit);
            pulses -= limit;
        }
        if(pulses) {
            mic_render_polarity(level ? HIGH : LOW), level ^= 1;
            mic_render_sync(-pulses);
        }
    }

    // ... data

    mic_finish();
    return 1;
}

int tzx_load(byte *fp, int len) {
    mic_reset();

    // verify tzx
    if( memcmp(fp, "ZXTape!\x1a", 8) ) return 0;

    // skip header & check version
    int major=fp[8];
    if (major>1) return 0; // unsupported version
    fp += 10;
    len -= 10;

    int valid=1;
    unsigned processed = 0;
    unsigned pulses, pilot, sync1, sync2, zero, one, pause, bytes, count, bits;
    char brief_description[4*1024] = {0}, *brief = brief_description;

//    int padding_bytes = -1;
    int group_level = 0;
    unsigned loop_counter = 0, loop_pointer = 0;

    // parse blocks till end of tape
    for(byte *src = (byte*)fp, *end = src+len; valid && src < end; ++processed) {
        int id = *src++;

        unsigned bytes = 0;
        const char *blockname = "", *debug = "";

        switch (id) {
            default:
                blockname = "????????";
#if 1
                valid = 0;
#else
                src = end;
#endif

            break; case 0x10: //OK(0)
                blockname = "Standard";
                pause  = (*src++); pause  |= (*src++)*0x100;
                bytes  = (*src++); bytes  |= (*src++)*0x100;
                pulses = src[0] < 128 ? DELAY_HEADER : DELAY_DATA; // < 4 ?
                mic_render_full(src, bytes, 8, pulses, PILOT, SYNC1, SYNC2, ZERO, ONE, pause);
                debug = va("%ums %.*s", pause, src[0] < 128 ? 10 : 0, bytes >= 12 ? src+1+!src[1] : (byte*)"");
                src += bytes;

            break; case 0x11: // OK(1)
                blockname = "Turbo";
                pilot  = (*src++); pilot  |= (*src++)*0x100;
                sync1  = (*src++); sync1  |= (*src++)*0x100;
                sync2  = (*src++); sync2  |= (*src++)*0x100;
                zero   = (*src++); zero   |= (*src++)*0x100;
                one    = (*src++); one    |= (*src++)*0x100;
                pulses = (*src++); pulses |= (*src++)*0x100;
                bits   = (*src++);
                pause  = (*src++); pause  |= (*src++)*0x100;
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000;
                mic_render_full(src, bytes, bits, pulses, pilot, sync1, sync2, zero, one, pause);
mic_queue_has_turbo = 1;
                debug = va("bits:%u N:%u P:%u S1:%u S2:%u 0:%u 1:%u %ums %.*s", bits, pulses, pilot, sync1, sync2, zero, one, pause, src[0] < 128 ? 10 : 0, bytes >= 12 ? src+1+!src[1] : (byte*)"");
                src += bytes;

            break; case 0x12: // REV
                blockname = "Pilot";
                pilot  = (*src++); pilot  |= (*src++)*0x100;
                pulses = (*src++); pulses |= (*src++)*0x100;
                mic_render_pilot(pulses, pilot);
mic_queue_has_turbo = 1;
                debug = va("pulses:%u, pilot:%u", pulses, pilot);
                src += 0;

            break; case 0x13: // REV
                blockname = "Bits";
                pulses = (*src++);
                for( unsigned i = 0; i < pulses; ++i ) {
                    sync1  = (*src++); sync1  |= (*src++)*0x100;
                    mic_render_sync(sync1);
                    if(i==0) debug = va("pulses:%u, sync:%u [...]", pulses, sync1);
                }
mic_queue_has_turbo = 1;
                src += 0;

            break; case 0x14: // OK(1)
                blockname = "Headerless";
                zero   = (*src++); zero   |= (*src++)*0x100;
                one    = (*src++); one    |= (*src++)*0x100;
                bits   = (*src++);
                pause  = (*src++); pause  |= (*src++)*0x100;
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000;
                mic_render_data(src, bytes, bits, zero, one, 2);
                mic_render_pause(pause);
mic_queue_has_turbo = 1;
                debug = va("bytes:%5u pause:%3ums 0:%3u 1:%4u bits:%u", bytes, pause, zero, one, bits);
                src += bytes;

            break; case 0x15: // @todo: Catacombs of Balachor, Dead by Dawn, zombo, amnesia, Terrahawks(48K),   Super Cold War Simulator, Touch My Spectrum, elfen, Overtake the Pope, Overtake the POPE 2, route66, boulder jumper, DreddOverEels, Thanatos(Erbe)
                blockname = "VOC";

                // spec says freq is 22050 or 44100 Hz (158 or 79 states/sample)
                // however, i have found only 79 or 80 states/sample

                unsigned states = (*src++); states |= (*src++)*0x100;
                         pause  = (*src++); pause  |= (*src++)*0x100;
                         bits   = (*src++);
                         bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000;

                debug = va("bytes:%u pause:%ums states/sample:%u bits:%u", bytes, pause, states, bits);

                // our current mic code puts a sample on EAR every 4 t-states
                // this freq is feeding 80 states/sample though. x20 times faster

                // mic_render_data(src, bytes/10, bits, ZERO, ONE, 1);
                for( int i = 0; i < bytes/10; ++i) {

                }

                mic_render_pause(pause);

                src += bytes;

            break; case 0x18: // ignored. see all cases: OlimpoEnGuerra(Part2), CaseOfMurderA, AdvancedGretaThunbergSimulator
                blockname = "CSW";
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000; bytes |= (*src++)*0x1000000;
                #if 0
                pause  = (*src++); pause  |= (*src++)*0x100;

                unsigned rate = (*src++); rate |= (*src++)*0x100; rate |= (*src++)*0x10000;
                unsigned comp = (*src++); unsigned zipped = comp & 2;
                pulses = (*src++); pulses |= (*src++)*0x100; pulses |= (*src++)*0x10000; pulses |= (*src++)*0x1000000;

                byte *csw = src;
                bytes -= 10;

                warning(va("csw %srle found", zipped ? "z-":""));
                #endif
                src += bytes;

            break; case 0x19: // @todo: Basil, BountyBobStrikesBack(Americana), NowotnikPuzzleThe, Twister-MotherOfCharlotte, AYankeeInIraqv132(TurboLoader)
                blockname = "generalizedData";
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000; bytes |= (*src++)*0x1000000;
                pause  = (*src++); pause  |= (*src++)*0x100;

                unsigned totp  = (*src++); totp  |= (*src++)*0x100; totp |= (*src++)*0x10000; totp |= (*src++)*0x1000000;
                unsigned npp = (*src++);
                unsigned asp = (*src++); asp += 256 * !asp;
                unsigned totd  = (*src++); totd  |= (*src++)*0x100; totd |= (*src++)*0x10000; totd |= (*src++)*0x1000000;
                unsigned npd = (*src++);
                unsigned asd = (*src++); asd += 256 * !asd;

                debug = va("bytes:%u, pause:%ums, totp:%u, npp:%u, asp:%u, totd:%u, npd:%u, asd:%u", bytes, pause, totp, npp, asp, totd, npd, asd);

                // 6951 bytes [Turbo        ] bits:2 N:1020 P:2577 S1:1365 S2:315 0:616 1:1232 7460ms UUUUUUUUUU
                printf("\ntzx.block %03d ($%02X)    GDB: totp:%u npp:%u asp:%u totd:%u npd:%u asd:%u", processed, id, totp, npp, asp, totd, npd, asd);

                struct symdef {
                    byte polarity;
                    word pulses[256]; // [0..NPP] or [0..NPD]
                } symdef_asp[256] = {0}, symdef_asd[256] = {0};

                for( unsigned symb = 0; totp && symb < asp; ++symb) {
                    struct symdef *s = symdef_asp + symb;

                    s->polarity = (*src++);
                    printf("\ntzx.block %03d ($%02X)    GDB: t[%d] +%u, ", processed, id, symb, s->polarity);
                    for( unsigned i = 0; i < npp; ++i) {
                        s->pulses[i] = (*src++); s->pulses[i] |= (*src++)*0x100;
                        printf("p%u, ", s->pulses[i]);
                    }
                }

                for( unsigned i = 0; i < totp; ++i) {
                    unsigned symb = (*src++);
                    unsigned reps = (*src++); reps |= (*src++)*0x100;

                    struct symdef *s = symdef_asp + symb;

                    for( int j = 0; j < npp; ++j) {
                        if( s->pulses[j] ) {
                            mic_render_polarity(s->polarity);
                            mic_render_pilot(reps, s->pulses[j]);
                        }
                    }
                }

                for( unsigned symb = 0; totd && symb < asd; ++symb) {
                    struct symdef *s = symdef_asd + symb;
                    s->polarity = (*src++);
                    printf("\ntzx.block %03d ($%02X)    GDB: d[%d] +%u, ", processed, id, symb, s->polarity);
                    for( unsigned i = 0; i < npd; ++i) {
                        s->pulses[i] = (*src++); s->pulses[i] |= (*src++)*0x100;
                        printf("p%u, ", s->pulses[i]);
                    }
                }

                puts("");

                unsigned NB = ceil(log2(asd));
                unsigned DS = ceil(NB*totd/8);

                while( DS-- ) {
                    byte data = (*src++);

                    int zero = symdef_asd[0].pulses[0];
                    int one = symdef_asd[1].pulses[0];

                    // @fixme: apply polarity per bit, not byte
                    mic_render_polarity(symdef_asd[0].polarity);
                    mic_render_data(&data, 1, NB > 8 ? 8 : NB, zero, one, 2);

                    NB -= 8;
                }

            break; case 0x20: // OK(0) // TheMunsters
                blockname = "pauseOrStop";
                pause  = (*src++); pause  |= (*src++)*0x100; 
                debug = va("%ums", pause);
#if 0
                if(ZX < 128) mic_render_stop();      // added ZX<128 for munsters
                else mic_render_pause(pause+!pause); // added for Untouchables(HitSquad) ??? // tzx 1.13 says: if(pause==0)pause=1;
#else
                if(!pause) mic_render_stop();
                else mic_render_pause(pause);
#endif

            break; case 0x21: // IGNORED (see: BleepLoad)
                blockname = "groupStart";
                bytes  = (*src++);
                src += bytes;

                group_level++;

            break; case 0x22: // IGNORED (see: BleepLoad)
                blockname = "groupEnd";
                src += 0;

                group_level--;

            break; case 0x23: // IGNORED: 1942.tzx, HollywoodPoker, PanamaJoe, MagicJohnson'sBasketball(Spanish)-SideA
                blockname = "jumpTo";
                bytes  = (*src++); bytes  |= (*src++)*0x100;
                src += 0;

            break; case 0x24: // OK?: HollywoodPoker, MarioBros
                blockname = "loopStart";
                count  = (*src++); count  |= (*src++)*0x100;
                src += 0;

                loop_counter = count;
                loop_pointer = (unsigned)(src - (byte*)fp);

            break; case 0x25: // OK?: HollywoodPoker
                blockname = loop_counter == 1 ? "loopEnd" : 0;
                src += 0;

                if( --loop_counter ) src = (byte*)fp + loop_pointer;

            break; case 0x26: // IGNORED: HollywoodPoker
                blockname = "callSeq";
                count  = (*src++); count  |= (*src++)*0x100;
                src += count * 2;

            break; case 0x27: // IGNORED: HollywoodPoker
                blockname = "return";
                src += 0;

            break; case 0x28: // can be ignored: LoneWolf-TheMirrorOfDeath.tzx, HitPakTrio-Side1(Zafiro).tzx, Multimixx4-SideB.tzx, ColeccionDeExitosDinamic*.tzx
                blockname = "select";
                count  = (*src++); count  |= (*src++)*0x100;
                src += count;

#if 1 // HAS_PROMPT
                src -= count;

                byte selections = *src++;
                uint16_t offsets[256] = {0};

                char body[1024] = {0}, *ptr = body;
                for( byte i = 0; i < selections; ++i ) {
                    offsets[i] = (*src++); offsets[i] |= (*src++) * 0x100;
                    byte len = (*src++);
                    ptr += sprintf(ptr, "%d) %.*s\n", i+1, len, src);
                    src += len;
                }
                char* answer = prompt( NULL, "Select block", body, "1" );

                int selection = answer ? atoi(answer) : 0;
                if( selection > 0 && selection <= selections ) {
                    unsigned num_skipped_blocks = offsets[--selection] - 1;
                    while( num_skipped_blocks-- > 0 ) {
                        #define READ1(p) (src[p])
                        #define READ2(p) (src[(p)+1] * 0x100 + src[p])
                        #define READ3(p) (src[(p)+2] * 0x10000 + READ2(p))
                        #define READ4(p) (src[(p)+3] * 0x1000000 + READ3(p))
                        switch( *src++ ) { default:
                        break; case 0x10: src += 0x04 + READ2(2);
                        break; case 0x11: src += 0x12 + READ3(0xF);
                        break; case 0x12: src += 0x04 ;
                        break; case 0x13: src += 0x01 + READ1(0) * 2;
                        break; case 0x14: src += 0x0A + READ3(7);
                        break; case 0x15: src += 0x08 + READ3(5);
                        break; case 0x18: src += 0x04 + READ4(0);
                        break; case 0x19: src += 0x04 + READ4(0);

                        break; case 0x20: src += 0x02;
                        break; case 0x21: src += 0x01 + READ1(0);
                        break; case 0x22: src += 0x00;
                        break; case 0x23: src += 0x02;
                        break; case 0x24: src += 0x02;
                        break; case 0x25: src += 0x00;
                        break; case 0x26: src += 0x02 + READ2(0) * 2;
                        break; case 0x27: src += 0x00;
                        break; case 0x28: src += 0x02 + READ2(0);
                        break; case 0x2A: src += 0x04;
                        break; case 0x2B: src += 0x05;

                        break; case 0x30: src += 0x01 + READ1(0);
                        break; case 0x31: src += 0x02 + READ1(1);
                        break; case 0x32: src += 0x02 + READ2(0);
                        break; case 0x33: src += 0x01 + READ1(0) * 3;
                        break; case 0x35: src += 0x14 + READ4(0x10);

                        break; case 0x5A: src += 0x09;
                        }
                    }
                }
#endif

            break; case 0x2A: // OK(0)
                blockname = "48KStopTape"; 
                count  = (*src++); count  |= (*src++)*0x100; count  |= (*src++)*0x10000; count  |= (*src++)*0x1000000;
                // src += count; // batman the movie has count(0), oddi the viking has count(4). i rather ignore the count value
                if(ZX < 128) mic_render_stop(); // @fixme: || page128 & 16

            break; case 0x2b: // REV: Cybermania, CASIO-DIGIT-INVADERS-v3
                blockname = "signalLevel";
                count  = (*src++); count  |= (*src++)*0x100;
                byte level = (*src++);
                src += 0;
                warning("signalLevel block tape found! please check (0x2b)");

            break; case 0x30: // OK(0)
                blockname = "Text";
                count  = (*src++);
                debug = va("%.*s", count, src);
                src += count;

            break; case 0x31: // OK(0)
                blockname = "Message";
                count  = (*src++); // timeout secs
                count  = (*src++);
                debug = va("%.*s", count, src);
                src += count;

            break; case 0x32: // IGNORED (OK)
                blockname = "fileInfo";
                count  = (*src++); count  |= (*src++)*0x100;
                src += count;

            break; case 0x33: // IGNORED
                blockname = "hardwareType";
                count  = (*src++);
                src += count*3;

            break; case 0x5a: // OK
                blockname = "+glue";
                src += 9;
                // if(ZX < 128) mic_render_stop(); // note: custom modification to handle consecutive glued tapes
                mic_render_pause(1000);
                mic_render_stop(); //< probably a good idea

            break; case 0x16: // DEPRECATED
                blockname = "c64Data (deprecated)";
                bytes  = (*src++); bytes  |= (*src++)*0x100;
                src += bytes;
            break; case 0x17: // DEPRECATED
                blockname = "c64Turbo (deprecated)";
                bytes  = (*src++); bytes  |= (*src++)*0x100;
                src += bytes;
            break; case 0x34: // DEPRECATED
                blockname = "emulationInfo (deprecated)";
                src += 2 + 1 + 2 + 3;
            break; case 0x35: // DEPRECATED (OK)
                blockname = "customInfo (deprecated)";
                src += 0x10;
                bytes = (*src++);
                bytes = bytes + (*src++)*0x100;
                bytes = bytes + (*src++)*0x10000;
                bytes = bytes + (*src++)*0x1000000;
                src += bytes;
            break; case 0x40: // DEPRECATED
                blockname = "snapshot (deprecated)";
                ++src;
                bytes = (*src++);
                bytes = bytes + (*src++)*0x100;
                bytes = bytes + (*src++)*0x10000;
                src+= bytes;
        }

        if(blockname) {
        *brief++ = blockname[0];
        printf("tzx.block %03d ($%02X) %6d bytes [%-13s] %s\n",processed,id,bytes,blockname,debug);
        }
    }

//  puts(brief_description);

    mic_finish();
    return valid;
}
