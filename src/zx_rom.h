#include "zx_embed.h"

#define ZX_CUSTOM_ROMS 1

#if ZX_CUSTOM_ROMS
// replaceable roms, and backups
const byte *rom48_ = rom48, *rom48_bak = rom48;
const byte *rom128_ = rom128, *rom128_bak = rom128;
const byte *romplus2_ = romplus2, *romplus2_bak = romplus2;
const byte *romplus341_ = romplus341, *romplus341_bak = romplus341;
const byte *rompentagon128_ = rompentagon128, *rompentagon128_bak = rompentagon128;
#define rom48 rom48_
#define rom128 rom128_
#define romplus2 romplus2_
#define romplus341 romplus341_
#define rompentagon128 rompentagon128_
#endif

#define ROMHACK_TURBO 2.6 // x2 ok; x4,x6,x8 modes not working anymore :(
#define IF_TURBOROM_FASTER_EDGES(...)               __VA_ARGS__ // can be enabled ugh
#define IF_TURBOROM_FASTER_PILOTS_AND_PAUSES(...)   __VA_ARGS__ // can be enabled
#define IF_TURBOROM_HALF_BITS(...)               // __VA_ARGS__ // not working anymore :(
#define IF_TURBOROM_TURBO(...)                      __VA_ARGS__ // can be enabled

// turborom stats
// parapshock: 232s (normal)
// parapshock:  73s (then, 50% processed bits, 6x pilots/pauses, x15 syncs) -> 30s -> 23s
// parapshock:  18s (then, F1 boost)

enum { TURBO_PATCH = 1, ALT_PATCH = 2, SCROLL_PATCH = 4 };
int rom_patches;

int patch(byte *from, byte *to, byte *src, const byte *dst, int len) {
    int hits = 0;
    while( (to - from) > len ) {
        if( memcmp(from, src, len) == 0 )
            memcpy((from += len) - len, dst, len), ++hits;
        else
            ++from;
    }
    return hits;
}

void rom_patch_scroll() {
    if(rom_patches & SCROLL_PATCH) return;
    rom_patches |= SCROLL_PATCH;
    // supress "Scroll?" message: JP #0CD2
    memset(ROM_BASIC()+0x0C93, 0, 0xCD2-0xC93);
}

void rom_restore() {
    rom_patches = 0;

    // [ref] https://speccy.xyz/rom/asm/0556.html

    /**/ if( ZX >= 210) memcpy(rom, romplus341, 0x4000*4);
    else if( ZX >= 200) memcpy(rom, romplus2,   0x4000*2);
    else if( ZX >= 128) memcpy(rom, rom128,     0x4000*2);
    else if( ZX >=  16) memcpy(rom, rom48,      0x4000*1);

    // install pentagon rom on 128 model :o)
    if( ZX == 128 )
    if( ZX_PENTAGON ) {
        memcpy(rom+0x4000*0, rompentagon128, 0x4000*2);
    }

    // install shadow trdos rom in unused slot
    if( ZX == 128 || ZX == 200 ) {
        memcpy(rom+0x4000*2, romtrdos503, 0x4000);
    }

    if(ZX_ALTROMS)
    {
#if 1
    // install plus2c on 128/+2 models
    if( ZX <= 200) memcpy(rom+0x0000, romplus2c, 0x4000), memcpy(rom+0x4000, rom128+0x4000, 0x4000);
    if( ZX <= 200) rom[0x0566] = '6';  // 198(6) Sinclair
    if( ZX <= 200) rom[0x37F6] = 0x00; // black menu titles
    if( ZX <= 200) rom[0x3864] = 0x40; // black banners
    if( ZX <= 200) rom[0x387a] = 0x1b; // shift banner strips +1 right
    if( ZX <= 200) memcpy(rom+0x26F1, rom128+0x26F1, 8); // restore classic keyclick bug sound
    if( ZX <= 200) memcpy(rom+0x2744+11, rom128+0x2744+11, 2); // restore classic rom1 locked in 48 mode
    if( ZX <= 200) rom[0x1B2B+13] = 0x03; // fix error msg on plus2c+gw03/lg18+SPECTRUM command combo; (BORDER q#PI instead of 0 OK) ; $0013 -> $0003 Address of a $FF byte within ROM 1, used to generate error report "0 OK".

    // install gw03 or lg18
    // if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48), romgw03v33/*romjgh077/*romgw03v33/*romlg18v07/*rom48*/, 0x4000);

    // install old sebasic where possible
    if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48), ZX==16 ? rom48 : romsebasic, 0x4000);
    if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48)+0x3D00, rom48+0x3D00, (0x7F-0x20)*8); // restore charset
    if( ZX == 200 || ZX ==128) memset(rom+0x4000*0+0x240, 0x00, 3); // make editor128 work with this rom

    // install jgh where possible
    // if( ZX <= 200) memcpy(ROM_BASIC(), ZX==16 ? rom48 : romjgh077, 0x4000);
    // if( ZX <= 200) memcpy(ROM_BASIC()+0x3D00, rom48+0x3D00, (0x7F-0x20)*8); // restore charset
    // if( ZX <= 200) ROM_BASIC()[0x11CD+1] = 0x38 + 7; // border 7
    // if( ZX <= 200) ROM_BASIC()[0x1265+1] = 0x38; // paper 7: ink 0
    // if( ZX == 200 || ZX ==128) memset(rom+0x4000*0+0x240, 0x00, 3);
#endif

    // install plus2b on +2A model (debugged 128k/original 16k/BBC/SEbasic)
    // if( ZX == 210) memcpy(rom, romplus2b, 0x4000*4);

    // note: rom48, contains a vector FF table in the [0x386E..0x3D00) region
    // if( ZX >= 128) memcpy(rom+0x4000, romplus2+0x4000, 0x4000); //memset(rom+0x4000*0+0x386E, 0xFF, 0x3D00-0x386E);

    // Owen: Changing three instructions to NOP allows the original, unmodified ROM from the Spectrum 48K to be used in place of ROM 1.
    // EF       RST  28H          ; Attempt to display TV tuning test screen.
    // 04 3C    DEFW TEST_SCREEN  ; $3C04. Will return if BREAK is not being pressed.
    // if(ZX==128||ZX==200) memset(rom+0x4000*0+0x240, 0x00, 3);

    // Groot: 0x1539 [(c) 1982 Sinclair Research Ltd]
    // Groot: New NMI routine: Quick start basic without memory erase!
    // memcpy(rom+0x4000 * (ZX > 48)+0x66, "\xF3\xAF\xD3\xFE\x3E\x3F\xED\x47\x2A\xB2\x5C\xC3\x19\x12", 14);

#if 0 // opense basic
    if( ZX <= 200) {
        memcpy(rom+0x0000, rom128, 0x4000);
        // patch x3 nop as described in plus2c.txt (aowen)
        if(ZX==128||ZX==200) memset(rom+0x4000*0+0x240, 0x00, 3);
        // update token tables from TOKENS=$95 to TOKENS=$a9
        patch(rom,rom+0x4000,"\x21\x96\x00","\x21\xAA\x00",3);  // LD   HL,TOKENS+$0001 ; $0096. Token table entry "RND" in ROM 1.
        patch(rom,rom+0x4000,"\x21\xCF\x00","\x21\xE2\x00",3);  // LD   HL,TOKENS+$003A ; $00CF. Token table entry "ASN" in ROM 1.
        patch(rom,rom+0x4000,"\x21\x00\x01","\x21\x13\x01",3);  // LD   HL,TOKENS+$006B ; $0100. Token table entry "OR" in ROM 1.
        patch(rom,rom+0x4000,"\x21\x3E\x01","\x21\x51\x01",3);  // LD   HL,TOKENS+$00A9 ; $013E. Token table entry "MERGE" in ROM 1.
        patch(rom,rom+0x4000,"\x21\x8B\x01","\x21\x9E\x01",3);  // LD   HL,TOKENS+$00F6 ; $018B. Token table entry "RESTORE" in ROM 1.
        patch(rom,rom+0x4000,"\x21\xD4\x01","\x21\xE5\x01",3);  // LD   HL,TOKENS+$013F ; $01D4. Token table entry "PRINT" in ROM 1.
        patch(rom,rom+0x4000,"\x21\x96\x00","\x21\xAA\x00",3);  // LD   HL,TOKENS+1     ; $0096. Address of token table in ROM 1.
    }
    if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48), romsebasic/*romgw03v33/*romlg18v07/*rom48*/, 0x4000);
#endif
    }

#if TESTS
    rom_patch_scroll();
#endif
}

byte TAP_load(byte id_a, unsigned start_ix, unsigned length_de)
{
    extern const byte *TAP_sof, *TAP_pof, *TAP_eof;

    if(!TAP_sof) return 0; /* fail, no file */

    if(TAP_pof >= TAP_eof) TAP_pof = TAP_sof; /* Back to start */
    int remain = (TAP_eof - TAP_pof);
    if(remain<3) return 0;

    unsigned length = (TAP_pof[1] << 8 | TAP_pof[0]) - 2; TAP_pof += 2;
    byte id_tape = *TAP_pof++;
    byte calc_checksum=id_tape;

    if( 1 ) // id_tape == id_a ) /* Yes, the correct ID, proceed. */
    {
        if( length_de <= length ) /* Length <= so OK */
        {
            byte data = 0;

            // read_block(start_ix, length_de)
            for( int i = 0; i < length_de; ++i ) {
                data = *TAP_pof++;
                calc_checksum ^= data;
                WRITE8(start_ix+i, data);
            }

            // skip_block(length-length_de);
            TAP_pof += length-length_de;

            byte checksum = *TAP_pof++;
            HL(cpu) = (checksum << 8) | data;
            IX(cpu) += length_de;
            DE(cpu) = 0; /* checksum in this case? */
            return checksum == calc_checksum;
        }
        else /* Too many bytes requested */
        {
            byte data = 0;

            int excess = length_de-length;

            // read_block(start_ix,length);
            for( int i = 0; i < length_de; ++i ) {
                data = i < excess ? id_tape/*A(cpu)*/ : *TAP_pof++;
                calc_checksum ^= data;
                WRITE8(start_ix+i, data);
            }

            byte checksum = *TAP_pof++;
            HL(cpu) = (0/*checksum*/ << 8) | data;
            IX(cpu) += length_de;
            DE(cpu) = 0;
            //return 0; /* fail */
            return 1; /* ok */
        }
    }
    else /* Wrong ID! */
    {
        // skip_block(length.W);
        TAP_pof += length;
        byte checksum = *TAP_pof++;
        return 0; /* fail */
    }
}
bool TAP_load2(byte id_a, unsigned start_ix, unsigned length_de) {
    unsigned where  = start_ix; //IX(cpu);
    unsigned amount = length_de; //DE(cpu);
    unsigned errors = 0;

    // auto tape-rewind function on end-of-file
    extern const byte *TAP_sof, *TAP_pof, *TAP_eof;
    if( TAP_pof > TAP_eof ) TAP_pof = TAP_sof;
    const byte *src = TAP_pof;

    int bytes = (TAP_pof[1] << 8) | TAP_pof[0]; TAP_pof+=2; //size of header
    if( bytes == 0 ) return 0; // error
    if( bytes == 1 ) return TAP_pof++, 0; // error
    bytes -= 2;

    byte id_tape = *TAP_pof++;
#if 0
    if( id_tape != id_a )
        return TAP_pof += bytes+2-1, 0; // if id not ok then skip block+checksum and return error
#endif

    // if there are more bytes in the tap than requested (ie, Gregory
    // Loses His Clock.tap has a header of 26 bytes instead of 13) then
    // read only the initial bytes and skip later the other bytes.

    DE(cpu) = 0;
    IX(cpu) += amount;

    int skip = amount > bytes ? amount - bytes : 0;
    if( skip ) amount = bytes/*, ++errors*/;

    byte crc = 0 ^ id_tape, last = id_tape, verify = 0; // F(cpu) & Z80_CF;
    for( int i = 0; i < amount+skip; i++ ) {
//        if( i >= skip ) // ? id_tape/*A(cpu)*/ : *TAP_pof++;
        crc ^= ( last = i >= skip ? *TAP_pof++ : last );
//        if( i >= amount ) continue;
        errors += verify ? READ8(where + i) != last : (WRITE8(where + i, last), 0);
    }

last = TAP_pof[0];
//if(skip) MessageBoxA(0,va("%d %d %d %d\n", last, TAP_pof[0], TAP_pof[1], TAP_pof[2]),0,0);

    TAP_pof += bytes - amount; // skip unused bytes, if any

//last = TAP_pof[0];

    byte checksum = *TAP_pof++;
    HL(cpu) = 0/*(checksum << 8)*/ | last;
    BC(cpu) = 0xCB01; //start_ix + (TAP_pof - src) + length_de + 1; //0xCB01;
    PC(cpu) = 0x05E2;

//    if(errors) A(cpu) = 0, F(cpu) &= ~(Z80_CF|Z80_ZF);
//    else       A(cpu) = 0, F(cpu) |=  (Z80_CF|Z80_ZF);

    return errors ? 0 : 1;
}
int rom_patch_trap() {
    if( !ZX_FLASHLOAD ) return 0;

//    byte *rombank = ROM_BANK(GET_BASIC_ROMBANK());
//    if( rombank != rom ) return 0;
    int rombank = GET_MAPPED_ROMBANK();
    int basicbank = GET_BASIC_ROMBANK();
    if( rombank != basicbank ) return 0;

    extern const byte *TAP_sof, *TAP_pof, *TAP_eof;
    if(!TAP_sof) return 0;

    if(PC(cpu)==1388+1) //1388
    {
#if 1
        unsigned af=AF(cpu); AF(cpu) = AF2(cpu);
//        int r = TAP_load(A(cpu),IX(cpu),DE(cpu));
        int r = TAP_load2(A(cpu),IX(cpu),DE(cpu));
        if(!r)
        {
            A(cpu) = 0;
            F(cpu) &= ~(Z80_CF|Z80_ZF);
        }
        else
        {
            A(cpu) = 0;
            F(cpu) |= Z80_ZF;
        }
        AF2(cpu)=AF(cpu);
        AF(cpu)=af;

        af=AF(cpu); AF(cpu)=AF2(cpu); AF2(cpu)=af; /* ex af,af' */
#endif

        PC(cpu)=1506;

        return 1;
    }

    return 0;
}

void rom_patch_turbo() {
    if(rom_patches & TURBO_PATCH) return;

    rom_patches |= TURBO_PATCH;

    byte *rombank = ROM_BANK(GET_BASIC_ROMBANK());

IF_TURBOROM_FASTER_PILOTS_AND_PAUSES(
    // ROMHACK $571 x6 faster pilot pulse
    memcpy(rombank+0x571, "\x21\x01\x00", 3); // LD HL,$0415 -> LD HL,$0105      The length of this waiting period will be almost one second in duration. -> /=4
    memcpy(rombank+0x580, "\x06\x4E",     2); // LD B,$9C                        The timing constant -> /=2
    memcpy(rombank+0x587, "\x3E\x63",     2); // LD A,$C6                        However the edges must have been found within about 3,000 T states of each other. -> /=2
);
IF_TURBOROM_FASTER_EDGES(
    // ROMHACK $5e7 x16 faster edges (sync) (358T->0T)
    memcpy(rombank+0x5E7, "\x3E\x01",     2); // LD A,$16 -> LD A,$1             Wait 358 T states before entering the sampling loop.
    // beware: +2A/+3 is LD A,$C6 !!! 170 extra states
);
IF_TURBOROM_HALF_BITS(
    // ROMHACK $5ca option A: eliminate dupe bits (data) AND 16 faster edges (sync) (358T->0T)
    //memcpy(rombank+0x5CA, "\xCD\xED\x05", 3); // CALL LD_EDGE_2->SAMPLE ($5E3->$5ED)  Find the length of the 'off' and 'on' pulses of the next bit.

    // ROMHACK $5ca option B: eliminate dupe bits (data)
    memcpy(rombank+0x5CA, "\xCD\xE7\x05", 3); // CALL LD_EDGE_2->LD_EDGE1 ($5E3->$5E7)  Find the length of the 'off' and 'on' pulses of the next bit.

    //memcpy(rombank+0x5CA, "\x00\x00\x00\x00", 4);
    //memcpy(rombank+0x5E3, "\x00\x00\x00\x00", 4);
);

IF_TURBOROM_TURBO(
    // ROMHACK $5a5 turbo loader
    // x1 B0,B2,CB,B0 OK
    // x2 58,59,66,58 OK
    // x4 2C,2D,30,2C OK
    // x6 16,17,18,16 OK (no t-=358 required)
    // x8 0B,0B,0C,0B NO 15,15,16,15
    //100 01,02,03,01 NO
    // byte base = 0x58;
    // byte patch1[] = "\x06\x58"; //patch1[1] = base;
    // byte patch2[] = "\x06\x59"; //patch2[1] = base * 1.01136;
    // byte patch3[] = "\x3E\x66"; //patch3[1] = base * 1.1534;
    // byte patch4[] = "\x06\x58"; //patch4[1] = base;

    byte patch[][4] = {
        "\xB0\xB2\xCB\xB0", // x1 OK
        "\xB0\xB2\xCB\xB0", // x1 OK
        "\x58\x59\x66\x58", // x2 OK
        "\x58\x59\x66\x58", // x3 never worked
        "\x2C\x2D\x30\x2C", // x4 was OK
        "\x2C\x2D\x30\x2C", // x5 never worked
        "\x16\x17\x18\x16", // x6 was OK (no t-=358 required)
        "\x16\x17\x18\x16", // x7 never worked
        "\x0B\x0B\x0C\x0B", // x8 never worked 15,15,16,15
        "\x0B\x0B\x0C\x0B", // x9 never worked 15,15,16,15
    };

    rombank[0x5A5 +1] /= ROMHACK_TURBO; // LD B,$B0 -> $XX                 Set the timing constant for the flag byte
    rombank[0x5C6 +1] /= ROMHACK_TURBO; // LD B,$B2 -> $XX                 Set the timing constant.
    rombank[0x5CE +1] /= ROMHACK_TURBO; // LD A,$CB -> $XX                 Compare the length against approx. 2,400 T states, resetting the carry flag for a '0' and setting it for a '1'.
    rombank[0x5D3 +1] /= ROMHACK_TURBO; // LD B,$B0 -> $XX                 Set the timing constant for the next bit.
);
}

void rom_patch_klmode(unsigned pc) {
    // if hot patch needed
    if( ZX_KLMODE_PATCH_NEEDED ) 

    // dont patch K/L mode if trdos is present. both lg18 and trdos rom regions do overlap.
    if( !ZX_PENTAGON )

    {

        // 0x15E1 used to be a stable hook for a long time, which worked in all models too.
        // these addresses are fluctuating now since Z80 INT placement has changed recently
        // @fixme: make them a stable/fixed address that does not need maintenance.
        // @fixme: find another less hacky PC addr other than 0x15E1
        unsigned pcs[] = {
            [16]=0x15E1,
            [48]=0x1600,
            [128]=0x15E7,
            [200]=0x15E7,
            [210]=0x15DF,
            [300]=0x15DF,
        };
        if( pc != pcs[ZX] ) return;

        int rombank = GET_MAPPED_ROMBANK();
        int basicbank = GET_BASIC_ROMBANK();
        if( rombank == basicbank ) 
        {
            ZX_KLMODE_PATCH_NEEDED = 0;

            // install lg18
            memcpy(rom + 0x4000 * basicbank, romlg18v07, 0x4000);
            if(rom_patches & TURBO_PATCH) rom_patch_turbo();
            if(rom_patches & SCROLL_PATCH) rom_patch_scroll();
            rom_patches |= ALT_PATCH;

            // toggle mode
            if( ZX_KLMODE ) { // L
                WRITE8(0x5C6A, READ8(0x5C6A) & ~32 ); // FLAGS2, clear bit 5
                WRITE8(0x5C3B, READ8(0x5C3B) &  (8 | 4) ); // FLAGS, set permanent+transient flag to 'L'
            } else { // K
                WRITE8(0x5C6A, READ8(0x5C6A) | 32 ); // FLAGS2, set bit 5
                WRITE8(0x5C3B, READ8(0x5C3B) & ~(8 | 4) ); // FLAGS, set permanent+transient flag to 'K'
            }

            // submit enter key to force a refresh in rom
            //extern int keymap[5][5];
            //keymap[3][2] &= 0xFE;
            extern int nextkey;
            nextkey = 37+1; // send ZX_ENTER+1 keystroke;
        }
    }
}


int translate(char *ptr, int size, int locale) { 
    if( !(!!ptr * size) )
        return 0;

    // @todo: provide more translations in both ways en<-->ru<-->es<-->cz
    // @todo: move these definitions to a .ini file

    // if( locale != 'en' ) return;

    // es2en
    const char* tx[][2] = { // important: list must be sorted
        {"ABAJO","DOWN"},
        {"ABORTA","ABORT"},
        {"ABORTAR","ABORT"},
        {"ARRIBA","UP"},
        {"BOMBA","BOMB"},
        {"COGER","PICK"},
        {"COMENZAR","START"},
        {"CONTROLES","CONTROLS"},
        {"CURSORES","CURSOR"},
        {"DEFINIR","DEFINE"},
        {"DERECHA","RIGHT"},
        {"DISPARO","FIRE"},
        {"EMPEZAR","START"},
        {"FUEGO","FIRE"},
        {"INSTRUCCIONES","INSTRUCTIONS"},
        {"IZQUIERDA", "LEFT"},
        {"JUEGO","GAME"},
        {"JUGAR","PLAY"},
        {"PAUSA","PAUSE"},
        {"PULSA UNA TECLA", "PRESS ANY KEY"},
        {"REDEFINIR", "DEFINE"},
        {"REINICIA","RESET"},
        {"REINICIAR", "RESET"},
        {"SALTAR","JUMP"},
        {"SALTO","JUMP"},
        {"TECLADO","KEYPAD"}, // cant use 'keyboard', as it is longer than 'teclado'
        {"TECLAS","KEYS"},
        //{"USAR", "USE"},
        {"VIDAS","LIFES"},
    };
    int patches = 0;
    for( int i = (sizeof(tx) / sizeof(tx[0])); --i >= 0; ) { // important: patch in reverse order
        char *t1 = va("%s", tx[i][0]);
        char *t2 = va("%s%*.s", tx[i][1], strlen(tx[i][0]) - strlen(tx[i][1]), "");
        if( strlen(t1) == strlen(t2) ) {
            // patch regular text
            patches += patch( ptr, ptr + size, t1, t2, strlen(t2) );
            // same than above, but 0x80 ended now 
            // (this is a common ZX practise to signal end of string)
            t1[strlen(t1)-1] |= 0x80,
            t2[strlen(t2)-1] |= 0x80,
            patches += patch( ptr, ptr + size, t1, t2, strlen(t2) );
        } else {
            alert(va("Error: %s/%s translation length mismatch",t1,t2));
        }
    }
    printf("%d translation patch(es) found\n", patches);
    return patches;
}
