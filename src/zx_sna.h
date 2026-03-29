static void *p = 0;
#define mread8()  (*src++)
#define mread16() (p = realloc(p, 2), 0[(byte*)p]=*src++, 1[(byte*)p]=*src++, *((unsigned short*)p))
#define mreadnum(n) (memcpy(p = realloc(p, (n)), src, (n)), src += (n), p)
//#define fread8()  (v = fgetc(fp), v)
//#define fread16() (v = fgetc(fp), v |= fgetc(fp) << 8, v)
//#define freadnum(n) (fread(p = realloc(p, (n)), 1, (n), fp), p)

int rom_load(const byte *src, int len) { // interface2 cartridge / rom (16k/32k/64k)
    if(!( src && (len == 16384 || len == 32768 || len == 65536) )) 
        return 0;

    // ensure this is a zx rom (DI opcode)
    if( !(*src == 0xF3) )
        return 0;

    boot(len > 32768 ? 210 : len > 16384 ? 128 : 48, 0); // @fixme: needed?. 300?
    /*
    page128 &= ~32;
    port_0x7ffd(32|16);
    */
    ZXBorderColor = 0;
    PC(cpu) = 0;
    memcpy(rom, src, len);
    pins = z80_prefetch(&cpu, cpu.pc);
    return 1;
}

#if 0
.if1 should go into byte shadow_rom[8192];

memcpy(rom+16384, rom, 16384);
memcpy(rom+16384, if1, if1sz);

READ:
    return rom[addr + 16384 * (mdr_active && mdr_paged)]; // shadow rom in 2nd 16k page

int if1_load(const byte *src, int len) { // interface1 cartridge (8k max)
    if(!(src && len > 2 && len <= 8192) )
        return 0;
    if( memcmp(src, "\x3\0", 2) ) {
        return 0;
    }

    boot(16, 0); // @fixme: needed?
    /*
    page128 &= ~32;
    port_0x7ffd(32|16);
    */
    ZXBorderColor = 0;
    PC(cpu) = 0;
    memset(rom, 0, 16384);
    memcpy(rom, src, len);
    pins = z80_prefetch(&cpu, cpu.pc);
    return 1;
}
#endif

int scr_load(const byte *src, int len) { // screenshot
    // @todo: .ifl (multicolor8x2 9216 = 6144+768*4)
    // @todo: .mc/.mlt (multicolor8x1 12288 = 6144+768*8)
    // @todo: ulaplus+ screen$ (6912+64)
    if(len != 6912) return 0;

    memcpy(rom, "\xF3\x00\x00\x76\x00\x00\x00", 7); // di, nop2, halt, nop3
    memcpy(VRAM, src, 6912);

    // detect border color based on 4 corners. if they mismatch, likely 0 would just fit better
    byte *chr0 = &VRAM[6144+0], *chr1 = chr0 + 31, *chr2 = chr0 + 767, *chr3 = chr2 - 31; 
    int paper0 = (*chr0 >> 3) & 7, paper1 = (*chr1 >> 3) & 7, paper2 = (*chr2 >> 3) & 7, paper3 = (*chr3 >> 3) & 7;
    ZXBorderColor = paper0 == paper1 && paper1 == paper2 && paper2 == paper3 ? paper0 : 0;

    PC(cpu) = 0;
    pins = z80_prefetch(&cpu, cpu.pc);
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

//boot(size == 49179 ? 48 : 128, 0);
    page128 = 0; port_0x7ffd(32|16);

    I(cpu) = mread8();
    HL2(cpu) = mread16();
    DE2(cpu) = mread16();
    BC2(cpu) = mread16();
    AF2(cpu) = mread16();
    HL(cpu) = mread16();
    DE(cpu) = mread16();
    BC(cpu) = mread16();
    IY(cpu) = mread16();
    IX(cpu) = mread16();

    int iff2 = mread8();
    IFF1(cpu) =
    IFF2(cpu) = !!(iff2 & 0x04);

    R(cpu) = mread8();
    AF(cpu) = mread16();
    SP(cpu) = mread16();
    IM(cpu) = mread8() & 3; if(IM(cpu) == 3) IM(cpu) = 2;
    ZXBorderColor = mread8(); // & 7;

    memcpy(RAM_BANK(5), mreadnum(16384), 16384);
    memcpy(RAM_BANK(2), mreadnum(16384), 16384);
    memcpy(RAM_BANK(0), mreadnum(16384), 16384);

    // 128k
    if( size != 49179 ) {
        unsigned short pc = mread16();
        byte page = mread8();
        byte trdos128 = !!mread8();

        PC(cpu) = pc;
        page128 = 0; port_0x7ffd(page); page &= 7;

        if( page ) {
            memcpy(RAM_BANK(page), RAM_BANK(0), 16384);
        }
        for( int i = 0; i < 8; ++i ) {  // pentagon has up to 16 pages
            if( i == 2 || i == 5 || i == page ) continue;
            memcpy(RAM_BANK(i), mreadnum(16384), 16384);
        }

        if( trdos128 ) MEMr[0] = rom + 0x4000 * 2;

#if 0
        // amend some games that rely on AY, which cannot be not saved in .sna format
        // see: tai-pan 128k (title screen). probably matchday2 (0x5E), Tank, and other speedlock games.
        if(1) ay_registers[7] = 0x00b8;
        if(1) ay_registers[15] = 0x00e5;
#endif
    }
    else {
        // Rui Ribeiro's fix as seen in CSS FAQ
        unsigned short sp = SP(cpu);
        unsigned short pc = READ16(sp);
        PC(cpu) = pc;

        // if( sp < 0x4000 || sp == 0xffff ) return 0;
        if(sp >= 0x4000) WRITE16(sp, 0);
        if(sp <= 0xfffd) SP(cpu) = sp+2;
    }

#if !NEWCORE
    EI(cpu) = IFF1(cpu);
#endif

#if 0 // def NEWCORE
    AF(cpu) = bswap16(AF(cpu));
    AF2(cpu) = bswap16(AF2(cpu));
#endif

    outport(0xFE, ZXBorderColor);

#if DEV
//    puts(regs("sna_load"));
#endif

    pins = z80_prefetch(&cpu, cpu.pc);
    return 1;
}

enum { Z80V1_RAW, Z80V1_RLE, Z80V2_RAW = 3, Z80V2_RLE };

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
                memset(dest+pc, val, rep); pc += rep;
            }
        }

        if (type == Z80V1_RLE)    if(pc>=limit) return;
        if (type == Z80V2_RLE) if(count>=limit) return;
    }
}
int z80_guess(const byte *source_, int len) {
    byte buffer[86+10], ver = 0;
    memcpy(buffer, source_, 87);

    if( buffer[12]==255 ) buffer[12]=1; /*as told in CSS FAQ / .z80 section */

    unsigned pc = buffer[7]<<8|buffer[6];
    if(! pc ) {
        pc = buffer[33]<<8|buffer[32];
        if( !strchr("\x17\x36\x37", buffer[30]) )
            alert(va(".z80 unknown version: %d\n", buffer[30]));
        ver = buffer[30] == 0x17 ? 2 : 3;
    }

    if( ver < 2 ) {
        return 48;
    }

    // common extended values (v2+v3)
    /**/ if(buffer[34]== 7) return buffer[37]&0x80 ? 210 : 300; //OK!
    else if(buffer[34]== 8) return buffer[37]&0x80 ? 210 : 300; //OK!
    else if(buffer[34]== 9) return 128|1; // Pentagon 128k
    else if(buffer[34]==10) return alert(va(".z80 submodel not supported: %d", buffer[34])), 0; // Scorpion 256k
    else if(buffer[34]==11) return alert(va(".z80 submodel not supported: %d", buffer[34])), 0; // Didaktik-Kompakt
    else if(buffer[34]==12) return 200; //OK!
    else if(buffer[34]==13) return 210; //OK!
    else if(buffer[34]==14) return alert(va(".z80 submodel not supported: %d", buffer[34])), 0; // TC2048
    else if(buffer[34]==15) return alert(va(".z80 submodel not supported: %d", buffer[34])), 0; // TC2068
    else if(buffer[34]==16) return alert(va(".z80 submodel not supported: %d", buffer[34])), 0; // TS2068
    else {
    // v2 hw, v3 hw or Unknown hardware
    /**/ if(buffer[34]== 0 && ver>=2) return buffer[37]&0x80 ? 16 : 48; //OK!
    else if(buffer[34]== 1 && ver>=2) return buffer[37]&0x80 ? 16 : 48; //MISSING 48+IF1
    else if(buffer[34]== 2 && ver>=2) return buffer[37]&0x80 ? 16 : 48; //MISSING 48+SAMRAM
    else if(buffer[34]== 3 && ver>=2) return buffer[37]&0x80 ?200 :128; //OK!
    else if(buffer[34]== 4 && ver>=2) return buffer[37]&0x80 ?200 :128; //MISSING 128+IF1:
    else if(buffer[34]== 0 && ver!=2) return buffer[37]&0x80 ? 16 : 48; //OK!
    else if(buffer[34]== 1 && ver!=2) return buffer[37]&0x80 ? 16 : 48; //MISSING 16+IF1 & 48+IF1:
    else if(buffer[34]== 2 && ver!=2) return buffer[37]&0x80 ? 16 : 48; //MISSING 16+SAMRAM & 48+SAMRAM
    else if(buffer[34]== 3 && ver!=2) return buffer[37]&0x80 ? 16 : 48; //MISSING 16+MGT & 48+MGT:
    else if(buffer[34]== 4 && ver!=2) return buffer[37]&0x80 ?200 :128; //OK!
    else if(buffer[34]== 5 && ver!=2) return buffer[37]&0x80 ?200 :128; //MISSING +2+IF1 & 128+IF1 :
    else if(buffer[34]== 6 && ver!=2) return buffer[37]&0x80 ?200 :128; //MISSING 128 + MGT:
    else                              return 48;
    }
}
int z80_load(const byte *source_, int len) {
    int f, tam, sig;
    byte buffer[86+10], pag, ver = 0, ver_rle = 0;
    const byte *source = source_;
    const byte *end = source_ + len;

    memcpy(buffer, source, 87); // was, 86 before

    if( buffer[12]==255 ) buffer[12]=1; /*as told in CSS FAQ / .z80 section */

    // Check file version
    unsigned pc = buffer[7]<<8|buffer[6];
    if( !pc ) {
        pc = buffer[33]<<8|buffer[32];
        ver = buffer[30] == 23 ? 2 : 3;
    }

    // Config pages
    if( ver < 2 ) {
        // .z80 v1.45 or earlier
        source=source_; source+=30;

        // 48K only
        char *pages3 = malloc(0x4000*3);
        z80_unrle((buffer[12] & 0x20 ? Z80V1_RLE : Z80V1_RAW), pages3, source, 0);
        memcpy(RAM_BANK(5), pages3+0x4000*0, 0x4000);
        memcpy(RAM_BANK(2), pages3+0x4000*1, 0x4000);
        memcpy(RAM_BANK(0), pages3+0x4000*2, 0x4000);
        free(pages3);
    } else {

        // common extended values (v2+v3)
        if(ZX>=128) {
            //  if the word [30] is 23:
            for(int psg=0;psg<16;psg++) { port_0xfffd(psg); port_0xbffd(buffer[39+psg]); }
            port_0xfffd(buffer[38]);

            if(buffer[30] == 55) port_0x1ffd(buffer[86]);

            port_0x7ffd(buffer[35]);
        }

        sig = 30 + 2 + buffer[30]; // [30] should be 16bits

        for (f = 0; f < 16 ; f++) { //up 16 pages (ZS Scorpion)
            byte *target=NULL;

            source=source_; source+=sig;

            tam = *source++; 
            tam = tam + ((*source++) << 8);
            pag = *source++; 
            sig += 3 + tam;
if (tam==65535) sig-=49151;

            if(ZX<128)
            switch(pag) {
             case 1:  break; //Interface I/disciple/plus D rom
             case 11: break; //multiface rom

             //case 0:  target=ROM_BANK(0); break;
             case 8:  target=RAM_BANK(5); break;
             case 4:  target=RAM_BANK(2); break; // (2)?
             case 5:  target=RAM_BANK(0); break; // (3)?
             default: break;
            }
            else
            switch(pag) {
             case 1:  break; //Interface I/disciple/plus D rom
             case 11: break; //multiface rom

             //case 0:  target=ROM_BANK(1); break;
             //case 2:  target=ROM_BANK(0); break;
             default: if((pag>=3) && (pag<=10)) // [3..18] pages for scorpion
                                target=RAM_BANK(pag-3); break;
            }

            if(target!=NULL)
             z80_unrle(ver_rle = (tam == 0xffff ? Z80V2_RAW : Z80V2_RLE), target, source, tam);

            if(source_+sig>=end) break;
        }
    }

    PC(cpu) = pc;

    AF(cpu) = buffer[1]<<8|buffer[0];
    BC(cpu) = buffer[3]<<8|buffer[2];
    HL(cpu) = buffer[5]<<8|buffer[4];

    SP(cpu) = buffer[9]<<8|buffer[8];
    I(cpu) = buffer[10];
    R(cpu) = (buffer[11] & 0x7F) | ((buffer[12] & 1) << 7);
    ZXBorderColor = (buffer[12] >> 1); // & 7;

    DE(cpu) = buffer[14]<<8|buffer[13];
    BC2(cpu) = buffer[16]<<8|buffer[15];
    DE2(cpu) = buffer[18]<<8|buffer[17];
    HL2(cpu) = buffer[20]<<8|buffer[19];
    AF2(cpu) = buffer[22]<<8|buffer[21];
    IY(cpu) = buffer[24]<<8|buffer[23];
    IX(cpu) = buffer[26]<<8|buffer[25];

#if NEWCORE
    IFF1(cpu) = !!buffer[27];
#else
    EI(cpu) = !!buffer[27];
    IFF1(cpu) = 
#endif
    IFF2(cpu) = !!buffer[28];

    IM(cpu) = buffer[29] == 0xFF ? 1 : buffer[29] & 0x03;
    if(IM(cpu) == 3) IM(cpu) = 2;

    outport(0xFE, ZXBorderColor);

#if 1 // def NEWCORE
    AF(cpu) = bswap16(AF(cpu));
    AF2(cpu) = bswap16(AF2(cpu));
#endif

    printf("z80 v%d (rle:%d) (machine:%d)\n", ver, ver_rle, buffer[34]);

#if DEV
//    puts(regs("z80_load"));
#endif

    pins = z80_prefetch(&cpu, cpu.pc);
    return 1;
}


void pok_apply(const char *pok) { // :POKE8,38373,39,0:POKE8,38374,256,3
    while(*pok) {
        while(!isdigit(*pok)) pok++;

        int bank, addr, value, defaults;
        int args = sscanf(pok, "%d,%d,%d,%d", &bank, &addr, &value, &defaults);
        if( args == 4 ) {
            if( value == 256 )
            value = (byte)atoi(prompt("Enter value", "", va("%d",defaults)));

            if( bank & 8 )
            WRITE8(addr, value);
            else
            RAM_BANK(bank&7)[addr & 0x3FFF] = value;
        }

        while(*pok && *pok != ':') ++pok;
    }
}

int pok_load(const byte *src, int len) {
    /*
    NInfinite lives
    M  8 41047   0  53
    Z  8 41046 195  91
    NSet number of lives
    Z  8 23343 256   0
    Y

    lbbb aaaaa vvv ooo
    Where l determines the content, bbb is the bank, aaaaa is the address to be poked with value vvv and ooo is the original 
    value of aaaaa. All values are decimal, and separated by one or more spaces, apart from between l and bbb; however, 
    the bank value is never larger than 8, so you will always see 2 spaces in front of the bank. The field bank field is
    built from;

    bit 0-2 : bank value
    bit 3 : ignore bank (1=yes, always set for 48K games)

    If the field [value] is in range 0-255, this is the value to be POKEd. If it is 256, a requester should pop up where
    the user can enter a value.
    */
    if( *src != 'N' ) return 0; // not a .pok file

    ui_dialog_new("-Select Cheat-");

    char pokes[256] = {0};
    char trainer[256] = {0};
    char line[256] = {0};
    while( strchr("NMZY", *src) && sscanf(src, "%[^\r\n]", line) > 0 ) {
        src += strlen(line);
        while( strchr("\r\n", *src) ) ++src;

        if( line[0] == 'N' ) { if(trainer[0]) ui_dialog_option(1,va("%s\n",trainer),NULL,'POKE',pokes); strcpy(trainer, line+1); pokes[0] = 0; continue; }
        if( line[0] == 'Y' ) { if(trainer[0]) ui_dialog_option(1,va("%s\n",trainer),NULL,'POKE',pokes); ui_dialog_separator(), ui_dialog_option(1,"Cancel\n",NULL,0,NULL); return 1; }

        int bank, addr, value, defaults;
        if( 4 != sscanf(line+1, "%d %d %d %d", &bank, &addr, &value, &defaults) ) break;

        // int current = bank & 8 ? READ8(addr) : RAM_BANK(bank&7)[addr & 0x3FFF];
        // if( current != defaults ) { alert("poke mismatch"); continue; }

        enum { disallow_prompts = 1 };
        if( disallow_prompts )
        if( value == 256 ) trainer[0] = '\0';

        strcat(pokes,va(":%d,%d,%d,%d",bank,addr,value,defaults));
    }

    puts(".pok error");
    ui_dialog_new(NULL);
    return 0;
}

int szx_load(const byte *src, int len) {
    const byte *eof = src + len;

    if( len <= 0x08 || memcmp(src, "ZXST", 4))
        return 0;

    src += 4;

    int models[17] = { 16,48,128,200,210,300,300,128|1,0*2048,0*2068,0*'scor',0*'se',0*2068,0*512,0*1024,48,128};

    byte major = *src++;
    byte minor = *src++;
    int machine = models[*src++];
    int flags = *src++; // alt_timings: == 1

    unsigned size;
    char id[5] = {0};
    int fix_af = 0;

    for( ; src < eof; ) {
        memcpy(id, src, 4); src += 4;
        memcpy(&size, src, 4); src += 4;

        const byte *p = src;
        src += size;

        if( !strcmp(id, "CRTR") ) {
            // Early versions of libspectrum (prior to 1.0.0) mistakengly swapped A<>F A'<>F' pairs
            fix_af = !strcmp(p, "libspectrum: 0.");
        }
        else
        if( !strcmp(id, "Z80R") ) {
            int O = fix_af, l = !O;

            AF(cpu)  = p[l] * 256 + p[O]; p += 2;
            BC(cpu)  = p[1] * 256 + p[0]; p += 2;
            DE(cpu)  = p[1] * 256 + p[0]; p += 2;
            HL(cpu)  = p[1] * 256 + p[0]; p += 2;
            AF2(cpu) = p[l] * 256 + p[O]; p += 2;
            BC2(cpu) = p[1] * 256 + p[0]; p += 2;
            DE2(cpu) = p[1] * 256 + p[0]; p += 2;
            HL2(cpu) = p[1] * 256 + p[0]; p += 2;
            IX(cpu)  = p[1] * 256 + p[0]; p += 2;
            IY(cpu)  = p[1] * 256 + p[0]; p += 2;
            SP(cpu)  = p[1] * 256 + p[0]; p += 2;
            PC(cpu)  = p[1] * 256 + p[0]; p += 2;
            IR(cpu)  = p[1] * 256 + p[0]; p += 2;

            IFF1(cpu) = *p++;
            IFF2(cpu) = *p++;
            IM(cpu) = *p++;

            enum { ZXSTZF_EILAST = 1, ZXSTZF_HALTED = 2, ZXSTZF_FSET = 4 };

            // @fix: DWORD dwCyclesStart;
            // @fix: BYTE chHoldIntReqCycles;
            // @fix: BYTE chFlags;
            // @fix: WORD wMemPtr;
        }
        else
        if( !strcmp(id, "SPCR") ) {
            byte border = *p++;
            byte p7ffd = *p++;
            byte p1ffd = *p++;
            byte pfe = *p++;

            outport(0x7ffd, p7ffd);
            outport(0x1ffd, p1ffd);
            outport(0xfe, pfe);
            ZXBorderColor = border;
        }
        else
        if( !strcmp(id, "RAMP") ) {
            enum { ZXSTRF_COMPRESSED = 1 };
            word flags = p[1] * 256 + p[0]; p += 2;
            byte page = *p++;

            if( flags & ZXSTRF_COMPRESSED ) {
                // deflate_decode(p, size - 3, MEMr[page], 16384);
                mz_ulong cap = 16384;
                int ok = mz_uncompress(RAM_BANK(page), &cap, p, size - 3) == MZ_OK;
            } else {
                memcpy(RAM_BANK(page), p, 16384);
            }
        }
    }

    return 1;
}

#if 1
#define DISK_SECTOR_SIZE 256
#define DISK_CATALOG_OFFSET 0x2000
#define DISK_MAX_FILES 128
#define DISK_MAX_SECTORS 2544 // Max sectors for a 640 KB disk

// Check if a string is a valid TR-DOS filename (printable ASCII or spaces)
int is_valid_trd_filename(const unsigned char *name) {
    for (int i = 0; i < 8; i++) {
        if (name[i] != 0x20 && (name[i] < 0x20 || name[i] > 0x7F)) {
            return 0; // Not a printable ASCII character or space
        }
    }
    return 1;
}

// Check if data is TRD (TR-DOS disk image) by scanning file entries
int is_trd(const unsigned char *data, size_t size) {
    // Minimum size to check file entries (at least one sector)
    if (size < DISK_SECTOR_SIZE) return 0;

    // Scan sectors 0-7 (0x0000-0x1FFF) for file entries
    int found_valid_entry = 0;
    for (size_t offset = 0; offset < 0x2000 && offset < size; offset += 16) {
        const unsigned char *entry = data + offset;
        
        // Check filename (bytes 0-7)
        if (!is_valid_trd_filename(entry)) continue;
        
        // Check file type (byte 8: 'B', 'C', 'D', '#', etc.)
        unsigned char file_type = entry[8];
        if (file_type != 'B' && file_type != 'C' && file_type != 'D' && file_type != '#') continue;
        
        // Check length (bytes 9-10)
        unsigned short length = entry[9] | (entry[10] << 8);
        if (length == 0) continue; // Length must be non-zero
        
        // Check sector and track (bytes 13-14)
        unsigned char sector = entry[13];
        unsigned char track = entry[14];
        if (sector > 15 || track > 79) continue; // Invalid sector/track
        
        found_valid_entry = 1;
        break; // Found at least one valid entry
    }
    
    if (found_valid_entry) return 1;

    // Fallback: Check catalog at 0x2000 if present (optional)
    if (size >= DISK_CATALOG_OFFSET + DISK_SECTOR_SIZE) {
        const unsigned char *catalog = data + DISK_CATALOG_OFFSET;
        unsigned char disk_type = catalog[8];
        unsigned char num_files = catalog[9];
        unsigned short free_sectors = catalog[10] | (catalog[11] << 8);
        
        // Relaxed check: Accept any non-zero disk_type and cap free_sectors
        if (disk_type != 0 && num_files <= DISK_MAX_FILES && free_sectors <= DISK_MAX_SECTORS * 2) {
            return 1;
        }
    }
    return 0;
}

// Check if data is SCL (Simplified TR-DOS image)
int is_scl(const unsigned char *data, size_t size) {
    if (size < 9) return 0;
    return memcmp(data, "SINCLAIR", 8) == 0 && data[8] <= DISK_MAX_FILES;
}

// Check if data is Hobeta (.$b, .$c, .$d)
int is_hobeta(const unsigned char *data, size_t size) {
    if (size < 17 || size > 100000) return 0;
    
    if (data[15] != 0x00) return 0;
    
    unsigned char checksum = 0;
    for (int i = 0; i < 16; i++) checksum += data[i];
    
    unsigned short file_length = data[11] | (data[12] << 8);
    return checksum == data[16] && (file_length + 17) == size;
}

// Check if data is FDI (Formatted Disk Image)
int is_fdi(const unsigned char *data, size_t size) {
    if (size < 3) return 0;
    return memcmp(data, "FDI", 3) == 0;
}

// Check if data is IMG (Raw TR-DOS image)
int is_img(const unsigned char *data, size_t size) {
    if (size < DISK_CATALOG_OFFSET || size > 1000000 || size % DISK_SECTOR_SIZE != 0) return 0;
    return is_trd(data, size); // IMG shares TR-DOS catalog structure
}

// Check if data is MGT (+D disk image)
int is_mgt(const unsigned char *data, size_t size) {
    if (size < DISK_SECTOR_SIZE) return 0;
    
    // Check for valid MGT catalog entries (32-byte entries, first byte 0x00-0x0F or 0xFF)
    for (int i = 0; i < DISK_SECTOR_SIZE; i += 32) {
        if (data[i] != 0xFF && data[i] > 0x0F) return 0;
    }
    return 1;
}

// Identify format from data array
const char *identify_disk(const unsigned char *data, size_t size) {
    if (is_scl(data, size)) return "SCL";
    if (is_fdi(data, size)) return "FDI";
    return NULL;
}
const char *identify_headerless_disk(const unsigned char *data, size_t size) {
    const char *ext = identify_disk(data,size);
    if( ext ) return ext;
    if (is_trd(data, size)) return "TRD";
    if (is_hobeta(data, size)) return "Hobeta";
    if (is_img(data, size)) return "IMG";
    if (is_mgt(data, size)) return "MGT";
    return NULL;
}

#endif

int guess_v1(const byte *ptr, int size) { // guess required model type for given data
    // ay first
    if( size > 0x08 &&(!memcmp(ptr, "ZXAYEMUL", 8) )) return 128; // @fixme: 129? TurboAY?

    // szx
    if( size > 0x08 &&!(memcmp(ptr, "ZXST", 4))) {
        int models[17] = { 16,48,128,200,210,300,300,128|1,0,0,0,0,0,0,0,48,128};
        return ptr[7] < 17 ? models[ptr[7]] : 0;
    }

    // rzx
    if( size > 0x04 &&(!memcmp(ptr, "RZX!", 4) )) return 128; // @fixme: parse inner .z80/.sna instead

    // dsk first
    if( size > 0x08 &&(!memcmp(ptr, "MV - CPC", 8) || !memcmp(ptr, "EXTENDED", 8)) ) return 300;

    // tapes first
    int find_bas = 0;
    if( size > 0x17 && !memcmp(ptr, "Compressed Square Wave\x1a", 0x17) ) return 0;
    if( size > 0x04 && !memcmp(ptr, "\x13\x00\x00\x03", 4) ) return 48; // LOAD""CODE, this is either a 16k or 48k old game
    if( size > 0x10 && !memcmp(ptr, "ZXTape!\x1a", 8) ) if(memmem(ptr, 0x10, "\x13\x00\x00\x03", 4)) return 48; // LOAD""CODE
    if( size > 0x02 && !memcmp(ptr, "\x13\x00", 2) ) find_bas = 0x04;
    if( size > 0x08 && !memcmp(ptr, "ZXTape!\x1a", 8) ) find_bas = ((const byte*)memmem(ptr, size-8, "\x13\x00\x00\x00", 4) - ptr) + 0x04;
    if( size > 0x04 && !memcmp(ptr, "PZXT", 4) ) find_bas = ((const byte*)memstr(ptr, size-4, "DATA") - ptr) + 0x1A;
    if( find_bas ) {
        if( find_bas > 0 && find_bas < size && memstr(ptr+find_bas, 10, "48") )  return  48;
        if( find_bas > 0 && find_bas < size && memstr(ptr+find_bas, 10, "128") ) return 128;
        return 0;
    }

    // roms
    if( *ptr == 0xF3 )
    {
        if( size == 16384 ) return 48;
        if( size == 32768 ) return 128;
        if( size == 65536 ) return 210; // 300?
    }

    // headerless fixed-size formats now, sorted by ascending file size.
    if( size == 6912 ) return 48;
    if( size == 49179 ) return 48;
    if( size == 131103 ) return 128|ZX_PENTAGON; // @fixme: force ZX_PENTAGON for .sna?
    if( size == 147487 ) return 128|ZX_PENTAGON; // @fixme: force ZX_PENTAGON for .sna?

    // disk images with headers (fdi,scl)
    if( identify_disk(ptr,size) ) return 128|1;

    // headerless variable-size formats now
    if( size < 132000 ) {
    if( *ptr == 'N' ) return 0; // .pok
    int rc = z80_guess(ptr, size); // .z80?
    if( rc ) return rc;
    }

    // @fixme: trd/scl/fdi/mgt headers to parse?
    // at this point file is too large to be a snapshot. must be a disk instead (trd,hobeta,mgt,etc)
    if( identify_headerless_disk(ptr,size) ) return 128|1;
    if( size > 147487 ) return 128|1;

    return 0;
}
