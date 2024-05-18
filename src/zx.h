// known issues:
// p-47 in-game click sound (ayumi? beeper?). double check against zx spectrum
// taipan + quickload
// gunstick + (kempston || kmouse) conflicts
// altroms:
// - donkey kong (AY 128, wrong ROM checksum)
// - blood brothers (AY 128, wrong ROM checksum)
// - MASK
// crash (vsync/int line?):
// - cauldron2(silverbird)
// - hostages(erbe) +2, +3 versions. 48 is ok.
// runahead:
// - +3 not compatible
// - breaks drums in TargetRenegade128(1988)+AYUMI
// - Tai-Pan(1986) crashes (bc of quickload)
// - https://near.sh/articles/input/run-ahead https://www.youtube.com/watch?v=_qys9sdzJKI // https://docs.libretro.com/guides/runahead/
// beeper:
// - indiana jones last crusade
// tape ticks:
// - 1942.tzx
// - nosferatu(alternativeltd)
// - darkstar, wizball.tap
// floating:
// - not working yet
// timing:
// - border: sentinel, defenders of earth, dark star, super wonder boy
// - border: aquaplane, olympic games, vectron, mask3
// - multicolor: mask3, shock megademo, MDA, action force 2, old tower, hardy
// - contended: exolon main tune requires contended memory, otherwise it's too fast +15% tempo
// - floating: arkanoid (original), cobra (original), sidewize, short circuit
// fdc:
// - afterburner(1988).dsk
// - z80 or fdc not being reset sometimes: load mercs.dsk, then wrestlingSuperstars.dsk
//   also dynasty wars, then e-motion. also ghouls n ghosts.dsk
// turborom:
// - x4,x6 not working anymore. half bits either. 
// - not compatible with lg18v07 rom (see: r-type128-km-nocheats.tap)
// - custardthekid + turborom

// note about TESTS mode:
// - scans src/tests/ folder
// - creates log per test
// - 48k
// - exits automatically
// - 50% frames not drawn
// - 50% drawn in fastest mode

#ifdef NDEBUG
#define DEV 0
#else
#define DEV 1
#endif

FILE *printer;

int do_audio = 1;

// ZX

#define ZX_TS_48K     69888
#define ZX_TS_128K    70908
#define ZX_FREQ_48K   3500000
#define ZX_FREQ_128K  3546894

int ZX_TS;
int ZX_FREQ;
int ZX_RF = !DEV;
int ZX_CRT = !DEV;
int ZX = 128; // 48, 128, 200 (+2), 210 (+2A), 300 (+3)
int ZX_FAST = 1;
int ZX_AY = 2; // mask: 0 (no ay), 1 (ayumi), 2 (flooh's), 3 (both)
int ZX_ALTROMS = 0; // 0:(no, original), 1:(yes, custom)
int ZX_TURBOROM = 0; // 0:(no) 1: patch rom so loading standard tape blocks is faster (see: .tap files)
int ZX_JOYSTICK = 3; // 0:(no), 1: sinclair1, 2: sinclair2, 3: cursor/fuller/kempston/protek
int ZX_AUTOPLAY = 1; // yes/no: auto plays/stops tapes based on #FE port reads
int ZX_RUNAHEAD = 0; // yes/no: improves input latency
int ZX_MOUSE = 1; // yes/no: kempston mouse
int ZX_ULAPLUS = 1; // yes/no: ulaplus 64color mode
int ZX_GUNSTICK = 0; // yes/no: gunstick/lightgun @fixme: conflicts with kempston & kmouse
int ZX_DEBUG = 0; // status bar, z80 disasm

void outport(word port, byte value);
byte inport(word port);

void port_0x1ffd(byte value);
void port_0x7ffd(byte value);
void port_0xbffd(byte value);
void port_0xfffd(byte value);
void reset(int model);

void regs(const char *banner);

// z80
z80_t cpu;
uint64_t pins;
int int_counter;

// mem
    byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
    byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
    #define RAM_BANK(n)   (mem + (n) * 0x4000)
    #define ROM_BANK(n)   (rom + (n) * 0x4000)
    #define DUMMY_BANK(n) (dum + (0) * 0x4000)
    #define VRAM RAM_BANK(5 + ((page128 & 8) >> 2)) // branchless ram bank 5 or 7. equivalent to RAM_BANK(page128 & 8 ? 7 : 5)

    int vram_contended;
    int vram_accesses;
    //byte contended[70908];
    unsigned short floating_bus[70908];

    byte *mem; // 48k
    byte *rom;
    byte *dum;

    // 16/48/128/+2: pages 1,3,5,7 are contended (1), 0,2,4,6 not contended (0) -> so mask is 0001 (1)
    // +2A/+3:       pages 4,5,6,7 are contended (1), 0,1,2,3 not contended (0) -> so mask is 0100 (4)
    //#define IS_CONTENDED_PAGE(addr) !!((addr >> 14) & (ZX >= 210 ? 4 : 1))

    #define ADDR(a)      ((byte *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define READ8(a)     (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(byte *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define READ16(a)    (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(word *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define WRITE8(a,v)  (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(byte *)&MEMw[(a)>>14][(a)&0x3FFF]=(v))
    #define WRITE16(a,v) (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(word *)&MEMw[(a)>>14][(a)&0x3FFF]=(v))

// ula
int  ZXFlashFlag;
rgba ZXPalette[64];
byte ZXBorderColor;

// 128
byte page128;

// plus3/2a
byte page2a;

// keyboard
int keymap[5][5];

// joysticks
byte kempston,fuller;

// mouse
byte kempston_mouse;

// beeper
#define TAPE_VOLUME 0.15f // relative to ~~buzz~~ ay
#define BUZZ_VOLUME 0.25f // relative to ay
beeper_t buzz;
byte last_fe;

// ay
ay38910_t ay;
struct ayumi ayumi;
byte ay_current_reg;
int/*byte*/ ay_registers[ 16 ];

// vsync
byte zx_vsync;

// ticks
uint64_t ticks, TS;

// boost
byte boost_on;

// tape
int tape_hz; // for vis
uint64_t tape_ticks;


#include "zx_rom.h"
#include "zx_dsk.h"
#include "zx_tap.h" // requires page128
#include "zx_tzx.h"
#include "zx_sna.h" // requires page128, ZXBorderColor

rgba ZXPaletteDef[64] = { // 16 regular, 64 ulaplus
#if 0 // check these against SHIFT-SPC during reset
    rgb(0x00,0x00,0x00),
    rgb(0x00,0x00,0xCD),
    rgb(0xCD,0x00,0x00),
    rgb(0xCD,0x00,0xCD),
    rgb(0x00,0xCD,0x00),
    rgb(0x00,0xCD,0xCD),
    rgb(0xCD,0xCD,0x00),
    rgb(0xD4,0xD4,0xD4),

    rgb(0x00,0x00,0x00),
    rgb(0x00,0x00,0xFF),
    rgb(0xFF,0x00,0x00),
    rgb(0xFF,0x00,0xFF),
    rgb(0x00,0xFF,0x00),
    rgb(0x00,0xFF,0xFF),
    rgb(0xFF,0xFF,0x00),
    rgb(0xFF,0xFF,0xFF),
#elif 0 // my fav, i guess
    rgb(0x00,0x00,0x00), // normal: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x00,0x00,0xC0), // note: D7 seems fine too
    rgb(0xC0,0x00,0x00),
    rgb(0xC0,0x00,0xC0),
    rgb(0x00,0xC0,0x00),
    rgb(0x00,0xC0,0xC0),
    rgb(0xC0,0xC0,0x00),
    rgb(0xC0,0xC0,0xC0),

    rgb(0x00,0x00,0x00), // bright: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x00,0x00,0xFF),
    rgb(0xFF,0x00,0x00),
    rgb(0xFF,0x00,0xFF),
    rgb(0x00,0xFF,0x00),
    rgb(0x00,0xFF,0xFF),
    rgb(0xFF,0xFF,0x00),
    rgb(0xFF,0xFF,0xFF),
#elif 0
    // Richard Atkinson's colors (zx16/48)
    rgb(0x06,0x08,0x00),
    rgb(0x0D,0x13,0xA7),
    rgb(0xBD,0x07,0x07),
    rgb(0xC3,0x12,0xAF),
    rgb(0x07,0xBA,0x0C),
    rgb(0x0D,0xC6,0xB4),
    rgb(0xBC,0xB9,0x14),
    rgb(0xC2,0xC4,0xBC),

    rgb(0x06,0x08,0x00),
    rgb(0x16,0x1C,0xB0),
    rgb(0xCE,0x18,0x18),
    rgb(0xDC,0x2C,0xC8),
    rgb(0x28,0xDC,0x2D),
    rgb(0x36,0xEF,0xDE),
    rgb(0xEE,0xEB,0x46),
    rgb(0xFD,0xFF,0xF7)
#elif 1 // vivid
    rgb(0x06,0x08,0x00), // normal: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x00,0x00,0xAB), // D8 and 96 looked fine
    rgb(0xAB,0x00,0x00),
    rgb(0xAB,0x00,0xAB),
    rgb(0x00,0xAB,0x00),
    rgb(0x00,0xAB,0xAB),
    rgb(0xAB,0xAB,0x00),
    rgb(0xAB,0xAB,0xAB),

    rgb(0x06,0x08,0x00), // bright: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x00,0x00,0xFF),
    rgb(0xFF,0x00,0x00),
    rgb(0xFF,0x00,0xFF),
    rgb(0x00,0xFF,0x00),
    rgb(0x00,0xFF,0xFF),
    rgb(0xFF,0xFF,0x00), // rgb(0xEE,0xEB,0x46), brighter yellow because jacknipper2 looks washed
    rgb(0xFF,0xFF,0xFF)
#elif 0 // latest
    rgb(0x00,0x00,0x00), // normal: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x00,0x00,0xC0), // note: D7 seems fine too
    rgb(0xC0,0x00,0x00),
    rgb(0xC0,0x00,0xC0),
    rgb(0x00,0xC0,0x00),
    rgb(0x00,0xC0,0xC0),
    rgb(0xC0,0xC0,0x00),
    rgb(0xC0,0xC0,0xC0),

    rgb(0x06,0x08,0x00), // bright: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x16,0x1C,0xB0), // Richard Atkinson's bright colors
    rgb(0xCE,0x18,0x18),
    rgb(0xDC,0x2C,0xC8),
    rgb(0x28,0xDC,0x2D),
    rgb(0x36,0xEF,0xDE),
    rgb(0xEE,0xEB,0x00), // rgb(0xEE,0xEB,0x46), brighter yellow because jacknipper2 looks washed
    rgb(0xFD,0xFF,0xF7)
#endif
};

enum SpecKeys {
    ZX_0,ZX_1,ZX_2,ZX_3,ZX_4,ZX_5,  ZX_6,ZX_7,ZX_8,ZX_9,ZX_A,ZX_B,  ZX_C,ZX_D,ZX_E,ZX_F,ZX_G,ZX_H,
    ZX_I,ZX_J,ZX_K,ZX_L,ZX_M,ZX_N,  ZX_O,ZX_P,ZX_Q,ZX_R,ZX_S,ZX_T,  ZX_U,ZX_V,ZX_W,ZX_X,ZX_Y,ZX_Z,
    ZX_SPACE,ZX_ENTER,ZX_SHIFT,ZX_SYMB,ZX_CTRL
};
#define ZXKey(a) ( keymap[ keytbl[a][0] ][ keytbl[a][1] ] &= keytbl[a][2] )
#define ZXKeyUpdate() \
keymap[1][1] = keymap[1][2] = keymap[2][2] = keymap[3][2] = keymap[4][2] = \
keymap[4][1] = keymap[3][1] = keymap[2][1] = 0xFF;

const unsigned char keytbl[256][3] = {
    {1, 2, 0xFE}, {1, 1, 0xFE}, {1, 1, 0xFD}, /* 0|1|2 */
    {1, 1, 0xFB}, {1, 1, 0xF7}, {1, 1, 0xEF}, /* 3|4|5 */
    {1, 2, 0xEF}, {1, 2, 0xF7}, {1, 2, 0xFB}, /* 6|7|8 */
    {1, 2, 0xFD}, {3, 1, 0xFE}, {4, 2, 0xEF}, /* 9|a|b */
    {4, 1, 0xF7}, {3, 1, 0xFB}, {2, 1, 0xFB}, /* c|d|e */
    {3, 1, 0xF7}, {3, 1, 0xEF}, {3, 2, 0xEF}, /* f|g|h */
    {2, 2, 0xFB}, {3, 2, 0xF7}, {3, 2, 0xFB}, /* i|j|k */
    {3, 2, 0xFD}, {4, 2, 0xFB}, {4, 2, 0xF7}, /* l|m|n */
    {2, 2, 0xFD}, {2, 2, 0xFE}, {2, 1, 0xFE}, /* o|p|q */
    {2, 1, 0xF7}, {3, 1, 0xFD}, {2, 1, 0xEF}, /* r|s|t */
    {2, 2, 0xF7}, {4, 1, 0xEF}, {2, 1, 0xFD}, /* u|v|w */
    {4, 1, 0xFB}, {2, 2, 0xEF}, {4, 1, 0xFD}, /* x|y|z */
    {4, 2, 0xFE}, {3, 2, 0xFE}, {4, 1, 0xFE}, /* SPC|ENT|SHF */
    {4, 2, 0xFD}, {1, 2, 0xEF},               /* SYMB|CTRL */
};

// 16/48
void port_0x00fe(byte value) {
    last_fe = value;

    // border color
    ZXBorderColor = (value & 0x07);

    // speaker
    //0x08 : tape when saving
    //0x10 : speaker
    //0x18 : both. works for 99% of games, some others wont (parapshock values: 0x18,0x08,0x18,...)
    //mic  : tape when loading

#if 0
    int is_saving = (page128&16) && (PC(cpu) >= 0x4ae && PC(cpu) <= 0x51a);
    int mask = is_saving ? 0x08 : 0x10;
    int spk = mic_on ? mic : 0;

    beeper_set(&buzz, !!(spk || (value & mask)));
#else
    // ref: https://piters.tripod.com/cassport.htm
    // ref: https://retrocomputing.stackexchange.com/a/27539
    // The threshold voltage for reading the input state (at the pin) of the ULA is 0.7V (*)
    // BIT4 EAR     BIT3 MIC     PIN 28 VOLTAGE  BIT6 IN READ
    //        0            0               0.34             0
    //        0            1               0.66             0 (borderline (*))
    //        1            0               3.56             1
    //        1            1               3.70             1
    //
    const float beeper_volumes[] = {
        TAPE_VOLUME, // rest volume
        TAPE_VOLUME, // tape volume (??%) see diagram above (*)
        0.96f, // beeper volume (96%)
        1.00f  // tape+beeper volume (100%)
    };

    int beeper = !!(value & 0x10);
    int tape = !!(mic | (!(value & 0x8)));
    int combined = beeper * 2 + tape;

    #if 1 // @fixme: adjust buzzer volume accordingly
    beeper_set_volume(&buzz, beeper_volumes[combined]);
    #endif

    beeper_set(&buzz, combined);
#endif
}

// zx128
void port_0x7ffd(byte value) {
    if(ZX >= 128)
    if(!(page128 & 32)) { //if bit5 not locked
        // check bit 2-0: RAM0/7 -> 0xc000-0xffff
        MEMw[3]=MEMr[3]=RAM_BANK(value&7);

        //16/48/128/+2: pages 1,3,5,7 are contended (1), 0,2,4,6 not contended (0) -> so mask is 0001 (1)
        //+2A/+3:       pages 4,5,6,7 are contended (1), 0,1,2,3 not contended (0) -> so mask is 0100 (4)
//MEMc[3]=value & contended_mask;

        //128:    check bit 4 : rom selection (ROM0/1 -> 0x0000-0x3FFF)
        //+2A/+3: check high bit of rom selection too (same as offset ROM selection to 0x8000) 1x0 101
        MEMr[0]=ROM_BANK((value & 16 ? 1 : 0) | ((page2a & 5) == 4 ? 2 : 0));

        page128=value;
    }
}

void ZXJoysticks(int up, int down, int left, int right, int fire) {
    #if 0
        //OPQAM/SP,OP1QM/SP,KLAZM/SP,ZXPL0/SP,QABNM/SP,QZIPM/SP,1ZIPM/SP,670OM/SP,QWOKC/SP
        static
        byte mapped_joystick = 0;
        byte mapped_joysticks[][6] = {
        { ZX_O,ZX_P,ZX_Q,ZX_A,ZX_M,ZX_SPACE },
        { ZX_O,ZX_P,ZX_1,ZX_Q,ZX_M,ZX_SPACE },
        { ZX_K,ZX_L,ZX_A,ZX_Z,ZX_M,ZX_SPACE },
        { ZX_Z,ZX_X,ZX_P,ZX_L,ZX_0,ZX_SPACE },
        { ZX_B,ZX_N,ZX_Q,ZX_A,ZX_M,ZX_SPACE },
        { ZX_I,ZX_P,ZX_Q,ZX_Z,ZX_M,ZX_SPACE },
        { ZX_I,ZX_P,ZX_1,ZX_Z,ZX_M,ZX_SPACE },
        { ZX_6,ZX_7,ZX_0,ZX_O,ZX_M,ZX_SPACE },
        { ZX_Q,ZX_W,ZX_O,ZX_K,ZX_C,ZX_SPACE },
        } ;
    #endif

    // kempston/i2l + fuller, then either sinclair1 or sinclair2 or cursor/protek/agf
    int joysticks[][5] = {{ZX_5,ZX_4,ZX_3,ZX_2,ZX_1},{ZX_0,ZX_9,ZX_8,ZX_7,ZX_6},{ZX_0,ZX_7,ZX_6,ZX_8,ZX_5},};
    kempston=0; fuller=0xff;
    if( ZX_JOYSTICK ) {
    if(fire)  { kempston|=16; fuller&=0xFF-128; ZXKey(joysticks[ZX_JOYSTICK-1][0]); }
    if(up)    { kempston|=8;  fuller&=0xFF-1;   ZXKey(joysticks[ZX_JOYSTICK-1][1]); }
    if(down)  { kempston|=4;  fuller&=0xFF-2;   ZXKey(joysticks[ZX_JOYSTICK-1][2]); }
    if(right) { kempston|=1;  fuller&=0xFF-8;   ZXKey(joysticks[ZX_JOYSTICK-1][3]); }
    if(left)  { kempston|=2;  fuller&=0xFF-4;   ZXKey(joysticks[ZX_JOYSTICK-1][4]); }
    if(ZX_JOYSTICK == 3 && (up | down | right | left)) ZXKey(ZX_SHIFT); // cursor requires shift
    }
}

// fdc
// fdc_ports.c -----------------------------------------------------------------

// control of port 0x1ffd (+2a/+3 -> mem/fdc/ptr)
void port_0x1ffd(byte value) {
    if(ZX < 210) return; //if not in +2a/+3 mode, return

    page2a = value; // value & 0x1f;    //save bits 0-4

    // check bit 1: special RAM banking or not

    if(value & 1)
    {
        switch(value & 7)
        {
            case 1 : //001=ram:0123
                MEMw[3]=MEMr[3]=RAM_BANK(3); // MEMc[3]=0;
                MEMw[2]=MEMr[2]=RAM_BANK(2); // MEMc[2]=0;
                MEMw[1]=MEMr[1]=RAM_BANK(1); // MEMc[1]=0;
                MEMw[0]=MEMr[0]=RAM_BANK(0); // MEMc[0]=0;
                break;

            case 3 : //011=ram:4567
                MEMw[3]=MEMr[3]=RAM_BANK(7); // MEMc[3]=1;
                MEMw[2]=MEMr[2]=RAM_BANK(6); // MEMc[2]=1;
                MEMw[1]=MEMr[1]=RAM_BANK(5); // MEMc[1]=1;
                MEMw[0]=MEMr[0]=RAM_BANK(4); // MEMc[0]=1;
                break;

            case 5 : //101=ram:4563
                MEMw[3]=MEMr[3]=RAM_BANK(3); // MEMc[3]=0;
                MEMw[2]=MEMr[2]=RAM_BANK(6); // MEMc[2]=1;
                MEMw[1]=MEMr[1]=RAM_BANK(5); // MEMc[1]=1;
                MEMw[0]=MEMr[0]=RAM_BANK(4); // MEMc[0]=1;
                break;

            case 7 : //111=ram:4763
                MEMw[3]=MEMr[3]=RAM_BANK(3); // MEMc[3]=0;
                MEMw[2]=MEMr[2]=RAM_BANK(6); // MEMc[2]=1;
                MEMw[1]=MEMr[1]=RAM_BANK(7); // MEMc[1]=1;
                MEMw[0]=MEMr[0]=RAM_BANK(4); // MEMc[0]=1;
                break;
        }
    } else {
        port_0x7ffd(page128);

        //restart locked_in_128_model values if touched by +2A or +3 emulation previously (see value & 1)
        MEMw[2]=MEMr[2]=RAM_BANK(2); //MEMc[2]=0;
        MEMw[1]=MEMr[1]=RAM_BANK(5); //MEMc[1]=1;
        MEMw[0]=DUMMY_BANK(0);       //MEMc[0]=0;
    }

    //bit 3: motor on/off
    if(ZX == 300) fdc_motor(value & 8);

    //bit 4: printer strobe
}


// control of port 0x2ffd (fdc in)
byte inport_0x2ffd(void) {
    return ZX == 300 ? fdc_read_status() : 0xFF;
}

// control of port 0x3ffd (fdc out & in)
byte inport_0x3ffd(void) {
    return ZX == 300 ? fdc_read_data() : 0xFF;
}

void port_0x3ffd(byte value) {
    if( ZX == 300 ) fdc_write_data(value);
}


void*    quicksave(unsigned slot);
void*    quickload(unsigned slot);


void config(int ZX) {
    if(ZX >= 16) {
        // memcpy(ROM_BANK(0),&rom48[0x4000*0],0x4000);
        // memcpy(&hwopt, &hwopt_48, sizeof(tipo_hwopt));

        //new layout to support flat 48k at UncompressZ80() (0567 instead of 0520)

        MEMr[3]=DUMMY_BANK(0); // MEMc[3]=0;
        MEMr[2]=DUMMY_BANK(0); // MEMc[2]=0;
        MEMr[1]=RAM_BANK(5); // MEMc[1]=1; //contended
        MEMr[0]=ROM_BANK(0); // MEMc[0]=0;

        MEMw[3]=RAM_BANK(0);
        MEMw[2]=RAM_BANK(2);
        MEMw[1]=RAM_BANK(5);
        MEMw[0]=DUMMY_BANK(0);

        page128=32; // -1
        page2a=128; // -1
        //contended_mask=1;
    }

    if(ZX >= 48) {
        MEMw[3]=MEMr[3]=RAM_BANK(0); // MEMc[3]=0;
        MEMw[2]=MEMr[2]=RAM_BANK(2); // MEMc[2]=0;
        MEMw[1]=MEMr[1]=RAM_BANK(5); // MEMc[1]=1; //contended
        MEMw[0]=MEMr[0]=ROM_BANK(0); // MEMc[0]=0;

        MEMw[0]=DUMMY_BANK(0);
    }

    if( ZX >= 128) {
        // memcpy(ROM_BANK(0),&rom128[0x4000*0],0x4000);
        // memcpy(ROM_BANK(1),&rom128[0x4000*1],0x4000);
        // memcpy(&hwopt, &hwopt_128, sizeof(tipo_hwopt));

        MEMw[3]=MEMr[3]=RAM_BANK(0); // MEMc[3]=0;
        MEMw[2]=MEMr[2]=RAM_BANK(2); // MEMc[2]=0;
        MEMw[1]=MEMr[1]=RAM_BANK(5); // MEMc[1]=1; //contended
        MEMr[0]=        ROM_BANK(0); // MEMc[0]=0;

        MEMw[0]=DUMMY_BANK(0);

        page128=0;
        //contended_mask=1;
    }

    if( ZX >= 200 ) { // +2
        // memcpy(ROM_BANK(0),&plus2[0x4000*0],0x4000);
        // memcpy(ROM_BANK(1),&plus2[0x4000*1],0x4000);
    }

    if( ZX >= 210 || ZX >= 300 ) { // +2A +3
        // memcpy(ROM_BANK(0),&plus3[0x4000*0],0x4000*4);

        page2a=0;
        //contended_mask=4;
    }

    // floating bus [16,48,128,+2]
    //
    // [ref] https://sinclair.wiki.zxnet.co.uk/wiki/Floating_bus
    // [ref] https://github.com/jsmolina/z88dk-tutorial-sp1/blob/master/floating-bus.md
    // [ref] https://softspectrum48.weebly.com/notes/category/floating-bus
    //
    // note: for +2A/+3 models, it works this way:
    // While the effect is no longer present on unused ports, it is still evident when reading from ports
    // which match a particular addressing pattern, expressed as (1+n*4), or in binary, 0000 xxxx xxxx xx01.
    // This is only evident when the memory paging ports are unlocked, otherwise they will always return FF.
    // In detail:
    // 1. It is only found on ports that follow the pattern (1 + (4 * n) && n < 0x1000) (that is, ports 1, 5, 9, 13 . . . 4093).
    // 2. The bus always returns 0xFF if bit 5 of port 32765 is set (i.e. paging is disabled), so it won’t work in 48K mode.
    // 3. Otherwise, the value returned is the value currently read by the ULA ORed with 1 (i.e. Bit 0 is always set).
    // 4. During idling intervals (i.e. when the ULA is drawing the border or in between fetching the bitmap/attribute byte), the bus latches onto the last value that was written to, or read from, contended memory, and not strictly 0xFF. This is crucial to keep in mind.

    // @fixme: for floatspy.tap to be stable
    // IM2 t_offs needs to be 25->22 (128K), 29->30 (48K)

    memset(floating_bus, 0, sizeof(floating_bus));
    if( ZX <= 200 ) {
        enum { TIMING_48  = 14340 }; // others: 14338 (faq), ramsoft: 14347(+9)
        enum { TIMING_128 = 14366 }; // others: 14364 (faq), ramsoft: 14368(+4)
        int timing = ZX < 128 ? TIMING_48 : TIMING_128, inc = ZX < 128 ? 96 : 100;
        int cycles = ZX < 128 ? 69888 : 70908;
        int adjust = ZX < 128 ? 0 : 0; // adjust

        for( int k = timing + adjust, y = 0; y < 192; ++y, k += inc ) {
            int SCANLINE = ((((((y)%64) & 0x38) >> 3 | (((y)%64) & 0x07) << 3) + ((y)/64) * 64) << 5);
            int pixel = 0x4000 + SCANLINE, attr = 0x5800 + 32*(y/8);

            for( int x = 0; x < 16; ++x ) { //16*8=128 T states per line (main screen)
                floating_bus[k++]=pixel++; // = 1;
                floating_bus[k++]=attr++;  // = 2;
                floating_bus[k++]=pixel++; // = 3;
                floating_bus[k++]=attr++;  // = 4;
                floating_bus[k++]=0x0000;  // = 5;
                floating_bus[k++]=0x0000;  // = 6;
                floating_bus[k++]=0x0000;  // = 7;
                floating_bus[k++]=0x0000;  // = 8;
            }
        }
    }
}

// mouse
struct mouse {
    int x, y, lb, mb, rb;
};
struct mouse mouse() {
    extern Tigr *app;
    int mx, my, mb, lmb, mmb, rmb; // buttons: R2M1L0 bits
    tigrMouse(app, &mx, &my, &mb); lmb = mb & 1; mmb = !!(mb & 2); rmb = !!(mb & 4);
    return ( (struct mouse) {mx, my, lmb, mmb, rmb} );
}
void mouse_clip(int clip) {
    static RECT restore; do_once GetClipCursor(&restore);

    extern Tigr *app;
    HWND hWnd = (HWND)app->handle;

    extern int active; // ui

    // get client area (0,0,w,h)
    RECT dims;
    GetClientRect(hWnd, &dims);

    // convert area to desktop coordinates
    RECT win = dims;
    ClientToScreen(hWnd, (POINT*)&win.left); // convert top-left
    ClientToScreen(hWnd, (POINT*)&win.right); // convert bottom-right

    // wrap mouse over the edges
    if( clip && !active ) {
        int w = dims.right, h = dims.bottom;
        int x = win.left, y = win.top;

        // mouse coords relative to top-left (0,0) client area
        struct mouse m = mouse();
        // convert to desktop coords
        int mx = win.left + m.x;
        int my = win.top + m.y;

        // difference between Tigr's and Windows' mouse desktop coords
        POINT p;
        GetCursorPos(&p);
        int diffx = mx - p.x, diffy = my - p.y;

        if(mx<x) while(mx<x) mx += w;
        else while(mx>(x+w/2)) mx -= w;

        if(my<y) while(my<y) my += h;
        else while(my>(y+h/2)) my -= h;

        SetCursorPos(mx - diffx,my - diffy);
    }

    // Clip or restore the cursor to its previous area.
    // ClipCursor(clip && !active ? &win : &restore);

    // Hide or show cursor
    ShowCursor(clip && !active ? FALSE : TRUE);
}




// ay
void port_0xfffd(byte value) {
    // select ay register
    ay_current_reg=(value&15);
    // floooh's
    ay38910_iorq(&ay, AY38910_BDIR|AY38910_BC1|ay_current_reg<<16);
}
void port_0xbffd(byte value) {
    // update ay register
    ay_registers[ay_current_reg]=value;
    // floooh's
    ay38910_iorq(&ay, AY38910_BDIR|value<<16);
    // ayumi
    int *r = ay_registers;
    switch (ay_current_reg)
    {
    case 0:
    case 1:
        ayumi_set_tone(&ayumi, 0, (r[1] << 8) | r[0]);
        break;
    case 2:
    case 3:
        ayumi_set_tone(&ayumi, 1, (r[3] << 8) | r[2]);
        break;
    case 4:
    case 5:
        ayumi_set_tone(&ayumi, 2, (r[5] << 8) | r[4]);
        break;
    case 6:
        ayumi_set_noise(&ayumi, r[6]);
        break;
    case 8:
        ayumi_set_mixer(&ayumi, 0, r[7] & 1, (r[7] >> 3) & 1, r[8] >> 4);
        ayumi_set_volume(&ayumi, 0, r[8] & 0xf);
        break;
    case 9:
        ayumi_set_mixer(&ayumi, 1, (r[7] >> 1) & 1, (r[7] >> 4) & 1, r[9] >> 4);
        ayumi_set_volume(&ayumi, 1, r[9] & 0xf);
        break;
    case 10:
        ayumi_set_mixer(&ayumi, 2, (r[7] >> 2) & 1, (r[7] >> 5) & 1, r[10] >> 4);
        ayumi_set_volume(&ayumi, 2, r[10] & 0xf);
        break;
    case 7:
        ayumi_set_mixer(&ayumi, 0, r[7] & 1, (r[7] >> 3) & 1, r[8] >> 4);
        ayumi_set_mixer(&ayumi, 1, (r[7] >> 1) & 1, (r[7] >> 4) & 1, r[9] >> 4);
        ayumi_set_mixer(&ayumi, 2, (r[7] >> 2) & 1, (r[7] >> 5) & 1, r[10] >> 4);
        break;
    case 11:
    case 12:
        ayumi_set_envelope(&ayumi, (r[12] << 8) | r[11]);
        break;
    case 13:
        if (r[13] != 255) //< needed?
        ayumi_set_envelope_shape(&ayumi, r[13]);
        break;
    }
}
byte inport_0xfffd(void) {
//  return ay38910_iorq(&ay, 0);
    unsigned char ay_registers_mask[ 16 ] = {
        0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x1f, 0xff,
        0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x0f, 0xff, 0xff,
    };

    if( ZX_GUNSTICK ) // magnum lightgun
    if( ay_current_reg == 14 ) {

        // Magnum Light Phaser / Defender Light Gun for Spectrum >= 128
        // 1101 1111 bit5: trigger button (0=Pressed, 1=Released)
        // 1110 1111 bit4: light sensor   (0=None, 1=Light)

        byte gunstick(byte);
        struct mouse m = mouse();
        byte v = m.lb ? 0xDF : 0xEF; // set button
        v |= 0x10 * (gunstick(0xFF) < 0xFE); // set light

        ay_registers[14] = v;

        //printf("\t\t\tmagnum (%02x vs %02x)\n", gunstick(0xFF), v);
        return v;
    }

    /* The AY I/O ports return input directly from the port when in
    input mode; but in output mode, they return an AND between the
    register value and the port input. So, allow for this when
    reading R14... */

    if( ay_current_reg == 14 )
    return (ay_registers[7] & 0x40 ? 0xbf & ay_registers[14] : 0xbf);

    /* R15 is simpler to do, as the 8912 lacks the second I/O port, and
    the input-mode input is always 0xff */

    if( ay_current_reg == 15 && !( ay_registers[7] & 0x80 ) ) return 0xff;

    /* Otherwise return register value, appropriately masked */
    return ay_registers[ay_current_reg] & ay_registers_mask[ay_current_reg];
}

uint64_t tick1(int num_ticks, uint64_t pins, void* user_data) {

#if DEV
    if( cpu.step == 0 ) {
        unsigned pc = Z80_GET_ADDR(pins);

#if PRINTER
        // traps rom print routine. useful for automation tests

        // check no int pending, +2a/+3, or rom1
        // if( !IFF1(cpu) && ZX <= 200 && !(page128 & 16) )

        if (pc == 0x09F4 /*&& SP(cpu) < 0x4000*/) {
            uint8_t channel_k = READ8( IY(cpu) + 0x30 ) & 16; // FLAGS2
            uint8_t is_lower_screen = READ8( IY(cpu) + 2 ) & 1; // TV_FLAG
            if (!channel_k /*&& !is_lower_screen*/ ) {
                char ch = cpu.a; // & 0x7f;
                // if(ch >= 0x7F ) printf(printer, "%c", "© ▝▘▀▗▐▚▜▖▞▌▛▄▟▙█"[ch - 0x7f]);
                // else
                if(ch == 0x0D || ch >= 32) {
                    fprintf(printer, "%c", ch < 32 ? '\n' : ch);
                    fflush(printer);
                }
            }
        }
#endif

        // disasm
        void dis(unsigned, unsigned, FILE*);

        int do_disasm = pc >= 0x38 && pc < 0x45; // mouse().rb;
        if( 0 )
        if( do_disasm ) dis(pc, 1, stdout);
    }
#endif

    // mem
    if( pins & Z80_MREQ ) {
        if( pins & Z80_RD ) {
            uint16_t addr = Z80_GET_ADDR(pins);
            uint8_t value = READ8(addr);
            Z80_SET_DATA(pins, value);
        }
        else if( pins & Z80_WR ) {
            uint16_t addr = Z80_GET_ADDR(pins);
            uint8_t value = Z80_GET_DATA(pins);
            WRITE8(addr, value);
        }
    }
    // ports
    else if( pins & Z80_IORQ ) {

#if NEWCORE
        if (pins & Z80_M1) {
            // put an interrupt vector on the data bus
            uint16_t intvec = (I(cpu)<<8) | 0xFF;
            Z80_SET_DATA(pins, intvec);
        }
        else
#endif

        if( pins & Z80_RD ) {
            uint16_t addr = Z80_GET_ADDR(pins);
            uint8_t value = inport(addr);
            Z80_SET_DATA(pins, value);
        }
        else if( pins & Z80_WR ) {
            uint16_t addr = Z80_GET_ADDR(pins);
            uint8_t value = Z80_GET_DATA(pins);
            outport(addr, value);
        }
    }

#if NEWCORE
    sys_audio();
    return pins;
#endif

    for(int i = 0; i < num_ticks; ++i)
    sys_audio();

    // vsync
    return pins | (zx_vsync ? zx_vsync = 0, Z80_INT : 0);
}

void init() {
    dum = (char*)realloc(dum, 16384);    // dummy page
    rom = (char*)realloc(rom, 16384*4);  // +3
    mem = (char*)realloc(mem, 16384*16); // scorpion

#if NEWCORE
    pins = z80_init(&cpu);
#else
    z80_desc_t desc = { tick1, NULL };
    z80_init(&cpu, &desc);
#endif

    beeper_desc_t bdesc = {0};
    bdesc.tick_hz = ZX_FREQ; // ZX_FREQ_48K
    bdesc.sound_hz = AUDIO_FREQUENCY;
    bdesc.base_volume = BUZZ_VOLUME;
    beeper_init(&buzz, &bdesc);

    // ay
    memset(ay_registers,0,16);
    ay_current_reg=0;

    // floooh's
    ay38910_desc_t ay_desc = {0};
    ay_desc.type = AY38910_TYPE_8912;
    ay_desc.tick_hz = ZX_FREQ / 2;
    ay_desc.sound_hz = AUDIO_FREQUENCY; // * 0.75; // fix: -25% speed
    ay_desc.magnitude = 1.0f;
    ay38910_init(&ay, &ay_desc);

    // ayumi
    const int is_ym = 1; // should be 0, but 1 sounds more Speccy to me somehow (?)
    const int eqp_stereo_on = 0;
    const double pan_modes[7][3] = { // pan modes, 7 stereo types
      {0.50, 0.50, 0.50}, // MONO, original
      {0.10, 0.50, 0.90}, // ABC, common in west-Europe
      {0.10, 0.90, 0.50}, // ACB, common in east-Europe
      {0.50, 0.10, 0.90}, // BAC
      {0.90, 0.10, 0.50}, // BCA
      {0.50, 0.90, 0.10}, // CAB
      {0.90, 0.50, 0.10}, // CBA
    };

    if (!ayumi_configure(&ayumi, is_ym, 2000000 * (2000000.0 / (ZX_FREQ / 2.0)), AUDIO_FREQUENCY)) { // ayumi is AtariST based, i guess. use 2mhz clock instead
        die("ayumi_configure error (wrong sample rate?)");
    }
    const double *pan = pan_modes[0]; // MONO
    ayumi_set_pan(&ayumi, 0, pan[0], eqp_stereo_on);
    ayumi_set_pan(&ayumi, 1, pan[1], eqp_stereo_on);
    ayumi_set_pan(&ayumi, 2, pan[2], eqp_stereo_on);
}

// reset most systems. preserve mic
void reset(int model) {
    ZX = model; // (model == 16 || model == 48 ? 48 : 128);

    ZX_TS = ZX < 128 ? ZX_TS_48K : ZX_TS_128K;
    ZX_FREQ = ZX < 128 ? ZX_FREQ_48K : ZX_FREQ_128K;
    init(); // static int ever = 0; if(!ever) { ever = 1; init(); }

    page2a = ZX < 210 ? 128 : 0;
    page128 = ZX < 128 ? 32 : 0;
    last_fe = 0;

//    memset(mem, 0x00, 128*1024);
//    memcpy(rom, ZX < 128 ? rom48 : rom128, ZX < 128 ? 16384 : 2*16384);
    rom_patch(0);

    beeper_reset(&buzz);

    ay38910_reset(&ay);
    ayumi_reset(&ayumi);

    void ula_reset();
    ula_reset();

    mouse_clip(0);

    kempston_mouse = 0;

config(ZX);

if(DEV) {
    // keep developer's azimuth between sessions
} else {
    // reset azimuth per session for end-users
    azimuth = AZIMUTH_DEFAULT;
}

#if NEWCORE
    pins = z80_reset(&cpu);
#else
    z80_reset(&cpu);
#endif

    ticks=0;

    tape_hz = 0;
    tape_ticks = 0;
}

// reset & clear all systems
void clear(int model) {
    reset(model);

    mic_reset();
}

void sim(unsigned TS) {
    // if(TS<0)return;

#if NEWCORE

    for( int tick = 0; tick < TS; ++tick) {

        if (zx_vsync /*  && cpu.step == 2 && IFF1(cpu) */ ) { // adjust
            zx_vsync = 0;

            // request vblank interrupt
            z80_interrupt(&cpu, 1);

            // hold the INT pin for 32 ticks
            int_counter = 32; // - (4 - (tick & 3)); // 23?
        }
        
        // clear INT pin after 32 ticks
        if (int_counter > 0) {
            if(!--int_counter) z80_interrupt(&cpu, 0);
        }

        pins = z80_tick(&cpu, pins);
        ++ticks;
        tape_ticks += !!mic_on;

        pins = tick1(1,pins,0);
    }
#else
    z80_exec(&cpu, TS);
    ticks += TS;
    tape_ticks += !!mic_on * TS;
#endif
}

void sys_audio() {
    // tick the beeper (full frequency)
    bool sample_ready = beeper_tick(&buzz);

    // tick the AY (half frequency)
    static float ay_sample1 = 0, ay_sample2 = 0;
    static byte even = ~0u; ++even;
    if( ZX_AY & 1 ) if(!(even & 0x7F)) ay_sample1 = ayumi_render(&ayumi, ay_registers, 1) * 2; // 2/256 freq. even == 0 || even == 0x80
    if( ZX_AY & 2 ) if( even & 1 ) ay38910_tick(&ay), ay_sample2 = ay.sample; // half frequency

    if( ZX_AY == 0 ) ay_sample1 = ay_sample2 = 0; // no ay
    if( ZX_AY == 1 ) ay_sample2 = ay_sample1;     // ayumi only
    if( ZX_AY == 2 ) ay_sample1 = ay_sample2;     // floooh's only
    float ay_sample = (ay_sample1 + ay_sample2) * 0.5f; // both

    if( do_audio && sample_ready ) {
        float master = 0.98f;
        float sample = (buzz.sample * 0.75f + ay_sample * 0.25f) * master;

        audio_queue(sample);
    }
}

byte ulaplus_mode = 0; // 0:pal,1:mode,else:undefined
byte ulaplus_data = 0;
byte ulaplus_enabled = 0;
byte ulaplus_grayscale = 0;
byte ulaplus_registers[64+1] = {0};

void ula_reset() {
    ulaplus_mode = 0;
    ulaplus_data = 0;
    ulaplus_enabled = 0;
    ulaplus_grayscale = 0;
    memset(ulaplus_registers, 0, sizeof(ulaplus_registers));
    memcpy(ZXPalette, ZXPaletteDef, sizeof(ZXPalette[0]) * 64);

#if 0
    // contended lookups
    memset(contended, 0, sizeof(contended));
    for( int i = 63, c = 6; i <= (63+192); ++i, --c ) {
        contended[i*228]=c<0?0:c; // initial: 64*224 (48K)
        if(c<0) c=6;
    }
#endif

    // colorize. this is especially needed on Richard Atkinson's palette (imho)
    // also, for the RF effect, colors are saturated right here instead of during bitmap blits
    for( int i = 0; i < 16; ++i) {
        unsigned r,g,b; byte h,s,v;
        rgb_split(ZXPalette[i],r,g,b);
        rgb2hsv(r,g,b,&h,&s,&v);

                    //unsigned luma = (unsigned)((r * 0.299 + g * 0.587 + 0.114));
                    //luma *= 1.25; if( luma > 255 ) luma = 255;
                    //ZXPalette[i] = rgb(luma,luma,luma);

        // saturated; (h)ue bw-to-(s)aturation (b)rightness
                 // s = s*1.125 < 255 ? s*1.125 : 255; // extra saturated
                    v = v*1.125 < 255 ? v*1.125 : 255; // extra brightness
                    ZXPalette[i] = as_rgb(h,s,v);
                    continue;

        // bw
                    s = 0;
                    v = v*0.98;
                    ZXPalette[i] = as_rgb(h,s,v);
    }
}


void outport(word port, byte value) {
    //printf("out port[%04x]=$%02x\n", port, value);

#if 0
    if(contended_mask & 4) //if +2a or +3 then apply (fixes fairlight games)
#else
    // +2a/+3
    if( ZX >= 210 )
#endif
    {
        //if (port & (0xFFFF^0x1FFD)) { port_0x1ffd(value); return; }
        //if (port & (0xFFFF^0x3FFD)) { port_0x3ffd(value); return; }

      //if((port & 0xf002) == 0x0000) { port_0x0ffd(value); return; } // 0000xxxx xxxxxx0x   parallel port
        if((port & 0xf002) == 0x1000) { port_0x1ffd(value); return; } // 0001xxxx xxxxxx0x   mem pagination
      //if((port & 0xf002) == 0x2000) { port_0x2ffd(value); return; } // 0010xxxx xxxxxx0x   fdc status
        if((port & 0xf002) == 0x3000) { port_0x3ffd(value); return; } // 0011xxxx xxxxxx0x   fdc data
        if((port & 0xc002) == 0xa000) { port_0x7ffd(value); return; } // 01xxxxxx xxxxxx0x   mem pagination
        if((port & 0xc002) == 0xb000) { port_0xbffd(value); return; } // 10xxxxxx xxxxxx0x   ay data
        if((port & 0xc002) == 0xc000) { port_0xfffd(value); return; } // 11xxxxxx xxxxxx0x   ay data register
    }

    // sorted ports
    if (ZX >= 128) if(!(port & (0xFFFF^0x7FFD))) { port_0x7ffd(value); return; }


    // ula+ register port
    if( ZX_ULAPLUS )
    if (!(port & (0xFFFF^0xBF3B))) {
        ulaplus_data = value & 0x3f;
        ulaplus_mode = value >> 6;
    }

    // ay
    if (!(port & (0xFFFF^0xBFFD)))     { port_0xbffd(value); return; } // ay

    // ula+ data port
    if( ZX_ULAPLUS )
    if (!(port & (0xFFFF^0xFF3B))) {
        if( ulaplus_mode == 0 ) {
            ulaplus_registers[ulaplus_data] = value;
            unsigned b = (value & 0x03) << 6; b |= !!b << 5;
            unsigned r = (value & 0x1c) << 3;
            unsigned g = (value & 0xe0);
            ZXPalette[ulaplus_data] = ulaplus_grayscale ? rgb(value,value,value) : rgb(r,g,b);
        }
        if( ulaplus_mode != 0 ) {
            ulaplus_registers[64] = value;
            ulaplus_enabled = value & 1;
            ulaplus_grayscale = !!(value & 2);
        }
    }

    // ay
    if (!(port & (0xFFFF^0xFFFD)))     { port_0xfffd(value); return; } // ay

    // fuller audio box emulation
    if (!(port & (0xFF^0x3F)))         { port_0xfffd(value); return; } // ay
    if (!(port & (0xFF^0x5F)))         { port_0xbffd(value); return; } // ay

    // default
    if( !(port & (0xFF^0xFE))) {
        port_0x00fe(value);
        return;
    }
}
byte gunstick(byte code) { // out: FF(no), FE(fire), FB(lit)
    // @fixme, lmb should last at least 2 frames (see Guillermo Tell)
    enum { h_border = _32 };
    enum { v_border = _24 };

    struct mouse m = mouse();

    // fire
    if (m.lb || m.rb) {
        code &= 0xFE;
        goto trig;
    }

        // estimate X,Y position based on TS and check against real mouse pos
        int scanline = (ticks % 70908) / 311;
        if( scanline <= 64 || scanline > (64+192) ) return 0xFF;
        int raster = (ticks % 70908) % 228;
        if( raster > 128 ) return 0xFF;
#if 0
        m.x -= h_border; // 0..256
        m.y -= v_border; // 0..191
        raster *= 2; // 0..256
        scanline -= 64; // 0..191
#endif
        if( ((m.x-raster)*(m.x-raster)) < 64 )
            if( ((m.y-scanline)*(m.y-scanline)) < 64 )
                code &= 0xFB;

        return code;

        // debug
        printf("gun(%3d,%3d) ask(%3d,%3d) %02x\n", m.x,m.y, raster,scanline, code);

        // debug
        if( 1 )
        {
            raster *= 2; // 0..256
            raster += _32; // 32..288

            scanline -= 64; // 0..191
            scanline += _24; // 24..215

            int x = 0; // raster > _319 ? _319 : raster;
            int y = scanline > _239 ?  _239 : scanline;

            extern window *app;
            int width = _32+256+_32;
            rgba *texture = &((rgba*)app->pix)[0];
            texture[ x + y * width ] = rgb(255,0,255);
        }
        return code;

trig:;

    // light
    // check whether attribute is white
    if (m.x > h_border && (m.x - h_border < 256))
    if (m.y > v_border && (m.y - v_border < 192)) {
        int x = (m.x - h_border) / 8;
        int y = (m.y - v_border) / 8;

        int cell = x;
        int line = m.y - v_border;
        int offset = ((line & 7) << 8) + ((line & 0x38) << 2) + ((line & 0xC0) << 5) + cell;
        byte *pixel = VRAM + offset;
        byte *attr = VRAM + 6144 + 32 * y + x;
        int ink = *attr & 7;
        int paper = (*attr >> 3) & 7;
        int bright = *attr & 0x40;

        #if 0 // find bright luma pixels (green 4, cyan 5, yellow 6, white 7)
        if( *pixel ? ink >= 4 : paper >= 4 ) code &= 0xFB;
        #else // find white attribs
        if (ink == 0x07 || paper == 0x07) code &= 0xFB;
        #endif

        #if 0 // debug
        printf("gun(%d,%d,%d) attr(%d,%d,%02x) val:%02x\n", m.x,m.y,m.lb, x,y,(byte)*attr, code);
        *pixel = 0xFF, *attr = 0x38;
        #endif
    }
    return code;
}
byte inport(word port) {
    // unsigned PC = z80_pc(&cpu); if(PC >= 0x3D00 && PC <= 0x3DFF) printf(" in port[%04x]\n", port); // betadisk

    // if(port != 0x1ffd && port != 0x2ffd && port != 0x3ffd && port != 0x7ffe) { regs("inport"); printf(" in port[%04x]\n", port); }
    //[0102] alien syndrome.dsk ??
    //[0002] alien syndrome.dsk ??
    //[fefe] alien syndrome.dsk

    // +2a/+3
    if( ZX >= 210 )
    {
        if((port & 0xf002) == 0x2000) { return inport_0x2ffd(); } // 0010xxxx xxxxxx0x   fdc status
    }

    if( ZX_MOUSE )
    {
        // kempston mouse detection
        if(port == 0xFADF) kempston_mouse |= 1;
        if(port == 0xFBDF) kempston_mouse |= 2;
        if(port == 0xFFDF) kempston_mouse |= 4;

        // kempston mouse
        if( kempston_mouse == 7 ) {
            struct mouse m = mouse();
            if(!(port & (0xFFFF^0xFADF))) return mouse_clip(1), 0xFF&~(1*m.rb+2*m.lb+4*m.mb); // ----.--10.--0-.----  =  Buttons: D0 = RMB, D1 = LMB, D2 = MMB
            if(!(port & (0xFFFF^0xFBDF))) return mouse_clip(1),  m.x;                         // ----.-011.--0-.----  =  X-axis:  $00 … $FF
            if(!(port & (0xFFFF^0xFFDF))) return mouse_clip(1), -m.y;                         // ----.-111.--0-.----  =  Y-axis:  $00 … $FF
        }
    }

    // kempston port (31) @fixme: also seen mappings on port 55 and 95 (?)
    if( kempston_mouse != 7 ) {
        // gunstick (bestial warrior)
        if(ZX_GUNSTICK)
        if(!(port & (0xFF^0xDF))) return /*puts("gunstick kempston"),*/ gunstick(0xFF);

        // kempston joystick
        if(!(port & (0xFF^0xDF))) return /*mic_on = 0,*/ kempston; //bit 5 low = reading kempston (as told pera putnik)
        //if(!(port & 0xE0)) return kempston;          //bit 7,6,5 low = reading kempston (as floooh)
    }

    // +2a/+3
    if( ZX >= 210 ) {
    if(!(port & (0xFFFF^0x2FFD))) return inport_0x2ffd();
    if(!(port & (0xFFFF^0x3FFD))) return inport_0x3ffd();
    }

    // ulaplus read data
    if( ZX_ULAPLUS )
    if (!(port & (0xFFFF^0xFF3B))) {
        if( ulaplus_mode == 0 ) {
            return ulaplus_registers[ulaplus_data];
        }
        if( ulaplus_mode != 0 ) {
            return ulaplus_registers[64];
        }
    }

    // gunstick 0x7ffe,0xbffe,0xdffe,0xeffe,0xf7fe,0xfefe,0xfbfe,0xfdfe
    // @fixme: add SINCLAIR2 port too
    if( ZX_GUNSTICK )
    if (!(port & (0xFFFF^0xEFFE))) {
        int code = 0xFF;

        // prevent emulator input being passed to zx
        extern int active;
        if(!active) {
        if (!(port & 0x0100)) code &= keymap[4][1];
        if (!(port & 0x0200)) code &= keymap[3][1];
        if (!(port & 0x0400)) code &= keymap[2][1];
        if (!(port & 0x0800)) code &= keymap[1][1];
        if (!(port & 0x1000)) code &= keymap[1][2];
        if (!(port & 0x2000)) code &= keymap[2][2];
        if (!(port & 0x4000)) code &= keymap[3][2];
        if (!(port & 0x8000)) code &= keymap[4][2];
        }

        return /*puts("gunstick sinclair"),*/ gunstick(code);
    }

    // ay
    if(!(port & (0xFFFF^0xFFFD))) return inport_0xfffd();

    // fuller joystick
    if(!(port & (0xFF^0x7F))) return /*mic_on = 0,*/ fuller;

    // keyboard
    if(!(port & (0xFF^0xFE))) {
        byte code = 0xFF;

        // prevent emulator input being passed to zx
        extern int active;
        if(!active) {

        if (!(port & 0x0100)) code &= keymap[4][1];
        if (!(port & 0x0200)) code &= keymap[3][1];
        if (!(port & 0x0400)) code &= keymap[2][1];
        if (!(port & 0x0800)) code &= keymap[1][1];
        if (!(port & 0x1000)) code &= keymap[1][2];
        if (!(port & 0x2000)) code &= keymap[2][2];
        if (!(port & 0x4000)) code &= keymap[3][2];
        if (!(port & 0x8000)) code &= keymap[4][2];

        }

        // abu simbel profanation (FIXME)
        // issue2 keyboard emulation (16K/48K only). bits 765 set. 560,000 48k sold models.
        // issue3 keyboard emulation (>=128 models). bits 7 5 set, 6 reset. 3,000,000 48k sold models.
        // On an Issue 2, both bits 3 and 4 must be reset for the same effect to occur.
        // On an Issue 3, an OUT 254 with bit 4 reset will give a reset bit 6 from IN 254.
#if 1
        int issue2 = ZX < 128; // false
        int issue_mask = issue2 ? 0x18 : 0x10;
        code |= 0xE0;
        if( !(last_fe & issue_mask) ) code &= ~0x40;
#else
        code |= 0xE0; // issue2
        //    code &= 0xBF; // issue3
#endif

        if( 1 ) {
            byte ear = ReadMIC(tape_ticks);

#if 1 // output sound if tape is being read
            if(mic_on)
            beeper_set(&buzz, !!((last_fe & 0x10) | !!ear));
#endif

            code = code ^ ear;
        }


        if( ZX_AUTOPLAY )
        {
            // auto-tape tests [* issue]
            // [x] rom: pilot loader, acid killer.tzx
            // [x] full suite: gauntlet, *p-47 thunderbolt, munsters 48k, myth,
            // [x] glued: batman caped crusader
            // [x] ay stopper: madmix2 (symb), mortadelo y filemon 2 (spc), wizard warz,
            // [x] press-any-key: the egg, cauldron2.tap
            // [x] loading pauses: diver, doctum, coliseum.tap, barbarian(melbourne), indiana jones last crusade
            // [ ] *viaje al centro de la tierra, tuareg, desperado
            // [ ] *cauldron (silverbird),
            // [ ] *turrican, turrican2, un squadron,
            // [x] tusker, predator,
            // [x] last ninja 2, karnov,
            // [x] gryzor 48k, rainbow islands 48k
            // [?] twilight, wild streets, 
            // [?] time scanner, tmnt coin op, super wonder boy, strider 2, street hassle,
            // [?] st dragon, spherical, real ghostbusters, spherical, perico delgado,
            // [?] pang, outrun, laser squad, koronis rift,
            // [?] gauntlet 2, line of fire, mandragore, the deep, 
            // [?] galaxy force, flash gordon, dragons lair, dragons lair 2, dragon breed,
            // [?] double dragons, desolator, final assault, forgotten worlds, crack down,
            // [?] alien syndrome, altered beast,

            // stable tape heuristics I used for a long time:
            // - hint play if loading from rom && mic has tape
            // newer heuristics that worked ok-ish:
            // - hint stop if strchr("uo",voc[voc_pos/4] & 0x7f) ; pa(u)se st(o)p
            // - hints based on IN FE port activity
            //   - 70908 ts/s / 8 ts/portread = 8863 portreads/s. theory limit of 8863 Hz.
            //     very high frequency polling are press-any-key pollings (>1700 Hz)
            //     high frequency polling are data or pilot loaders (~500..1200 Hz)
            //     low frequency polling are keyboard handlers (0..8 Hz)
            //   - hint stop if keyboard is being read (<300hz); else hint loader. myth will work; madmix2 (shift) wont.
            //   - hint stop if press-any-key tight loops (>1700hz); else hint loader. madmix2 will work; myth wont.
            // newer heuristics that I tried and didnt fully work:
            // - hint stop if num_written_ports(ay||spk) is significant, then we are playing; else hint loader. madmix2 will work; gauntlet wont.
            // - hint stop if hash of AY regs[0..13] changes every second. 
            // - hint stop if black/blue borders; else hint loader. gauntlet will work; p47 wont.
            // - hint stop if IN FE from slow page; else hint loader. firelord wont work.
            // @todo: more hints I didnt try
            // - hint stop if reading mouse port (~playing)
            // - hint stop if reading kempston(s) ports (~playing)
            // - hint stop if reading keyboard matrix other than SPACE or BREAK (~playing)
            // - hint stop if long pause (>=5s) found after tape progress is >30% (see: myth)
            // - hint stop if long pause (>=5s) found between turbo blocks (see: myth or gauntlet)

            // what I'm using now:
            // inspect how the game processes the IN FE byte. if byte is RRA or checked against 0x20 then byte is a loading one.
            // count the amount of loading bytes within a second: hint stop if count == 0; hint play if count > 200.

            if( mic_has_tape )
            {
                unsigned pc = PC(cpu) - 2;
                byte *addr = ADDR(pc);

                static unsigned numreads = 0;
                if( 0
                    || !memcmp(addr,  "\xDB\xFE\x1F", 3) // Common
                    || !memcmp(addr,  "\xDB\xFE\xA9\xE6\x40", 5) // P-47, BloodBrothers
                    || !memcmp(addr,  "\xDB\xFE\xA0\xC2\x88\xFD", 6) // lonewolf3 v1
                    || !memcmp(addr,  "\xDB\xFE\xA0\xCA\x48\xFD", 6) // lonewolf3 v2
                    || !memcmp(addr,  "\xED\x78\xA8\xE6\x40", 5) // trivial pursuits' questions
                    || !memcmp(addr-1,"\x2C\xDB\xFE\xA4\xC2", 5) // gremlin2: Basil the Mouse Detective (10 Great Games 2),  Mask,  Mask (10 Great Games 2)
                    || !memcmp(addr-1,"\x2C\xDB\xFE\xA4\xCA", 5) // gremlin2: Basil the Mouse Detective (10 Great Games 2),  Mask,  Mask (10 Great Games 2)
                )
                numreads++;

                static int freq = 0;
                static uint64_t last = 0;

                enum { SLICE = 1 };

                ++freq;

                if( (ticks - last) > 69888/SLICE ) {
                    tape_hz = freq*SLICE;
                    last = ticks, freq = 0;

    #if 0
                    // auto-tape issues:
                    // - not working well: turrican1
                    // - wont stop tape: *viaje al centro de la tierra, p47 thunderbolt, *dynamite dux, *outrun europa

                    if( mic_on ) {
                        if( !numreads ) {
                            printf("auto-stop tape (%u Hz, %u reads)\n", tape_hz, numreads);
                            mic_on = 0;
                        }
                    } else {
                        if( numreads > 200 ) { // 273Hz turrican
                            printf("auto-start tape (%u Hz, %u reads)\n", tape_hz, numreads);
                            mic_on = 1;
                        }
                    }
    #else
                    // fixes: turrican, p47,
                    // pending: wont stop tape: *viaje al centro de la tierra, *dynamite dux, *outrun europa (stop:228Hz)

                    if( numreads <= 9 ) { // 9Hz turrican, 228Hz outrun europa
                        if(mic_on) printf("auto-stop tape (%u Hz, %u reads)\n", tape_hz, numreads);
                        mic_on = 0;
                    }
                    if( numreads > 200 ) { // 273Hz turrican
                        if(!mic_on) printf("auto-start tape (%u Hz, %u reads)\n", tape_hz, numreads);
                        mic_on = 1;
                    }
    #endif
                    numreads = 0;
                }
            }
        }
        else
        {
            // if no auto-play...

            // initial auto-start tape based on rom trap. user needs to pause/resume tape manually (via F2 key).
            if(0)
            if(!mic_on) {
                unsigned pc = PC(cpu);
                bool rom1 = ZX<=48 || (ZX <= 200 && (page128 & 16));
                bool loading_from_rom = rom1 && (pc >= 0x04C2 && pc < 0x09F4); // (pc == 0x562 || pc == 0x5f1);
                if( loading_from_rom ) {
                    puts("auto-start tape (rom)");
                    mic_on = 1;
                }
            }
        }

        return code;
    }

    // unattached port, read from floating bus. +2A/+3 returns 0xFF under most circumstances.
    // When the Z80 reads from an unattached port it will read the data present on the ULA bus, which will be a display byte being transferred to the video circuits or 0xFF when idle, such as periods spent building the border.
    // A number of commercial games used the floating bus effect, generally to synchronise with the display and allow for flicker-free drawing. Games known to use the floating bus include:
    // - Arkanoid (original release only, the Hit Squad re-release does not use the floating bus)
    // - Cobra (original release only, the Hit Squad re-release does not use the floating bus)
    // - Sidewize
    // - Short Circuit
    if ((port & 0xFF) == 0xFF) {
        if( ZX >= 210 ) return 0xFF;
        unsigned tstate = ticks % (ZX < 128 ? 69888 : 70908);
        unsigned value = floating_bus[tstate] ? READ8(floating_bus[tstate]) : 0xFF;
        // debug: value = floating_bus[tstate] ? floating_bus[tstate] : 0xFF ; 
        // debug: printf("%d %d\n", tstate, value); // adjust
        return value;
    } // 48: 56 << 8 + 3..6 (0) = 14339..45 (vs 14336) // 128: 56 << 8 + 30 (28) = 14366 (vs 14364)

    return 0xFF;
    // return port & 1 ? 0xFF : 0x00;
}


#include "zx_sav.h"

#define FULL_QUICKSAVES 1 // 0 breaks run-a-head

struct quicksave {
    // cpu
    z80_t cpu;
    uint64_t pins;
    int int_counter;

    // rom patch
    int patched_rom;
    // control flags
    int ZX_TS;
    int ZX_FREQ;
    int ZX_RF;
    int ZX_CRT;
    int ZX;
    int ZX_FAST;
    // audio
    //int audio_pos;
    //float audio_buffer[AUDIO_BUFFER];
    // memory
    byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
    byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
    int vram_contended;
    int vram_accesses;

#if FULL_QUICKSAVES
    // byte contended[70908];
    byte floating_bus[70908];

    byte dum[16384];    // dummy page
    byte rom[16384*4];  // +3
    byte mem[16384*16]; // scorpion
#endif

    // screen
    int  ZXFlashFlag;
    byte ZXBorderColor;
    rgba ZXPalette[64];
    // input
    int keymap[5][5];
    // joysticks
    byte kempston,fuller;
    // mouse
    byte kempston_mouse;
    byte last_fe;
    // vsync
    byte zx_vsync;
    // tape
    int tape_hz;
    uint64_t tape_ticks;
    // ticks
    uint64_t ticks, TS;
    // boost
    byte boost_on;
    // plus3/2a
    byte page2a;
    // zx128
    byte page128;
    // audio
#if FULL_QUICKSAVES
    beeper_t buzz;
    struct ayumi ayumi;
    ay38910_t ay;
#endif
    // ay
    byte ay_current_reg;
    byte ay_registers[ 16 ];
    // ula+
    byte ulaplus_mode;
    byte ulaplus_data;
    byte ulaplus_enabled;
    byte ulaplus_grayscale;
    byte ulaplus_registers[64+1];

    // @todo: pointers needed?
    //#include "dsk.c"
#if FULL_QUICKSAVES
    t_FDC fdc;
    struct t_drive driveA;
    struct t_drive driveB;
#endif
    struct t_drive *active_drive; // reference to the currently selected drive
    t_track *active_track; // reference to the currently selected track, of the active_drive
    dword read_status_delay;
    byte *pbGPBuffer;

    // @todo: complete all missing
    //#include "tap.c"
    byte        mic,mic_on;
    int         mic_invert_polarity;
    int         mic_low;
    int         RAW_tstate_prev;
    uint64_t    RAW_fsm;
    int         mic_queue_wr;
    bool        mic_queue_has_turbo;

} quicksaves[10+1] = {0}; // user[0..9], sys/run-ahead reserved[10]

enum { quicksave_len = sizeof(struct quicksave) };

void* quicksave(unsigned slot) {
    if( slot >= 11) return 0;

    struct quicksave *c = &quicksaves[slot];

    #define $(member) \
        printf("%x..%x %s;\n", (int)offsetof(struct quicksave, member), (int)offsetof(struct quicksave, member) + (int)(sizeof( ((struct quicksave *)0)->member )), #member);

    // cpu
    c->cpu = cpu;
    c->pins = pins;
    c->int_counter = int_counter;
    // rom patch
    c->patched_rom = patched_rom;
    // control flags
    c->ZX_TS = ZX_TS;
    c->ZX_FREQ = ZX_FREQ;
    c->ZX_RF = ZX_RF;
    c->ZX_CRT = ZX_CRT;
    c->ZX = ZX;
    c->ZX_FAST = ZX_FAST;
    // audio
//  c->audio_pos = audio_pos;
//  float audio_buffer[AUDIO_BUFFER];
    // memory
//  byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
//  byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
    c->vram_contended = vram_contended;
    c->vram_accesses = vram_accesses;
#if FULL_QUICKSAVES
//  byte contended[70908];
//  byte floating_bus[70908];
    memcpy(c->dum, dum, 16384);
    memcpy(c->rom, rom, 16384*4);
    memcpy(c->mem, mem, 16384*16);
#endif
    // screen
    c->ZXFlashFlag = ZXFlashFlag;
    c->ZXBorderColor = ZXBorderColor;
    memcpy(c->ZXPalette, ZXPalette, sizeof(rgba)*64);
    // input
    memcpy(c->keymap, keymap, sizeof(int)*5*5);
    // joysticks
    c->kempston = kempston;
    c->fuller = fuller;
    // mouse
    c->kempston_mouse = kempston_mouse;
    // beeper
    c->last_fe = last_fe;
    // vsync
    c->zx_vsync = zx_vsync;
    // tape
    c->tape_hz = tape_hz;
    c->tape_ticks = tape_ticks;
    // ticks
    c->ticks = ticks;
    c->TS = TS;
    // boost
    c->boost_on = boost_on;
    // plus3/2a
    c->page2a = page2a;
    // zx128
    c->page128 = page128;
    // audio
#if FULL_QUICKSAVES
    c->buzz = buzz;
    c->ay = ay;
#endif
    // ay
    c->ay_current_reg = ay_current_reg;
    memcpy(c->ay_registers, ay_registers, sizeof(ay_registers));
    // ula+
    c->ulaplus_mode = ulaplus_mode;
    c->ulaplus_data = ulaplus_data;
    c->ulaplus_enabled = ulaplus_enabled;
    c->ulaplus_grayscale = ulaplus_grayscale;
    memcpy(c->ulaplus_registers, ulaplus_registers, sizeof(ulaplus_registers[0])*(64+1));
    // @todo
    //#include "dsk.c"
#if FULL_QUICKSAVES
    c->fdc = fdc;
    c->driveA = driveA;
    c->driveB = driveB;
#endif
    c->active_drive = active_drive;
    c->active_track = active_track;
    c->read_status_delay = read_status_delay;
    c->pbGPBuffer = pbGPBuffer;
    // @todo
    //#include "tap.c"
    c->mic = mic;
    c->mic_on = mic_on;
    c->mic_invert_polarity = mic_invert_polarity;
    c->mic_low = mic_low;
    c->RAW_tstate_prev = RAW_tstate_prev;
    c->RAW_fsm = RAW_fsm;
    c->mic_queue_wr = mic_queue_wr;
    c->mic_queue_has_turbo = mic_queue_has_turbo;

    return c;
}
void* quickload(unsigned slot) {
    if( slot >= 11) return 0;

    struct quicksave *c = &quicksaves[slot];

config(c->ZX);
port_0x7ffd(c->page128);

    // cpu
    cpu = c->cpu;
    pins = c->pins;
    int_counter = c->int_counter;
    // rom patch
    patched_rom = c->patched_rom;
    // control flags
    ZX_TS = c->ZX_TS;
    ZX_FREQ = c->ZX_FREQ;
    ZX_RF = c->ZX_RF;
    ZX_CRT = c->ZX_CRT;
    ZX = c->ZX;
    ZX_FAST = c->ZX_FAST;
    // audio
//  audio_pos = c->audio_pos;
//  float audio_buffer[AUDIO_BUFFER];
//  memset(audio_buffer, 0, sizeof(audio_buffer[0])*AUDIO_BUFFER);
    // memory
//  byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
//  byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
    vram_contended = c->vram_contended;
    vram_accesses = c->vram_accesses;
#if FULL_QUICKSAVES
//  byte contended[70908];
//  byte floating_bus[70908];
    memcpy(dum, c->dum, 16384);
    memcpy(rom, c->rom, 16384*4);
    memcpy(mem, c->mem, 16384*16);
#endif
    // screen
    ZXFlashFlag = c->ZXFlashFlag;
    ZXBorderColor = c->ZXBorderColor;
    memcpy(ZXPalette, c->ZXPalette, sizeof(rgba)*64);
    // input
    memcpy(keymap, c->keymap, sizeof(int)*5*5);
    // joysticks
    kempston = c->kempston;
    fuller = c->fuller;
    // mouse
    kempston_mouse = c->kempston_mouse;
    // beeper
    last_fe = c->last_fe;
    // vsync
    zx_vsync = c->zx_vsync;
    // tape
    tape_hz = c->tape_hz;
    tape_ticks = c->tape_ticks;
    // ticks
    ticks = c->ticks;
    TS = c->TS;
    // boost
    boost_on = c->boost_on;
    // plus3/2a
    page2a = c->page2a;
    // zx128
    page128 = c->page128;
    // audio
#if FULL_QUICKSAVES
    buzz = c->buzz;
    ay = c->ay;
#endif
    // ay
    ay_current_reg = c->ay_current_reg;
    memcpy(ay_registers, c->ay_registers, sizeof(ay_registers));
    // ula+
    ulaplus_mode = c->ulaplus_mode;
    ulaplus_data = c->ulaplus_data;
    ulaplus_enabled = c->ulaplus_enabled;
    ulaplus_grayscale = c->ulaplus_grayscale;
    memcpy(ulaplus_registers, c->ulaplus_registers, sizeof(ulaplus_registers[0])*(64+1));
    // @todo
    //#include "dsk.c"
#if FULL_QUICKSAVES
    fdc = c->fdc;
    driveA = c->driveA;
    driveB = c->driveB;
#endif
    active_drive = c->active_drive;
    active_track = c->active_track;
    read_status_delay = c->read_status_delay;
    pbGPBuffer = c->pbGPBuffer;
    // @todo
    //#include "tap.c"
    mic = c->mic;
    mic_on = c->mic_on;
    mic_invert_polarity = c->mic_invert_polarity;
    mic_low = c->mic_low;
    RAW_tstate_prev = c->RAW_tstate_prev;
    RAW_fsm = c->RAW_fsm;
    mic_queue_wr = c->mic_queue_wr;
    mic_queue_has_turbo = c->mic_queue_has_turbo;

    return c;
}
