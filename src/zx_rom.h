#include "res/roms/48"
#include "res/roms/128"
#include "res/roms/plus2"
#include "res/roms/plus3"

#include "res/roms/plus2c"
#include "res/roms/lg18v07"
#include "res/roms/gw03v33"

#include "res/snaps/ld48bas"
#include "res/snaps/ld48bin"
//#include "res/snaps/ld128scr"
#include "res/snaps/ld128bas"
#include "res/snaps/ld128bin"
#include "res/snaps/ldplus3"
//#include "res/snaps/ldusr0bas"
//#include "res/snaps/ldusr0bin"

#define ROMHACK_TURBO 2.6 // x2 ok; x4,x6,x8 modes not working anymore :(
#define IF_ROMHACK_FASTER_EDGES(...)             __VA_ARGS__ // can be enabled
#define IF_ROMHACK_FASTER_PILOTS_AND_PAUSES(...) __VA_ARGS__ // can be enabled
#define IF_ROMHACK_HALF_BITS(...)              //__VA_ARGS__ // not working anymore :(
#define IF_ROMHACK_TURBO(...)                    __VA_ARGS__ // can be enabled

// romturbo stats
// parapshock: 232s (normal)
// parapshock:  73s (then, 50% processed bits, 6x pilots/pauses, x15 syncs) -> 30s -> 23s
// parapshock:  18s (then, F1 boost)
int patched_rom = 0;

void rom_patch(int on) {
    // [ref] https://speccy.xyz/rom/asm/0556.html

    if( ZX <=  48) memcpy(rom, rom48,     0x4000*1);
    if( ZX == 128) memcpy(rom, rom128,    0x4000*2);
    if( ZX == 200) memcpy(rom, romplus2,  0x4000*2);
    if( ZX >= 210) memcpy(rom, romplus3,  0x4000*4);

    if(ZX_ALTROMS)
    {
    // install plus2c on 128/+2 models
    if( ZX <= 200) memcpy(rom+0x0000, romplus2c, 0x4000), memcpy(rom+0x4000, rom48, 0x4000);
    if( ZX <= 200) rom[0x0566] = '6';  // 198(6) Sinclair
    if( ZX <= 200) rom[0x37F6] = 0x00; // black menu titles
    if( ZX <= 200) rom[0x3864] = 0x40; // black banners
    if( ZX <= 200) rom[0x387a] = 0x1b; // shift banner strips +1 right
    if( ZX <= 200) memcpy(rom+0x26F1, rom128+0x26F1, 8); // restore classic keyclick bug sound
    if( ZX <= 200) memcpy(rom+0x2744+11, rom128+0x2744+11, 2); // restore classic rom1 locked in 48 mode
    if( ZX <= 200) rom[0x1B2B+13] = 0x03; // fix error msg on plus2c+gw03/lg18+SPECTRUM command combo; (BORDER q#PI instead of 0 OK) ; $0013 -> $0003 Address of a $FF byte within ROM 1, used to generate error report "0 OK".

    // install gw03 on 16/48/128/+2 models.
    if( ZX <= 200) memcpy(rom+0x4000 * (ZX > 48), romgw03v33/*romlg18v07,rom48*/, 0x4000);
    }

#if TESTS
    if( ZX <= 200) memset(rom+0x4000 * (ZX > 48)+0x0C93, 0, 0xCD2-0xC93); // supress "Scroll?" message: JP #0CD2
#endif

    patched_rom = on && ZX <= 200;
if( patched_rom ) {

    byte *rombank = ROM_BANK(ZX >= 128 ? 1 : 0);

IF_ROMHACK_FASTER_PILOTS_AND_PAUSES(
    // ROMHACK $571 x6 faster pilot pulse
    memcpy(rombank+0x571, "\x21\x01\x00", 3); // LD HL,$0415 -> LD HL,$0105      The length of this waiting period will be almost one second in duration. -> /=4
    memcpy(rombank+0x580, "\x06\x4E",     2); // LD B,$9C                        The timing constant -> /=4
    memcpy(rombank+0x587, "\x3E\x63",     2); // LD A,$C6                        However the edges must have been found within about 3,000 T states of each other. -> /=4
);
IF_ROMHACK_FASTER_EDGES(
    // ROMHACK $5e7 x16 faster edges (sync) (358T->0T)
    memcpy(rombank+0x5E7, "\x3E\x01",     2); // LD A,$16 -> LD A,$1             Wait 358 T states before entering the sampling loop.
);
IF_ROMHACK_HALF_BITS(
    // ROMHACK $5ca option A: eliminate dupe bits (data) AND 16 faster edges (sync) (358T->0T)
    //memcpy(rombank+0x5CA, "\xCD\xED\x05", 3); // CALL LD_EDGE_2->SAMPLE ($5E3->$5ED)  Find the length of the 'off' and 'on' pulses of the next bit.

    // ROMHACK $5ca option B: eliminate dupe bits (data)
    memcpy(rombank+0x5CA, "\xCD\xE7\x05", 3); // CALL LD_EDGE_2->LD_EDGE1 ($5E3->$5E7)  Find the length of the 'off' and 'on' pulses of the next bit.

    //memcpy(rombank+0x5CA, "\x00\x00\x00\x00", 4);
    //memcpy(rombank+0x5E3, "\x00\x00\x00\x00", 4);
);

IF_ROMHACK_TURBO(
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
}
