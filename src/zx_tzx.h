// [ref] https://www.alessandrogrussu.it/tapir/tzxform120.html
// [ref] https://www.alessandrogrussu.it/loading/schemes/Schemes.html
//
// @todo
// tzx(csw) starbike1, starbike2
// tzx(gdb) basil great mouse, bc quest for tires, book of the dead part 1 (crl), dan dare 2 mekon, world cup carnival
// tzx(flow) hollywood poker, bubble bobble (the hit squad)
// tzx(noise) leaderboard par 3, tai-pan, wizball
// tzx(voc) catacombs of balachor
// tzx(???) bathyscape

#include <stdbool.h>

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
                mic_render_data(src, bytes, bits, zero, one);
                mic_render_pause(pause);
mic_queue_has_turbo = 1;
                debug = va("bytes:%5u pause:%3ums 0:%3u 1:%4u bits:%u", bytes, pause, zero, one, bits);
                src += bytes;

            break; case 0x15: // @todo: Terrahawks(48K), DreddOverEels, Thanatos(Erbe)
                blockname = "Voc";
                src += 5;
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000;
                src += bytes;
            break; case 0x18: // @todo: OlimpoEnGuerra(Part2), CaseOfMurderA
                blockname = "Csw";
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000; bytes |= (*src++)*0x1000000;
                src += bytes;
            break; case 0x19: // @todo: BountyBobStrikesBack(Americana), NowotnikPuzzleThe, Twister-MotherOfCharlotte, AYankeeInIraqv132(TurboLoader)
                blockname = "generalizedData";
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000; bytes |= (*src++)*0x1000000;
                pause  = (*src++); pause  |= (*src++)*0x100;

                unsigned totp  = (*src++); totp  |= (*src++)*0x100; totp |= (*src++)*0x10000; totp |= (*src++)*0x1000000;
                unsigned npp = (*src++);
                unsigned asp = (*src++);
                unsigned totd  = (*src++); totd  |= (*src++)*0x100; totd |= (*src++)*0x10000; totd |= (*src++)*0x1000000;
                unsigned npd = (*src++);
                unsigned asd = (*src++);
                if(totp>0) {
                    for( unsigned i = 0; i < asp; ++i ) {
                        unsigned level = (*src++);
                        for( unsigned j = 0; j < npp; ++j ) {
                            sync1 = (*src++); sync1 |= (*src++)*0x100;
                            mic_render_sync(sync1);
                        }
                    }
                    for( unsigned i = 0; i < totp; ++i ) {
                        unsigned symbol = (*src++);
                        count = (*src++); count |= (*src++)*0x100;
                    }
                }
                if(totd>0) {
                    for( unsigned i = 0; i < asd; ++i ) {
                        unsigned level = (*src++);
                        for( unsigned j = 0; j < npd; ++j ) {
                            sync1 = (*src++); sync1 |= (*src++)*0x100;
                            mic_render_sync(sync1);
                        }
                    }
                    for( unsigned i = 0; i < totd; ++i ) {
                        unsigned symbol = (*src++);
                        count = (*src++); count |= (*src++)*0x100;
                    }
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

            break; case 0x28: // IGNORED: LoneWolf3-TheMirrorOfDeath
                blockname = "select";
                count  = (*src++); count  |= (*src++)*0x100;
                src += count;

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
