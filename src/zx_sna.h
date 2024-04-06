static void *p = 0;
#define mread8()  (*src++)
#define mread16() (p = realloc(p, 2), 0[(byte*)p]=*src++, 1[(byte*)p]=*src++, *((unsigned short*)p))
#define mreadnum(n) (memcpy(p = realloc(p, (n)), src, (n)), src += (n), p)
//#define fread8()  (v = fgetc(fp), v)
//#define fread16() (v = fgetc(fp), v |= fgetc(fp) << 8, v)
//#define freadnum(n) (fread(p = realloc(p, (n)), 1, (n), fp), p)

int rom_load(const byte *src, int len) { // interface2 cartridge
    if( src && len == 16384 ) {
        reset(48);
        /*
        page128 &= ~32;
        port_0x7ffd(32|16);
        */
        ZXBorderColor = 0;
        cpu.pc = 0;
        memcpy(rom, src, len);
        return 1;
    }
    return 0;
}

int scr_load(const byte *src, int len) { // screenshot
    if(len != 6912) return 0;
#if 0
    reset(48);
#else
    reset(48);
    /*
    page128 &= ~32;
    port_0x7ffd(32|16);
    */
    ZXBorderColor = 0;
    cpu.pc = 0;
#endif
    memcpy(rom, "\xF3\x00\x00\x76\x00\x00\x00", 7); // di, nop2, halt, nop3
    memcpy(VRAM, src, 6912);
    return 1;
}

int sna_load(const byte *src, int size) {
    /*
        SNA128
        
        Offset   Size   Description
        ------------------------------------------------------------------------
        0        27     bytes  SNA header (see above)
        27       16Kb   bytes  RAM bank 5 \
        16411    16Kb   bytes  RAM bank 2  } - as standard 48Kb SNA file
        32795    16Kb   bytes  RAM bank n / (currently paged bank)
        49179    2      word   PC
        49181    1      byte   port 0x7ffd setting
        49182    1      byte   TR-DOS rom paged (1) or not (0)
        49183    16Kb   bytes  remaining RAM banks in ascending order
          ...
        ------------------------------------------------------------------------
        Total: 131103 or 147487 bytes
    */
    if( size != 49179 && size != 131103 && size != 147487 ) return 0;

reset(size == 49179 ? 48 : 128);
    page128 = 0; port_0x7ffd(32|16);

    cpu.i = mread8();
    cpu.hl2 = mread16();
    cpu.de2 = mread16();
    cpu.bc2 = mread16();
    cpu.af2 = mread16();
    cpu.hl = mread16();
    cpu.de = mread16();
    cpu.bc = mread16();
    cpu.iy = mread16();
    cpu.ix = mread16();

    int iff2 = mread8();
    cpu.iff1 = (iff2 & 0x04) > 0 ? 1 : 0;
    cpu.iff2 = (iff2 & 0x04) > 0 ? 1 : 0;

    cpu.r = mread8();
    cpu.af = mread16();
    cpu.sp = mread16();
    cpu.im = mread8() & 3;
    ZXBorderColor = mread8() & 7;
    memcpy(RAM_BANK(5), mreadnum(16384), 16384);
    memcpy(RAM_BANK(2), mreadnum(16384), 16384);
    memcpy(RAM_BANK(0), mreadnum(16384), 16384);

    // 128k
    if( size != 49179 ) {
        unsigned short pc128 = mread16();
        byte pg128 = mread8();
        byte trdos128 = !!mread8();
cpu.pc = pc128;
        page128 = 0; port_0x7ffd(pg128);
        if( pg128 & 7 ) {
        memcpy(RAM_BANK(pg128&7), RAM_BANK(0), 16384);
        }
        for( int i = 0; i < 8; ++i ) {
            if( i == 2 || i == 5 ) continue;
            if( i == (pg128 & 7) ) continue;
            memcpy(RAM_BANK(i), mreadnum(16384), 16384);
        }
        for( int i = 0; i < 8; ++i ) {
        //    printf("RAMBANK-%d) [%02x]\n", i, RAM_BANK(i)[0]);
        }
    } else {
        // Rui Ribeiro's fix as seen in CSS FAQ
        unsigned short sp = cpu.sp, sp_bak = sp;
        unsigned short pc = READ16(sp);
        cpu.pc = pc;
        if(sp <= 0xfffd) cpu.sp = sp+2;
        if(sp >= 0x4000) WRITE16(sp_bak, 0);
    }

    return 1;
}

enum { Z80V1_RAW, Z80V1_RLE, Z80V2_RAW, Z80V2_RLE };

void z80_unrle(int type, byte *dest, const byte* src, int len) {
    int limit;

    if(type == Z80V1_RAW || type == Z80V2_RAW) {
        memcpy(dest, src, type == Z80V1_RAW ? 0xc000 : 0x4000);
        return;
    }
    else if(type == Z80V1_RLE) {
        memset(dest,0,0xc000);
        limit = 0 + 0xc000;
    }
    else { // Z80V2_RLE
        memset(dest,0,0x4000);
        limit = 0 + len;
    }

    for(int pc=0, count=0;;) {
        byte c1=src[count++];

        if( c1 != 237 ) dest[pc++]=c1;   // if not ED...
        else {
            byte c2=src[count++];
            if( c2 != 237 ) dest[pc++]=c1,dest[pc++]=c2;  // if not ED ED...
            else {
                byte rep=src[count++];
                byte val=src[count++];
                //memset(dest+pc, val, rep); pc += rep;
                for(int x=0;x<rep;x++) dest[pc++]=val;
            }
        }

        if (type == Z80V1_RLE)    if(pc>=limit) return;
        if (type == Z80V2_RLE) if(count>=limit) return;
    }
}

//#define reset(x) reset_quick(x)

int z80_load(const byte *source_, int len) {
    int f, tam, sig;
    byte buffer[86+10], pag, ver = 0;
    const byte *source = source_;
    const byte *end = source_ + len;

    memcpy(buffer, source, 86); 

    if( buffer[12]==255 ) buffer[12]=1; /*as told in CSS FAQ / .z80 section */

    // Check file version
    if( buffer[6] != 0 ) {
        reset(48);

        // .z80 v1.45 or earlier
        source=source_; source+=30;

        char *pages3 = malloc(0x4000*3);
        z80_unrle((buffer[12] & 0x20 ? Z80V1_RLE : Z80V1_RAW), pages3, source, 0);
        memcpy(RAM_BANK(0), pages3+0x4000*2, 0x4000);
        memcpy(RAM_BANK(2), pages3+0x4000*1, 0x4000);
        memcpy(RAM_BANK(5), pages3+0x4000*0, 0x4000);
        free(pages3);

        cpu.pc = buffer[7]<<8|buffer[6];
    } else {
        switch( buffer[30] ) {
            case 23:   ver = 2; break;
            case 54: 
            case 55:   ver = 3; break;
            default:            break; //?
        }

        // common extended values (v2+v3)
        /**/ if(buffer[34]== 7) reset(buffer[37]&0x80 ? 210 : 300); //OK!
        else if(buffer[34]== 8) reset(buffer[37]&0x80 ? 210 : 300); //OK!
        else if(buffer[34]== 9) return 0; // Pentagon 128k
        else if(buffer[34]==10) return 0; // Scorpion 256k
        else if(buffer[34]==11) return 0; // Didaktik-Kompakt
        else if(buffer[34]==12) reset(200); //OK!
        else if(buffer[34]==13) reset(210); //OK!
        else if(buffer[34]==14) return 0; // TC2048
        else if(buffer[34]==15) return 0; // TC2068
        else if(buffer[34]==16) return 0; // TS2068
        else {
        // v2 hw, v3 hw or Unknown hardware
        /**/ if(buffer[34]== 0 && ver==2) reset(buffer[37]&0x80 ? 16 : 48); //OK!
        else if(buffer[34]== 1 && ver==2) reset(buffer[37]&0x80 ? 16 : 48); //MISSING 48+IF1
        else if(buffer[34]== 2 && ver==2) reset(buffer[37]&0x80 ? 16 : 48); //MISSING 48+SAMRAM
        else if(buffer[34]== 3 && ver==2) reset(buffer[37]&0x80 ?200 :128); //OK!
        else if(buffer[34]== 4 && ver==2) reset(buffer[37]&0x80 ?200 :128); //MISSING 128+IF1:
        else if(buffer[34]== 0 && ver!=2) reset(buffer[37]&0x80 ? 16 : 48); //OK!
        else if(buffer[34]== 1 && ver!=2) reset(buffer[37]&0x80 ? 16 : 48); //MISSING 16+IF1 & 48+IF1:
        else if(buffer[34]== 2 && ver!=2) reset(buffer[37]&0x80 ? 16 : 48); //MISSING 16+SAMRAM & 48+SAMRAM
        else if(buffer[34]== 3 && ver!=2) reset(buffer[37]&0x80 ? 16 : 48); //MISSING 16+MGT & 48+MGT:
        else if(buffer[34]== 4 && ver!=2) reset(buffer[37]&0x80 ?200 :128); //OK!
        else if(buffer[34]== 5 && ver!=2) reset(buffer[37]&0x80 ?200 :128); //MISSING +2+IF1 & 128+IF1 :
        else if(buffer[34]== 6 && ver!=2) reset(buffer[37]&0x80 ?200 :128); //MISSING 128 + MGT:
        else                              reset(48);
        }

        if(ZX>=128) {
            for(int psg=0;psg<16;psg++) { port_0xfffd(psg); port_0xbffd(buffer[39+psg]); }
            port_0xfffd(buffer[38]);
            if(buffer[30] == 55) outport_0x1ffd(buffer[86]);
            port_0x7ffd(buffer[35]);
        }

        sig = 30 + 2 + buffer[30];

        for (f = 0; f < 16 ; f++) { //up 16 pages (ZS Scorpion)
            byte *target=NULL;

            source=source_; source+=sig;

            tam = *source++; 
            tam = tam + ((*source++) << 8);
            pag = *source++; 
            sig += 3 + tam;

            if(ZX<128)
            switch(pag) {
             case 1:  break; //Interface I/disciple/plus D rom
             case 11: break; //multiface rom

             //case 0:  target=ROM_BANK(0); break;
             case 8:  target=RAM_BANK(5); break;
             case 4:  target=RAM_BANK(2); break;
             case 5:  
            default:  target=RAM_BANK(0); break;

            }
            else
            switch(pag) {
             case 1:  break; //Interface I/disciple/plus D rom
             case 11: break; //multiface rom

             //case 0:  target=ROM_BANK(1); break;
             //case 2:  target=ROM_BANK(0); break;
             default: if((pag>=3) && (pag<=18))
                                target=RAM_BANK(pag-3); break;
            }

            if(target!=NULL)
             z80_unrle((tam == 0xffff ? Z80V2_RAW : Z80V2_RLE), target, source, tam);

            if(source_+sig>=end) break;
        }
        cpu.pc = buffer[33]<<8|buffer[32];
    }

    cpu.af = buffer[1]<<8|buffer[0];
    cpu.bc = buffer[3]<<8|buffer[2];
    cpu.hl = buffer[5]<<8|buffer[4];
    cpu.sp = buffer[9]<<8|buffer[8];
    cpu.i = buffer[10];

    cpu.r = ((buffer[11] & 0x7F) | (buffer[12] & 1 ) << 7);
    ZXBorderColor = (buffer[12] >> 1) & 7;

    cpu.de = buffer[14]<<8|buffer[13];
    cpu.bc2 = buffer[16]<<8|buffer[15];
    cpu.de2 = buffer[18]<<8|buffer[17];
    cpu.hl2 = buffer[20]<<8|buffer[19];
    cpu.af2 = buffer[22]<<8|buffer[21];
    cpu.iy = buffer[24]<<8|buffer[23];
    cpu.ix = buffer[26]<<8|buffer[25];

    cpu.iff1 = buffer[27] > 0 ? 1 : 0;
    cpu.iff2 = buffer[28] > 0 ? 1 : 0;

    cpu.im = buffer[29] & 0x03;

    return 1;
/*
    // double int freq
    // highvideo,lowvideo,normal

    //issue2
    keyboard_issue=flag&4 ? 255 : 191;

    //joystick type
    joy_type=(flag&0xC0)>>6;
    switch(joy_type) {
        case 0 : printf("Cursor\n");break;
        case 1 : printf("Kempston\n");break;
        case 2 : printf("Sinclair 1\n");break;
        case 3 : printf("Sinclair 2\n");break;
    }
*/
}

//#undef reset
