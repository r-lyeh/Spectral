int do_audio = 1;

// todo (trap)
// [ ] trap rom loading, edge detection
// [ ] disable boost if tape == tape_prev for +1s
// [ ] tzx block scanner
//
// todo (tapes)
// [ ] overlay ETA
// [ ] auto-stop tape, auto-rewind at end of tape if multiload found (auto-stop detected), auto-insert next tape at end of tape (merge both during tzx_load! argv[1] argv[2])
//
// test
// [ ] border: sentinel,
// [ ] timing: shock megademo, MDA
// [ ] test db [y/n/why]
//
// [ ] loaders: black arrow, black tiger, blood brothers, bobby bearing, joe blade 2, fairlight,
// [ ] loaders: fighting warrior, moonstrike, rigel's revenge, trap door, travel trashman,
// [ ] loaders: return of bart bear, ballbreaker, cobra, critical, deflektor, bubble bobble/flying shark/soldier of fortune,
// [ ] loaders: freddy hardest, gunrunner, joe 128, indy last crusade, locomotion, amc, atlantis,
// [ ] loaders: lode runner, podraz 32, saboteur, wizball.tap, manic miner, scooby doo, splat,
// [ ] loaders: star wars, technician ted, three weeks in paradise, uridium/firelord, xeno, ik+,
// [ ] loaders:
// [ ] loaders: spirits, song in 3 lines,
//
// fix: mic_on @ eot
// fix: exolon main tune requires contended memory, otherwise it's too fast +15% tempo


// ZX

#define ZX_TS_48K     69888
#define ZX_TS_128K    70908
#define ZX_FREQ_48K   3500000
#define ZX_FREQ_128K  3546894

int ZX_TS;
int ZX_FREQ;
int ZX_TV = 1;
int ZX = 128; // 48, 128, 200 (+2), 210 (+2A), 300 (+3)
int ZX_FAST = 1;

    byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
    byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
    #define RAM_BANK(n)   (mem + (n) * 0x4000)
    #define ROM_BANK(n)   (rom + (n) * 0x4000)
    #define DUMMY_BANK(n) (dum + (0) * 0x4000)
    #define VRAM /*(MEMr[1])*/ (page128 & 8 ? RAM_BANK(7) : RAM_BANK(5))

    int vram_contended;
    int vram_accesses;
    byte contended[70908];

    byte *mem = 0; // 48k
    byte *rom = 0;
    byte *dum = 0;

    z80_t cpu;

    #define READ8(a)     (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(byte *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define READ16(a)    (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(word *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define WRITE8(a,v)  (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(byte *)&MEMw[(a)>>14][(a)&0x3FFF]=(v))
    #define WRITE16(a,v) (/*vram_accesses += vram_contended && ((a)>>14==1),*/ *(word *)&MEMw[(a)>>14][(a)&0x3FFF]=(v))


byte page128;
byte ZXBorderColor;

void port_0x7ffd (byte value);
void outport_0x1ffd(byte value);
void port_0xfffd(byte value);
void port_0xbffd(byte value);
void reset(int model);

#include "zx_rom.h"
#include "zx_dsk.h"
#include "zx_tap.h" // requires page128
#include "zx_tzx.h"
#include "zx_sna.h" // requires page128, ZXBorderColor

void outport(word port, byte value);
byte inport(word port);


int  ZXFlashFlag;
rgba ZXPalette[64], ZXPaletteDef[64] = { // 16 regular, 64 ulaplus
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
    rgb(0x00,0x00,0xD8),
    rgb(0xD8,0x00,0x00),
    rgb(0xD8,0x00,0xD8),
    rgb(0x00,0xD8,0x00),
    rgb(0x00,0xD8,0xD8),
    rgb(0xD8,0xD8,0x00),
    rgb(0xD8,0xD8,0xD8),

    rgb(0x06,0x08,0x00), // bright: black,blue,red,pink,green,cyan,yellow,white
    rgb(0x00,0x00,0xFF),
    rgb(0xFF,0x00,0x00),
    rgb(0xFF,0x00,0xFF),
    rgb(0x00,0xFF,0x00),
    rgb(0x00,0xFF,0xFF),
    rgb(0xFF,0xFF,0x00), // rgb(0xEE,0xEB,0x46), brighter yellow because jacknipper2 looks washed
    rgb(0xFF,0xFF,0xFF)
#elif 1 // latest
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
#define ZXKey(a) do { keymap[ keytbl[a][0] ][ keytbl[a][1] ] &= keytbl[a][2]; } while(0)
#define ZXKeyUpdate() \
keymap[1][1] = keymap[1][2] = keymap[2][2] = keymap[3][2] = keymap[4][2] = \
keymap[4][1] = keymap[3][1] = keymap[2][1] = 0xFF;
int keymap[5][5];
const unsigned char keytbl[256][3] = {
    {1, 2, 0xFE}, /* 0 */ {1, 1, 0xFE}, /* 1 */ {1, 1, 0xFD},   /* 2 */
    {1, 1, 0xFB}, /* 3 */ {1, 1, 0xF7}, /* 4 */ {1, 1, 0xEF},   /* 5 */
    {1, 2, 0xEF}, /* 6 */ {1, 2, 0xF7}, /* 7 */ {1, 2, 0xFB},   /* 8 */
    {1, 2, 0xFD},       /* 9 */
    {3, 1, 0xFE}, /* a */ {4, 2, 0xEF}, /* b */ {4, 1, 0xF7},   /* c */
    {3, 1, 0xFB}, /* d */ {2, 1, 0xFB}, /* e */ {3, 1, 0xF7},   /* f */
    {3, 1, 0xEF}, /* g */ {3, 2, 0xEF}, /* h */ {2, 2, 0xFB},   /* i */
    {3, 2, 0xF7}, /* j */ {3, 2, 0xFB}, /* k */ {3, 2, 0xFD},   /* l */
    {4, 2, 0xFB}, /* m */ {4, 2, 0xF7}, /* n */ {2, 2, 0xFD},   /* o */
    {2, 2, 0xFE}, /* p */ {2, 1, 0xFE}, /* q */ {2, 1, 0xF7},   /* r */
    {3, 1, 0xFD}, /* s */ {2, 1, 0xEF}, /* t */ {2, 2, 0xF7},   /* u */
    {4, 1, 0xEF}, /* v */ {2, 1, 0xFD}, /* w */ {4, 1, 0xFB},   /* x */
    {2, 2, 0xEF}, /* y */ {4, 1, 0xFD}, /* z */
    {4, 2, 0xFE}, /*SPACE*/
    {3, 2, 0xFE}, /*ENTER*/
    {4, 1, 0xFE}, /*RSHIFT*/ {4, 2, 0xFD}, /*ALT*/ {1, 2, 0xEF}, /*CTRL*/
};

// joysticks
byte kempston,fuller;

// mouse
byte kempston_mouse;

// beeper
beeper_t buzz;
byte last_fe;

// vsync
byte zx_vsync;

// ticks
uint64_t ticks, TS;

// boost
byte boost_on;

// plus3/2a
byte page2a;

// zx128
void port_0x7ffd (byte value) {
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

// fdc
// fdc_ports.c -----------------------------------------------------------------

// control of port 0x1ffd (+2a/+3 -> mem/fdc/ptr)
void outport_0x1ffd(byte value) {
    if (page2a & 128) return; //if not in +2a/+3 mode, return

    page2a = value & 0x1f;    //save bits 0-4

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
    fdc_motor(value & 8);

    //bit 4: printer strobe
}

// control of port 0x2ffd (fdc in)
byte inport_0x2ffd(void) {
    return ZX == 300 ? fdc_read_status() : 0xFF;
}

// control of port 0x3ffd (fdc out & in)
void outport_0x3ffd(byte value) {
    if( ZX == 300 ) fdc_write_data(value);
}

byte inport_0x3ffd(void) {
    return ZX == 300 ? fdc_read_data() : 0xFF;
}



void checkpoint_save(unsigned slot);
void checkpoint_load(unsigned slot);


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

        page128=32;
        page2a=128;
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
        page2a=128;
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
}


// control of AY-3-8912 ports
#if !AYUMI
ay38910_t ay;
#endif
byte ay_current_reg;
int/*byte*/ ay_registers[ 16 ];
void port_0xfffd(byte value) {
    ay_current_reg=(value&15);
#if !AYUMI
    // select ay-3-8912 register
    ay38910_iorq(&ay, AY38910_BDIR|AY38910_BC1|ay_current_reg<<16);
#endif

#if 1 // stop the tape when AY activity is detected
    // mic_on = 0;
#endif
}
void port_0xbffd(byte value) {
//mic_on = 0;
    ay_registers[ay_current_reg]=value;
#if AYUMI
    int *r = ay_registers;
    switch (ay_current_reg)
    {
    case 0:
    case 1:
        ayumi_set_tone(&ay, 0, (r[1] << 8) | r[0]);
        break;
    case 2:
    case 3:
        ayumi_set_tone(&ay, 1, (r[3] << 8) | r[2]);
        break;
    case 4:
    case 5:
        ayumi_set_tone(&ay, 2, (r[5] << 8) | r[4]);
        break;
    case 6:
        ayumi_set_noise(&ay, r[6]);
        break;
    case 8:
        ayumi_set_mixer(&ay, 0, r[7] & 1, (r[7] >> 3) & 1, r[8] >> 4);
        ayumi_set_volume(&ay, 0, r[8] & 0xf);
        break;
    case 9:
        ayumi_set_mixer(&ay, 1, (r[7] >> 1) & 1, (r[7] >> 4) & 1, r[9] >> 4);
        ayumi_set_volume(&ay, 1, r[9] & 0xf);
        break;
    case 10:
        ayumi_set_mixer(&ay, 2, (r[7] >> 2) & 1, (r[7] >> 5) & 1, r[10] >> 4);
        ayumi_set_volume(&ay, 2, r[10] & 0xf);
        break;
    case 7:
        ayumi_set_mixer(&ay, 0, r[7] & 1, (r[7] >> 3) & 1, r[8] >> 4);
        ayumi_set_mixer(&ay, 1, (r[7] >> 1) & 1, (r[7] >> 4) & 1, r[9] >> 4);
        ayumi_set_mixer(&ay, 2, (r[7] >> 2) & 1, (r[7] >> 5) & 1, r[10] >> 4);
        break;
    case 11:
    case 12:
        ayumi_set_envelope(&ay, (r[12] << 8) | r[11]);
        break;
    case 13:
        if (r[13] != 255) 
        ayumi_set_envelope_shape(&ay, r[13]);
        break;
    }
#else
    ay38910_iorq(&ay, AY38910_BDIR|value<<16);
#endif
}
byte port_0xfffd_in(void) {
//mic_on = 0;
//  return ay38910_iorq(&ay, 0);
    unsigned char ay_registers_mask[ 16 ] = {
        0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x1f, 0xff,
        0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x0f, 0xff, 0xff,
    };

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

int int_counter = 0;
uint64_t pins = 0;

void init() {
    dum = (char*)realloc(dum, 16384);    // dummy page
    rom = (char*)realloc(rom, 16384*4);  // +3
    mem = (char*)realloc(mem, 16384*16); // scorpion

    pins = z80_init(&cpu);

    beeper_desc_t bdesc = {0};
    bdesc.tick_hz = ZX_FREQ; // ZX_FREQ_48K
    bdesc.sound_hz = AUDIO_FREQUENCY;
    bdesc.base_volume = 1.f; // 0.25f
    beeper_init(&buzz, &bdesc);

#if AYUMI
    const int is_ym = 0;
    const int eqp_stereo_on = 0;
    const double pan_ABC[7][3] = { // pan modes, 7 stereo types
      {0.50, 0.50, 0.50}, // MONO
      {0.10, 0.50, 0.90}, // ABC
      {0.10, 0.90, 0.50}, // ACB
      {0.50, 0.10, 0.90}, // BAC
      {0.90, 0.10, 0.50}, // BCA
      {0.50, 0.90, 0.10}, // CAB
      {0.90, 0.50, 0.10}, // CBA
    };

    if (!ayumi_configure(&ay, is_ym, ZX_FREQ / 2, AUDIO_FREQUENCY)) {
        exit(-printf("ayumi_configure error (wrong sample rate?)\n"));
    }
    const double *pan = pan_ABC[0]; // mono
    ayumi_set_pan(&ay, 0, pan[0], eqp_stereo_on);
    ayumi_set_pan(&ay, 1, pan[1], eqp_stereo_on);
    ayumi_set_pan(&ay, 2, pan[2], eqp_stereo_on);
#else
    ay38910_desc_t ay_desc = {0};
    ay_desc.type = AY38910_TYPE_8912;
    ay_desc.tick_hz = ZX_FREQ / 2;
    ay_desc.sound_hz = AUDIO_FREQUENCY; // * 0.75; // fix: -25% speed
    ay_desc.magnitude = 1.0f;
    ay38910_init(&ay, &ay_desc);
#endif

    memset(ay_registers,0,16);
    ay_current_reg=0;
}

// reset most systems. preserve mic
void reset(int model) {
    ZX = model; // (model == 16 || model == 48 ? 48 : 128);

    ZX_TS = ZX < 128 ? ZX_TS_48K : ZX_TS_128K;
    ZX_FREQ = ZX < 128 ? ZX_FREQ_48K : ZX_FREQ_128K;
    init(); // static int ever = 0; if(!ever) { ever = 1; init(); }

    page2a = 128;
    page128 = ZX < 128 ? 32 : 0;
    last_fe = 0;

//    memset(mem, 0x00, 128*1024);
//    memcpy(rom, ZX < 128 ? rom48 : rom128, ZX < 128 ? 16384 : 2*16384);
    rom_patch(0);

    beeper_reset(&buzz);
#if AYUMI
    ayumi_reset(&ay);
#else
    ay38910_reset(&ay);
#endif

    void ula_reset();
    ula_reset();

    kempston_mouse = 0;

config(ZX);

    AZIMUTH = AZIMUTH_DEFAULT;

    pins = z80_reset(&cpu);

    ticks=0;

    for( int i = 1, top = __argc; i < top; ++i ) {
        __argc = 0;
        if( __argv[i][0] != '-' ) {
            if( !load(__argv[i], 1, 1) ) {
                exit(-fprintf(stderr, "cannot open '%s' file\n", __argv[i]));
            }
        }
    }
}

// reset & clear all systems
void clear(int model) {
    reset(model);

    mic_reset();
}

void sim(int TS) {
    if(TS<0)return;

    for(int i = 0; i < TS; ++i) {
        // printf("%x %llx\n", cpu.pc, pins);
        // dis2(cpu.pc);
        pins = z80_tick(&cpu, pins);
        ++ticks;

        if (zx_vsync) {
zx_vsync = 0;
            // request vblank interrupt
            pins |= Z80_INT;
            // hold the INT pin for 32 ticks
            int_counter = 32; // 32;
        }

        // clear INT pin after 32 ticks
        if (pins & Z80_INT) {
            if (--int_counter < 0) {
                pins &= ~Z80_INT;
            }
        }

        // mem
        if( pins & Z80_MREQ ) {
            if( pins & Z80_RD ) {
                Z80_SET_DATA(pins, READ8(Z80_GET_ADDR(pins)));
            }
            else if( pins & Z80_WR ) {
                WRITE8(Z80_GET_ADDR(pins), Z80_GET_DATA(pins));
            }
        }
        // ports
        else if( pins & Z80_IORQ ) {
            if( pins & Z80_RD ) {
                Z80_SET_DATA(pins, inport(Z80_GET_ADDR(pins)));
            }
            else if( pins & Z80_WR ) {
                outport(Z80_GET_ADDR(pins), Z80_GET_DATA(pins));
            }
        }

#if 1
        sys_audio();
#endif
    }
}

void sys_audio() {
    // tick the beeper (full frequency)
    bool sample_ready = beeper_tick(&buzz);

    // tick the AY (half frequency)
    static float ay_sample = 0;
#if AYUMI
    static byte even = 0; ++even; if( even == 0 || even == 0x80 ) {
        // update_ayumi_state(&ay, ay_registers);
        ay_sample = ayumi_render(&ay, ay_registers, 50.00 / AUDIO_FREQUENCY); // frame_rate / audio_rate
    }    
#else
    static byte even = 0; if( ++even & 1 ) {
        ay38910_tick(&ay); ay_sample = ay.sample;
    }
#endif

    if( do_audio && sample_ready ) {
        float master = 0.95f * ( (mic_on && mic_has_tape) ? 0.05 : 1 );
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

    // contended lookups
    memset(contended, 0, sizeof(contended));
    for( int i = 63, c = 6; i <= (63+192); ++i, --c ) {
        contended[i*228]=c<0?0:c; // initial: 64*224 (48K)
        if(c<0) c=6;
    }

    // colorize
    for( int i = 0; i < 16; ++i) {
        unsigned r,g,b; byte h,s,v;
        rgb_split(ZXPalette[i],r,g,b);

                    //unsigned luma = (unsigned)((r * 0.299 + g * 0.587 + 0.114));
                    //luma *= 1.25; if( luma > 255 ) luma = 255;
                    //ZXPalette[i] = rgb(luma,luma,luma);

        // saturated
                    rgb2hsv(r,g,b,&h,&s,&v);
                    s = s*1.000 < 255 ? s*1.000 : 255;
                    v = v*1.125 < 255 ? v*1.125 : 255;
                    ZXPalette[i] = as_rgb(h,s,v);
                    continue;

        // bw
                    rgb2hsv(r,g,b,&h,&s,&v);
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
        if (!(port & (0xFFFF^0x1FFD))) { outport_0x1ffd(value); return; }
        if (!(port & (0xFFFF^0x3FFD))) { outport_0x3ffd(value); return; }
    }

    // sorted ports
    if (ZX >= 128) if(!(port & (0xFFFF^0x7FFD)))     { port_0x7ffd(value); return; }
    // ula+ register port
    if (!(port & (0xFFFF^0xBF3B))) {
        ulaplus_data = value & 0x3f;
        ulaplus_mode = value >> 6;
    }
    // ay
    if (!(port & (0xFFFF^0xBFFD)))     { port_0xbffd(value); return; } // ay
    // ula+ data port
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
        last_fe = value;

        // border color
        ZXBorderColor = (value & 0x07);

        // speaker
        //0x08 : tape when saving
        //0x10 : speaker
        //0x18 : both. works for 99% of games, some others wont (parapshock values: 0x18,0x08,0x18,...)
        //mic  : tape when loading

        int is_saving = (page128&16) && (cpu.pc >= 0x4ae && cpu.pc <= 0x51a);
        int mask = is_saving ? 0x08 : 0x10;
        int spk = mic_on ? mic : 0;

        beeper_set(&buzz, !!(spk || (value & mask)));
    }
}
byte inport(word port) {
    // unsigned PC = z80_pc(&cpu); if(PC >= 0x3D00 && PC <= 0x3DFF) printf(" in port[%04x]\n", port); // betadisk

    // if(port != 0x1ffd && port != 0x2ffd && port != 0x3ffd && port != 0x7ffe) { regs(); printf(" in port[%04x]\n", port); }
    //[0102] alien syndrome.dsk ??
    //[0002] alien syndrome.dsk ??
    //[fefe] alien syndrome.dsk

    // kempston mouse detection
    if(port == 0xFADF) kempston_mouse |= 1;
    if(port == 0xFBDF) kempston_mouse |= 2;
    if(port == 0xFFDF) kempston_mouse |= 4;

    // kempston mouse
    if( kempston_mouse == 7 ) {
//mic_on = 0;
        extern Tigr *app;
        int mx, my, mb, lmb, mmb, rmb; // buttons: R2M1L0 bits
        tigrMouse(app, &mx, &my, &mb); lmb = mb & 1; mmb = !!(mb & 2); rmb = !!(mb & 4);
        RECT rect; GetWindowRect((HWND)app->handle, &rect); //int hw = (rect.right - rect.left)/2, hh = (rect.bottom - rect.top)/2; rect.left+=hw; rect.right-=hw; rect.top+=hh; rect.bottom-=hh;
        if(!(port & (0xFFFF^0xFADF))) return ShowCursor(FALSE), ClipCursor(&rect), /*SetCursorPos((rect.right-rect.left)/2,(rect.bottom-rect.top)/2),*/ 0xFF&~(1*rmb+2*lmb+4*mmb); // ----.--10.--0-.----  =  Buttons: D0 = RMB, D1 = LMB, D2 = MMB
        if(!(port & (0xFFFF^0xFBDF))) return ShowCursor(FALSE), ClipCursor(&rect), /*SetCursorPos((rect.right-rect.left)/2,(rect.bottom-rect.top)/2),*/  mx;                       // ----.-011.--0-.----  =  X-axis:  $00 … $FF
        if(!(port & (0xFFFF^0xFFDF))) return ShowCursor(FALSE), ClipCursor(&rect), /*SetCursorPos((rect.right-rect.left)/2,(rect.bottom-rect.top)/2),*/ -my;                       // ----.-111.--0-.----  =  Y-axis:  $00 … $FF
    }
    // kempston joystick
    else {
        if(!(port & (0xFF^0xDF))) return /*mic_on = 0,*/ kempston; //bit 5 low = reading kempston (as told pera putnik)
        //if(!(port & 0xE0)) return kempston;          //bit 7,6,5 low = reading kempston (as floooh)
    }

    // +2a/+3
    if( ZX >= 210 ) {
    if(!(port & (0xFFFF^0x2FFD))) return inport_0x2ffd();
    if(!(port & (0xFFFF^0x3FFD))) return inport_0x3ffd();
    }

    // ulaplus read data
    if (!(port & (0xFFFF^0xFF3B))) {
        if( ulaplus_mode == 0 ) {
            return ulaplus_registers[ulaplus_data];
        }
        if( ulaplus_mode != 0 ) {
            return ulaplus_registers[64];
        }
    }

    // ay
    if(!(port & (0xFFFF^0xFFFD))) return port_0xfffd_in();

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

        /*
        I should implement this also:
        where earon = bit 4 of the last OUT to the 0xFE port
        and   micon = bit 3 of the last OUT to the 0xFE port
        byte ear_on = last_fe & (1<<4);
        byte mic_on_= last_fe & (1<<3);
        if( !ear_on && mic_on_) code &= 0xbf;
        */

        if( 1 ) { // mic_on
#if 0
            static int counter = 0;
            static int last_out = 0;
            if( last_fe == last_out ) {
                if( last_fe == 0 && ++counter > 25600 ) {
                    mic_on = 0;
                    counter = 0;
                }
            } else {
                counter = 0;
                last_out = last_fe;
            }
            if( !mic_on ) {
                return code;
            }
#endif

            byte ear = ReadMIC(ticks);

            if(mic_on)
            beeper_set(&buzz, !!((last_fe & 0x10) | !!ear));

            code = code ^ ear;
        }

        return code;
    }

    // unattached port, read from floating bus. +2A/+3 returns 0xFF always (Cobra)
    // When the Z80 reads from an unattached port it will read the data present on the ULA bus, which will be a display byte being transferred to the video circuits or 0xFF when idle, such as periods spent building the border.
    // A number of commercial games used the floating bus effect, generally to synchronise with the display and allow for flicker-free drawing. Games known to use the floating bus include:
    // Arkanoid (original release only, the Hit Squad re-release does not use the floating bus)
    // Cobra (original release only, the Hit Squad re-release does not use the floating bus)
    // Sidewize
    // Short Circuit
    if ((port & 0xFF) == 0xFF) {
        int scanline = (ticks % 70908) / 311;
        if( scanline < 64 || scanline >= (64+192) ) return 0xFF;
        int raster = (ticks % 70908) % 228;
        if( raster < 48 || raster >= (48+128) ) return 0xFF;
        return 0x00;
    }

    return 0xFF; // port & 1 ? 0xFF : 0x00;
}


struct checkpoint {
    int version;

    // rom patch
    int patched_rom;
    // control flags
    int ZX_TS;
    int ZX_FREQ;
    int ZX_TV;
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
    byte contended[70908];

    byte dum[16384];    // dummy page
    byte rom[16384*4];  // +3
    byte mem[16384*16]; // scorpion

    // cpu
    z80_t cpu;
    uint64_t pins;
    int int_counter;
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
    // beeper
    beeper_t buzz;
    byte last_fe;
    // vsync
    byte zx_vsync;
    // ticks
    uint64_t ticks, TS;
    // boost
    byte boost_on;
    // plus3/2a
    byte page2a;
    // zx128
    byte page128;
    // ay
#if AYUMI
    struct ayumi ay;
#else
    ay38910_t ay;
#endif
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
    t_FDC FDC;
    byte *pbGPBuffer;
    struct t_drive driveA;
    struct t_drive driveB;
    struct t_drive *active_drive; // reference to the currently selected drive
    t_track *active_track; // reference to the currently selected track, of the active_drive
    dword read_status_delay;

    // @todo: complete all missing
    //#include "tap.c"
    byte        mic,mic_on,mic_has_tape;
    int         mic_last_block;
    const char *mic_last_type;
    int         mic_invert_polarity;
    int         mic_low;
    int         RAW_repeat, RAW_pulse;
    int         RAW_tstate_prev, RAW_tstate_curr, RAW_tstate_goal;
    int         RAW_blockinfo;
    uint64_t    RAW_fsm;
    int         mic_queue_rd, mic_queue_wr;
    bool        mic_queue_has_turbo;

} checkpoints[10+1] = {0}; // user[0..9], sys/run-ahead reserved[10]

void checkpoint_save(unsigned slot) {
    if( slot >= 11) return;

    struct checkpoint *c = &checkpoints[slot];
    c->version = 0x100;

    // rom patch
    c->patched_rom = patched_rom;
    // control flags
    c->ZX_TS = ZX_TS;
    c->ZX_FREQ = ZX_FREQ;
    c->ZX_TV = ZX_TV;
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
//  byte contended[70908];
    memcpy(c->dum, dum, 16384);
    memcpy(c->rom, rom, 16384*4);
    memcpy(c->mem, mem, 16384*16);
    // cpu
    c->cpu = cpu;
    c->pins = pins;
    c->int_counter = int_counter;
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
    c->buzz = buzz;
    c->last_fe = last_fe;
    // vsync
    c->zx_vsync = zx_vsync;
    // ticks
    c->ticks = ticks;
    c->TS = TS;
    // boost
    c->boost_on = boost_on;
    // plus3/2a
    c->page2a = page2a;
    // zx128
    c->page128 = page128;
    // ay
    c->ay = ay;
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
    c->FDC = FDC;
    c->pbGPBuffer = pbGPBuffer;
    c->driveA = driveA;
    c->driveB = driveB;
    c->active_drive = active_drive;
    c->active_track = active_track;
    c->read_status_delay = read_status_delay;
    // @todo
    //#include "tap.c"
    c->mic = mic;
    c->mic_on = mic_on;
    c->mic_has_tape = mic_has_tape;
    c->mic_last_block = mic_last_block;
    c->mic_last_type = mic_last_type;
    c->mic_invert_polarity = mic_invert_polarity;
    c->mic_low = mic_low;
    c->RAW_repeat = RAW_repeat;
    c->RAW_pulse = RAW_pulse;
    c->RAW_tstate_prev = RAW_tstate_prev;
    c->RAW_tstate_curr = RAW_tstate_curr;
    c->RAW_tstate_goal = RAW_tstate_goal;
    c->RAW_blockinfo = RAW_blockinfo;
    c->RAW_fsm = RAW_fsm;
    c->mic_queue_rd = mic_queue_rd;
    c->mic_queue_wr = mic_queue_wr;
    c->mic_queue_has_turbo = mic_queue_has_turbo;
}
void checkpoint_load(unsigned slot) {
    if( slot >= 11) return;

    struct checkpoint *c = &checkpoints[slot];
    if(c->version != 0x100) return;

config(c->ZX);
port_0x7ffd(c->page128);

    // rom patch
    patched_rom = c->patched_rom;
    // control flags
    ZX_TS = c->ZX_TS;
    ZX_FREQ = c->ZX_FREQ;
    ZX_TV = c->ZX_TV;
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
//  byte contended[70908];
    memcpy(dum, c->dum, 16384);
    memcpy(rom, c->rom, 16384*4);
    memcpy(mem, c->mem, 16384*16);
    // cpu
    cpu = c->cpu;
    pins = c->pins;
    int_counter = c->int_counter;
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
    buzz = c->buzz;
    last_fe = c->last_fe;
    // vsync
    zx_vsync = c->zx_vsync;
    // ticks
    ticks = c->ticks;
    TS = c->TS;
    // boost
    boost_on = c->boost_on;
    // plus3/2a
    page2a = c->page2a;
    // zx128
    page128 = c->page128;
    // ay
    ay = c->ay;
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
    FDC = c->FDC;
    pbGPBuffer = c->pbGPBuffer;
    driveA = c->driveA;
    driveB = c->driveB;
    active_drive = c->active_drive;
    active_track = c->active_track;
    read_status_delay = c->read_status_delay;
    // @todo
    //#include "tap.c"
    mic = c->mic;
    mic_on = c->mic_on;
    mic_has_tape = c->mic_has_tape;
    mic_last_block = c->mic_last_block;
    mic_last_type = c->mic_last_type;
    mic_invert_polarity = c->mic_invert_polarity;
    mic_low = c->mic_low;
    RAW_repeat = c->RAW_repeat;
    RAW_pulse = c->RAW_pulse;
    RAW_tstate_prev = c->RAW_tstate_prev;
    RAW_tstate_curr = c->RAW_tstate_curr;
    RAW_tstate_goal = c->RAW_tstate_goal;
    RAW_blockinfo = c->RAW_blockinfo;
    RAW_fsm = c->RAW_fsm;
    mic_queue_rd = c->mic_queue_rd;
    mic_queue_wr = c->mic_queue_wr;
    mic_queue_has_turbo = c->mic_queue_has_turbo;
}
