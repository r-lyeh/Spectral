// [ref] https://www.alessandrogrussu.it/tapir/tzxform120.html
// [ref] https://www.alessandrogrussu.it/loading/schemes/Schemes.html
//
// @todo
// tzx(gdb) basil great mouse, bc quest for tires, book of the dead part 1 (crl), dan dare 2 mekon, world cup carnival
// tzx(flow) hollywood poker, bubble bobble (the hit squad)
// tzx(noise) leaderboard par 3, tai-pan, wizball

int tzx_load(const byte *fp, int len) {
    // verify tzx
    if( memcmp(fp, "ZXTape!\x1a", 8) ) return 0;

    // skip header & check version
    int major=fp[8];
    if (major>1) return 0; // unsupported version
    fp += 10;
    len -= 10;

    tape_reset();

    int valid=1;
    unsigned processed = 0;
    unsigned pulses, pilot, sync1, sync2, zero, one, pause, bytes, count, bits;

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
                valid = 0;

            break; case 0x10: //OK(0)
                blockname = "Standard";
                pause  = (*src++); pause  |= (*src++)*0x100;
                bytes  = (*src++); bytes  |= (*src++)*0x100;
                pulses = src[0] < 128 ? DELAY_HEADER : DELAY_DATA; // < 4 ?
                tape_render_full(src, bytes, 8, pulses, PILOT, SYNC1, SYNC2, ZERO, ONE, pause);
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
                tape_render_full(src, bytes, bits, pulses, pilot, sync1, sync2, zero, one, pause);
                tape_has_turbo = 1;
                debug = va("bits:%u N:%u P:%u S1:%u S2:%u 0:%u 1:%u %ums %.*s", bits, pulses, pilot, sync1, sync2, zero, one, pause, src[0] < 128 ? 10 : 0, bytes >= 12 ? src+1+!src[1] : (byte*)"");
                src += bytes;

            break; case 0x12: // REV
                blockname = "Pilot";
                pilot  = (*src++); pilot  |= (*src++)*0x100;
                pulses = (*src++); pulses |= (*src++)*0x100;
                tape_render_pilot(pulses, pilot);
                tape_has_turbo = 1;
                debug = va("pulses:%u, pilot:%u", pulses, pilot);
                src += 0;

            break; case 0x13: // REV
                blockname = "Bits";
                pulses = (*src++);
                for( unsigned i = 0; i < pulses; ++i ) {
                    sync1  = (*src++); sync1  |= (*src++)*0x100;
                    tape_render_sync(sync1);
                    if(i==0) debug = va("pulses:%u, sync:%u [...]", pulses, sync1);
                }
                tape_has_turbo = 1;
                src += 0;

            break; case 0x14: // OK(1)
                blockname = "Headerless";
                zero   = (*src++); zero   |= (*src++)*0x100;
                one    = (*src++); one    |= (*src++)*0x100;
                bits   = (*src++);
                pause  = (*src++); pause  |= (*src++)*0x100;
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000;
                tape_render_data(src, bytes, bits, zero, one, 2);
                tape_render_pause(pause);
                tape_has_turbo = 1;
                debug = va("bytes:%5u pause:%3ums 0:%3u 1:%4u bits:%u", bytes, pause, zero, one, bits);
                src += bytes;

            break; case 0x15: // OK: Catacombs of Balachor, Dead by Dawn, zombo, amnesia, Terrahawks(48K), 
                // Super Cold War Simulator, Touch My Spectrum, elfen, Overtake the Pope, Overtake the POPE 2, 
                // route66, boulder jumper, DreddOverEels, Thanatos(Erbe)
                blockname = "VOC";

                // spec says freq is 22050 or 44100 Hz (158 or 79 states/sample)
                // however, i have found only 79 or 80 states/sample

                unsigned states = (*src++); states |= (*src++)*0x100;
                         pause  = (*src++); pause  |= (*src++)*0x100;
                         bits   = (*src++);
                         bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000;

                debug = va("bytes:%u pause:%ums states/sample:%u bits:%u", bytes, pause, states, bits);

                for( int len = 0; len < bytes; ++len, ++src )
                    for( int i = 0; i < 8; ++i) {
                        tape_render_bit((*src) & (1<<(7-i)), states);
                    }

                voc_len -= 8 - bits; // trim ending excess bits

                tape_render_stop();
                tape_has_turbo = 1;

            break; case 0x18: // OK, ignored. see all cases: OlimpoEnGuerra(Part2), CaseOfMurderA, AdvancedGretaThunbergSimulator
                blockname = "CSW";
                bytes  = (*src++); bytes  |= (*src++)*0x100; bytes |= (*src++)*0x10000; bytes |= (*src++)*0x1000000;
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
                            tape_render_polarity(s->polarity);
                            tape_render_pilot(reps, s->pulses[j]);
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
                    tape_render_polarity(symdef_asd[0].polarity);
                    tape_render_data(&data, 1, NB > 8 ? 8 : NB, zero, one, 2);

                    NB -= 8;
                }

            break; case 0x20: // OK(0) // TheMunsters, Untouchables(HitSquad)
                blockname = "pauseOrStop";
                pause  = (*src++); pause  |= (*src++)*0x100; 
                debug = va("%ums", pause);
                if(!pause) tape_render_stop();
                else tape_render_pause(pause);

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
                if(ZX < 128) tape_render_stop(); // @fixme: || page128 & 16
                else tape_render_pause(1000); // experimental

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
                // if(ZX < 128) tape_render_stop(); // note: custom modification to handle consecutive glued tapes
                tape_render_pause(1000);
                tape_render_stop(); //< probably a good idea

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
        printf("tzx.block %03d ($%02X) %6d bytes [%-13s] %s\n",processed,id,bytes,blockname,debug);
        }
    }

    tape_finish();
    return valid;
}

int csw_load(const byte *fp, int len) {
    if( len < 0x20 || memcmp(fp, "Compressed Square Wave\x1a", 0x17) )
        return 0;

    const byte *beg = fp, *eot = fp + len;

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
    else if( version == 200 ) {
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

    printf("%d pulses\n", pulses);

    if( comp == 2 ) {
        static byte *unc = 0;
        unc = realloc(unc, pulses);

        printf("inflate @ %x offs, %d bytes\n", (unsigned)(fp - beg), (signed)(eot - fp));

        mz_stream z = {0};
        z.avail_in = eot - fp;
        z.next_in = fp;
        z.avail_out = pulses;
        z.next_out = unc;

        mz_inflateInit(&z);
        int ret = mz_inflate(&z, MZ_NO_FLUSH);
        mz_inflateEnd(&z);

        if( ret < 0 ) {
            warning("error: cant decompress csw2 file");
            return 0;
        }

        fp = beg = unc;
        eot = fp + pulses;
    }
    else
    if( comp != 0x1 ) {
        warning(va("error: unsupported .csw compression method (%d)",comp));
        return 0;
    }

    // 22050 or 44100 Hz (158 or 79 T-states/sample)
    printf("rate:%u, polarity:%d\n", rate, flags);

#if ALT_TIMINGS
    rate = 1;
#else
    rate = 44100. * 79 / rate;
#endif

    tape_reset();
    int level = !!flags;

    while( fp < eot ) {
        unsigned pulses = *fp++;
        if( pulses == 0 ) {
            pulses  = (*fp++);
            pulses |= (*fp++)*0x100;
            pulses |= (*fp++)*0x10000;
            pulses |= (*fp++)*0x1000000;
        }

        // tape_render_pilot(pulses);
        tape_push("piLot", level ? LEVEL_HIGH : LEVEL_LOW, pulses, rate); level ^= 1;
    }

    tape_finish();
    return 1;
}
