#include "res/roms/48"
#include "res/roms/128"
#include "res/roms/plus2"
#include "res/roms/plus3"
#include "res/roms/pentagon128"
#include "res/roms/trdos503"
//#include "res/roms/trdos505"
//#include "res/roms/trdos611e"
//#include "res/roms/trdos604"
//#include "res/roms/gluk663pen"

//#include "res/roms/plus2c"    // https://speccy4ever.speccy.org/_CMS.htm
//#include "res/roms/plus2b"    // https://shorturl.at/dY4wP
#include "res/roms/lg18v07"     // https://speccy4ever.speccy.org/_CMS.htm
//#include "res/roms/gw03v33"   // https://speccy4ever.speccy.org/_CMS.htm
//#include "res/roms/jgh077"    // https://speccy4ever.speccy.org/_CMS.htm
//#include "res/roms/sebasic"   // https://web.archive.org/web/20061213013302if_/http://www.worldofspectrum.org:80/sinclairbasic/software/sebas094.zip

#include "res/snaps/ld16bas"
#include "res/snaps/ld16bin"
#include "res/snaps/ld48bas"
#include "res/snaps/ld48bin"
#include "res/snaps/ld128bas"
#include "res/snaps/ld128bin"
#include "res/snaps/ldplus2bas"
#include "res/snaps/ldplus2bin"
#include "res/snaps/ldplus2abas"
#include "res/snaps/ldplus2abin"
#include "res/snaps/ldplus3"
#include "res/snaps/ldplus3bas"
#include "res/snaps/ldplus3bin"
//#include "res/snaps/ldusr0bas"
//#include "res/snaps/ldusr0bin"

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

    /**/ if( ZX >= 210) memcpy(rom, romplus3,  0x4000*4);
    else if( ZX >= 200) memcpy(rom, romplus2,  0x4000*2);
    else if( ZX >= 128) memcpy(rom, rom128,    0x4000*2);
    else if( ZX >=  16) memcpy(rom, rom48,     0x4000*1);

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
#if 0
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
    // if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48), ZX==16 ? rom48 : romsebasic, 0x4000);
    // if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48)+0x3D00, rom48+0x3D00, (0x7F-0x20)*8); // restore charset
    // if( ZX == 200 || ZX ==128) memset(rom+0x4000*0+0x240, 0x00, 3); // make editor128 work with this rom

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

void rom_patch_turbo() {
    if(rom_patches & TURBO_PATCH) return;

    rom_patches |= TURBO_PATCH;

    byte *rombank = ROM_BANK(GET_BASIC_ROMBANK());

IF_TURBOROM_FASTER_PILOTS_AND_PAUSES(
    // ROMHACK $571 x6 faster pilot pulse
    memcpy(rombank+0x571, "\x21\x01\x00", 3); // LD HL,$0415 -> LD HL,$0105      The length of this waiting period will be almost one second in duration. -> /=4
    memcpy(rombank+0x580, "\x06\x4E",     2); // LD B,$9C                        The timing constant -> /=4
    memcpy(rombank+0x587, "\x3E\x63",     2); // LD A,$C6                        However the edges must have been found within about 3,000 T states of each other. -> /=4
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

    rombank[0x5A5+1] /= ROMHACK_TURBO; // LD B,$B0 -> $XX                 Set the timing constant for the flag byte
    rombank[0x5C6+1] /= ROMHACK_TURBO; // LD B,$B2 -> $XX                 Set the timing constant.
    rombank[0x5CE+1] /= ROMHACK_TURBO; // LD A,$CB -> $XX                 Compare the length against approx. 2,400 T states, resetting the carry flag for a '0' and setting it for a '1'.
    rombank[0x5D3+1] /= ROMHACK_TURBO; // LD B,$B0 -> $XX                 Set the timing constant for the next bit.
);
}

void rom_patch_klmode() {
    // apply hot patch
    if( ZX_KLMODE_PATCH_NEEDED && PC(cpu) == 0x15E1 ) { // @todo: find another less hacky PC addr
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
            extern int keymap[5][5];
            keymap[3][2] &= 0xFE;
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
            warning(va("Error: %s/%s translation length mismatch",t1,t2));
        }
    }
    printf("%d translation patch(es) found\n", patches);
    return patches;
}
