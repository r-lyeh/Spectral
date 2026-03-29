// known issues:

// .dsk
// @obo: You can pass all the Speedlock weak sector protections with two data patterns: either a fully weak sector with everything changing, or a partially weak sector. If the first 255 bytes of the sector are the same value it's a partial weak, otherwise full weak.  ALL the partial weak sector protections can be passed with 336 bytes reliable followed by 176 bytes weak/changing.
// @obo: The true partial pattern is 256 bytes reliable, 32 bytes weak, 48 bytes reliable filler, 176 bytes weak.  The filler come after a weak patch so the FDC sync is unreliable on real hardware and the value changes, but the most of the run of 48 needs to be the same byte to pass.

// .sav
// - [x] MDA demo cannot restore the state fully (AY regs?)
// - [x] tape marker not very exact in turborom medias
// - [x] may be corrupt. repro steps: many save/loads (see: jacknipper2 menu, mask3 menu, RAIDERS.ROM screen$)
// media:
// - [x] scl not being saved on exit (borderbreak)
// auto-locale:
// - [x] will corrupt medias with checksums (like tap files)

// Woodster "overscan.tap: that LD (#8182),HL changes the int vector for the next int to end the loop 
// so break at #8244 to get the loop value; #0C33 for 48k (14L), #0C71 for 128k (13L), #0E20 for +3 (15L)"
// i got #0CC3 (+144), #0D01 (+144), #0DE1 (-63)

// ZXDunny "Standard acceleration == max Ts per frame, emulation as normal
// Edge loading == shortcut tape edges so they take one emulated instruction
// Flash loading == grab all the data, figure out where it goes, inject it and exit the loader entirely"

// tape
// - red/cyan tape freq bars

// pzx
// - load_flowcontrol\HollywoodPoker.pzx
// - load_gdb\Book Of The Dead - Part 1 (CRL).pzx
// tzx:
// - flow

// gallery
// - 0-byte .zips on Linux
// - thumbnail() wont decode ifl/mlt/mc

// zxdb
// - [x] infos get lost between different .sav sessions
// - JACKNIP.TAP ; difficult to get it right without hyphenation. best we could do for now is search JACKNIP%
// - instructions: utf8 bom
// - maps: battle valley, river raid
// - [x] indianajones.dsk
// - paperboy2-sidea(amstrad).dsk / sideb(zx)

// embedded zxplayer
// - cant load zip/rar files because FILE *fp is not pointing at seek pos

// trdos
// - trdos+L mode
// - page in .z80 not handled

// 128/+2:
// - parapshock should only load in 48 mode. it loads fine in 128/+2 for some reason (POKE STREAM #254)
// .ay files:
// - could use left/right keys to change tracks
// - could fill vram so it displays author, filename, etc datas. or use ui_notify() at least
// - felt the 0x8000/0x10000 magic numbers in zx_ay can be removed if we switch vars to be unsigned
// .sav files:
// - may be corrupt. repro steps: save during tape load or disk load
// altroms:
// - donkeykong(erbe) (128): no ay (wrong checksum?)
// - bloodbrothers (128): no ay (wrong checksum?)
// - travelwithtrashman (128): crash (wrong checksum?)
// - r-type128-km-nocheats (lg18v07 + usr0): r tape err (no turbo) or crash (turbo)
// - lg18,gw03,sebasic fail in the games above. jgh v0.77 seems to work. old sebasic partially too
// ay:
// - Fuller Box AY chip is clocked at 1.63819 MHz
// beeper:
// - indiana jones last crusade. what was the problem again?
// - OddiTheViking128
// - p-47 in-game click sound (ayumi? beeper?). double check against real zx spectrum
// disciple/plusd/mgt (lack of):
// - zxoom.z80
// lightgun:
// - gunstick + (kempston || kmouse) conflicts
// mouse:
// - kms window focus @ MMB+tigr, kms wrap win borders, kms fullscreen, kms coord wrap (inertia, r-type128-km)
// - load rtype-km, load another game or reset, notice no mouse cursor
// pentagon:
// - AY should clock at 1.75 MHz exact, CPU at 3.50 MHz exact
// - no autoboot (autoboot is activated by paging in TR-DOS rom and jumping to 0)
//   "Unreal Speccy Portable checks for a boot.B file in firsts sectors. If not found, send two keystrokes (Enter). That only makes sense on Pentagon machines with the reset service rom (gluck), the first Enter show a list of files and the latter selects the first file."
// ports:
// - linux: no mouse clip
// - osx: no mouse clip
// - osx: retina too heavy?
// turborom:
// - x4,x6 modes not working anymore. half bits either.

FILE *printer;

int do_audio = 1;

// ZX
// ----------- not serialized

int ZX_PLAYER = 0; // 0:app is full featured emulator, 1:app is a small zx player with reduced functionality

int ZX_BROWSER = 1; // game browser version to use, currently only v0 and v1 are supported
int ZX_DEVTOOLS = 0; // 0: regular, 1: development tools (@todo: profiler,analyzer)
int ZX_DEBUG = 0; // status bar, z80 disasm
int ZX_INPUT = 1;

int ZX_ALTROMS = 0; // 0:(no, original), 1:(yes, custom)

int ZX_MULTIFACE = 0; // 0:(no), 1:(yes, any kind)
int ZX_HAL10H8 = 1; // 0:(good), 1:(faulty HAL10H8 chip, as used in 128/+2 models)
int ZX_PRINTUI = 0; // whether print UI yes/no in both screenshots and/or video records
int ZX_ZOOM = 2; // 0..1:x1, 2:x2, 3:x3, 4:x4, >4:x8
int ZX_FULLSCREEN = 0; // 0:no, 1:yes

int ZX_TS;
int ZX_FREQ;

float ZX_FPS, ZX_RPS; // projected framerate on app, also frames the zx can render

// ZX
// ----------- serialized

int ZX_RF = !DEV;
int ZX_CRT = !DEV;
int ZX_BLUR = 50; // 0:off .. 100:max
int ZX_BLOOM = 0; // 0:off .. 100:max
int ZX_GRAIN = 0; // 0:off .. 100:max
int ZX = 128; // 16, 48, 128, 200 (+2), 210 (+2A), 300 (+3)
int ZX_AY = 1; // 0: no, 1: fast, 2: accurate
int ZX_PALETTE = 0; // 0: own, N: others
int ZX_PAUSE = 1; // 0: always on, 1: pause if app is minimized/blurred
int ZX_TURBOROM = 0; // 0: no, 1: patch rom so loading standard tape blocks is faster (see: .tap files)
int ZX_MOUSE = 1; // 0: no, 1:kempston mouse, 2:gunstick/lightgun
int ZX_MOUSE_AUTOFIRE = 0; // 0: no, 1: slow, 2: fast, 3:faster
int ZX_RESUME = 1; // yes/no: whether to resume last game or start fresh on launch
int ZX_AUTOLOAD = 1; // yes/no: reset ZX and load games automatically
int ZX_AUTOPLAY = 0; // yes/no: auto-plays tapes based on #FE port reads
int ZX_AUTOSTOP = 0; // yes/no: auto-stops tapes based on #FE port reads
int ZX_TAPECOUNTER = 1; // yes/no: display a NNN counter while tape loading
int ZX_RUNAHEAD = 0; // 0: no, 1: 1-frame run-ahead, 2: 2-frame run-ahead (improved input latency)
int ZX_ULAPLUS = 2; // 0:classic, 1: ulaplus 64color mode, 2: ulaplus/ultrawide
int ZX_FPSMUL = 100; // fps multiplier: 0 (max), x100 (50 pal), x120 (60 ntsc), x200 (7mhz), x400 (14mhz)
int ZX_AUTOLOCALE = 0; // yes/no: automatically patches games from foreign languages into english
int ZX_FASTCPU = 0; // yes/no: max cpu speed
int ZX_FASTTAPE = 1; // yes/no: max tape speed
int ZX_FLASHLOAD = 1; // yes/no: flashload standard tape blocks
int ZX_LENSLOK = 0; // yes/no: lenslok glass
int ZX_CONSOLE = 0; // yes/no
#define ZX_GUNSTICK (ZX_MOUSE==2)

int ZX_MOUSE_AUTOFIRE_BUTTON = 0; // internal variable. not exposed
int ZX_JOYSTICKS_AUTOFIRE_BUTTON[5] = {0}; // internal variable. not exposed

int ZX_PENTAGON = 0; // DEV; // whether the 128 model emulates the pentagon or not
int ZX_TURBOSOUND = 0; // whether we support TurboSound/TurboAY (6-channels) or not

int ZX_KLMODE = 0; // 0:(K mode in 48, default), 1:(L mode in 48)
int ZX_KLMODE_PATCH_NEEDED = 0; // helper internal variable, must be init to match ZX_KLMODE

int ZX_STEREO = 1; // 0: mono, 1: stereo
int ZX_WAVES = 0; // 0: no, 1: yes
int ZX_HORACE = 0; // 0: no, 1: player1 controlled (keys), 2: player2 controller (gamepad)
int ZX_LOBBY = 0; // 0: no, 1: yes
int ZX_PALETTE_PREVIEW = 0; // 0: no, 1: yes

const char *ZX_FN_STR[] = {"ESC","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12"};
int ZX_FN[12+1] = {'GAME'}; // redefineable function keys. FN[0] = ESC, FN[1..12] = F1..F12

// note that [0]=cursor keys,[1]=gamepad1,[2]=gamepad2...
int ZX_JOYSTICKS[5] = {1|2|16,4,8}; // 0: no, |1: cursor/protek/agf, |2: kempston, |4: sinclair1, |8: sinclair2, |16: fuller, |32:kempston2, >=256:... custom mappings
int ZX_JOYSTICKS_AUTOFIRE[5] = {0}; // 0: no, 1: slow, 2: fast, 3:faster
int ZX_JOYSTICKS_DEADZONE[5][2] = {{15,15},{15,15},{15,15},{15,15},{15,15}}; // [-100..100]% @ X,Y. default 15% positive

// Gamepads:
// Buttons A, X and Y are mapped to the joystick's fire button
// Button B is mapped to the UP directional button. 
// Buttons L1 and R1 are mapped to RETURN and SPACE, respectively.
// @todo: START to bring up on-screen keyboard
// @todo: SELECT button to bring up menus
const char *ZX_PAD_STR[] = {"⭠","⭢","⭡","⭣","\4A\7","\2B\7","\5X\7","\6Y\7","LB","RB","LT","RT","LS","RS","Bk","St"};
int ZX_GAMEPAD[5+1+1][16] = {
    {TK_LEFT,TK_RIGHT,TK_UP,TK_DOWN,TK_TAB},  // redefineable cursor   keys
    {TK_LEFT,TK_RIGHT,TK_UP,TK_DOWN,TK_TAB,TK_UP,TK_TAB,TK_TAB,TK_SPACE,TK_RETURN,0,0,0,0,0,TK_ESCAPE},  // redefineable gamepad1 keys
    {TK_LEFT,TK_RIGHT,TK_UP,TK_DOWN,TK_TAB,TK_UP,TK_TAB,TK_TAB,TK_SPACE,TK_RETURN,0,0,0,0,0,TK_ESCAPE},  // redefineable gamepad2 keys
    {TK_LEFT,TK_RIGHT,TK_UP,TK_DOWN,TK_TAB,TK_UP,TK_TAB,TK_TAB,TK_SPACE,TK_RETURN,0,0,0,0,0,TK_ESCAPE},  // redefineable gamepad3 keys
    {TK_LEFT,TK_RIGHT,TK_UP,TK_DOWN,TK_TAB,TK_UP,TK_TAB,TK_TAB,TK_SPACE,TK_RETURN,0,0,0,0,0,TK_ESCAPE},  // redefineable gamepad4 keys
    {TK_LEFT,TK_RIGHT,TK_UP,TK_DOWN,TK_TAB},  // preset
    {0},                                      // copied/backup values while remapping
};
#define ZX_GAMEPAD_PRESET_ ZX_GAMEPAD[5]
#define ZX_GAMEPAD_COPY_   ZX_GAMEPAD[6]
int ZX_PLAYERNUM = 0; // temp
#define ZX_PAD (ZX_GAMEPAD[ZX_PLAYERNUM])

int   ZX_TABS = 2; // [2]=>'A' current game letter being browsed. may be a letter or special char.
char *ZX_TITLE = 0; // current titlebar
char *ZX_MEDIA = 0; // current mounted game

//const
char *ZX_FOLDER = "./"; // 0;

int   ZX_SHADED = 0; // is ZX_SHADER enabled or not
char *ZX_SHADER = 0; // path to the custom shader file

#define INI_OPTIONS_STR(X) \
    X(ZX_FOLDER) X(ZX_TITLE) X(ZX_MEDIA) X(ZX_SHADER)

#define INI_OPTIONS_NUM(X) \
    X(ZX_GAMEPAD[0][0]) X(ZX_GAMEPAD[0][1]) X(ZX_GAMEPAD[0][2]) X(ZX_GAMEPAD[0][3]) X(ZX_GAMEPAD[0][4]) X(ZX_GAMEPAD[0][5]) X(ZX_GAMEPAD[0][6]) X(ZX_GAMEPAD[0][7]) X(ZX_GAMEPAD[0][8]) X(ZX_GAMEPAD[0][9]) X(ZX_GAMEPAD[0][10]) X(ZX_GAMEPAD[0][11]) X(ZX_GAMEPAD[0][12]) X(ZX_GAMEPAD[0][13]) X(ZX_GAMEPAD[0][14]) X(ZX_GAMEPAD[0][15]) \
    X(ZX_GAMEPAD[1][0]) X(ZX_GAMEPAD[1][1]) X(ZX_GAMEPAD[1][2]) X(ZX_GAMEPAD[1][3]) X(ZX_GAMEPAD[1][4]) X(ZX_GAMEPAD[1][5]) X(ZX_GAMEPAD[1][6]) X(ZX_GAMEPAD[1][7]) X(ZX_GAMEPAD[1][8]) X(ZX_GAMEPAD[1][9]) X(ZX_GAMEPAD[1][10]) X(ZX_GAMEPAD[1][11]) X(ZX_GAMEPAD[1][12]) X(ZX_GAMEPAD[1][13]) X(ZX_GAMEPAD[1][14]) X(ZX_GAMEPAD[1][15]) \
    X(ZX_GAMEPAD[2][0]) X(ZX_GAMEPAD[2][1]) X(ZX_GAMEPAD[2][2]) X(ZX_GAMEPAD[2][3]) X(ZX_GAMEPAD[2][4]) X(ZX_GAMEPAD[2][5]) X(ZX_GAMEPAD[2][6]) X(ZX_GAMEPAD[2][7]) X(ZX_GAMEPAD[2][8]) X(ZX_GAMEPAD[2][9]) X(ZX_GAMEPAD[2][10]) X(ZX_GAMEPAD[2][11]) X(ZX_GAMEPAD[2][12]) X(ZX_GAMEPAD[2][13]) X(ZX_GAMEPAD[2][14]) X(ZX_GAMEPAD[2][15]) \
    X(ZX_GAMEPAD[3][0]) X(ZX_GAMEPAD[3][1]) X(ZX_GAMEPAD[3][2]) X(ZX_GAMEPAD[3][3]) X(ZX_GAMEPAD[3][4]) X(ZX_GAMEPAD[3][5]) X(ZX_GAMEPAD[3][6]) X(ZX_GAMEPAD[3][7]) X(ZX_GAMEPAD[3][8]) X(ZX_GAMEPAD[3][9]) X(ZX_GAMEPAD[3][10]) X(ZX_GAMEPAD[3][11]) X(ZX_GAMEPAD[3][12]) X(ZX_GAMEPAD[3][13]) X(ZX_GAMEPAD[3][14]) X(ZX_GAMEPAD[3][15]) \
    X(ZX_GAMEPAD[4][0]) X(ZX_GAMEPAD[4][1]) X(ZX_GAMEPAD[4][2]) X(ZX_GAMEPAD[4][3]) X(ZX_GAMEPAD[4][4]) X(ZX_GAMEPAD[4][5]) X(ZX_GAMEPAD[4][6]) X(ZX_GAMEPAD[4][7]) X(ZX_GAMEPAD[4][8]) X(ZX_GAMEPAD[4][9]) X(ZX_GAMEPAD[4][10]) X(ZX_GAMEPAD[4][11]) X(ZX_GAMEPAD[4][12]) X(ZX_GAMEPAD[4][13]) X(ZX_GAMEPAD[4][14]) X(ZX_GAMEPAD[4][15]) \
    X(ZX_JOYSTICKS[0]) X(ZX_JOYSTICKS_AUTOFIRE[0]) X(ZX_JOYSTICKS_DEADZONE[0][0]) X(ZX_JOYSTICKS_DEADZONE[0][1]) \
    X(ZX_JOYSTICKS[1]) X(ZX_JOYSTICKS_AUTOFIRE[1]) X(ZX_JOYSTICKS_DEADZONE[1][0]) X(ZX_JOYSTICKS_DEADZONE[1][1]) \
    X(ZX_JOYSTICKS[2]) X(ZX_JOYSTICKS_AUTOFIRE[2]) X(ZX_JOYSTICKS_DEADZONE[2][0]) X(ZX_JOYSTICKS_DEADZONE[2][1]) \
    X(ZX_JOYSTICKS[3]) X(ZX_JOYSTICKS_AUTOFIRE[3]) X(ZX_JOYSTICKS_DEADZONE[3][0]) X(ZX_JOYSTICKS_DEADZONE[3][1]) \
    X(ZX_JOYSTICKS[4]) X(ZX_JOYSTICKS_AUTOFIRE[4]) X(ZX_JOYSTICKS_DEADZONE[4][0]) X(ZX_JOYSTICKS_DEADZONE[4][1]) \
    X(ZX_FN[0]) X(ZX_FN[1]) X(ZX_FN[2]) X(ZX_FN[3]) X(ZX_FN[4]) X(ZX_FN[5]) \
    X(ZX_FN[6]) X(ZX_FN[7]) X(ZX_FN[8]) X(ZX_FN[9]) X(ZX_FN[10]) X(ZX_FN[11]) X(ZX_FN[12]) \
    X(ZX) \
    X(ZX_RF) \
    X(ZX_CRT) \
    X(ZX_AY) \
    X(ZX_TURBOROM) \
    X(ZX_RUNAHEAD) \
    X(ZX_MOUSE) \
    X(ZX_MOUSE_AUTOFIRE) \
    X(ZX_ULAPLUS) \
    X(ZX_FPSMUL) \
    X(ZX_AUTOLOCALE) \
    X(ZX_FASTTAPE) \
    X(ZX_BROWSER) \
    X(ZX_ALTROMS) \
    X(ZX_PALETTE) X(ZX_PALETTE_PREVIEW) \
    X(ZX_MULTIFACE) \
    X(ZX_ZOOM) X(ZX_FULLSCREEN) X(ZX_WAVES) X(ZX_LENSLOK) X(ZX_SHADED) X(ZX_LOBBY) X(ZX_HORACE) \
    X(ZX_BLUR) X(ZX_BLOOM) X(ZX_TABS) X(ZX_STEREO) X(ZX_FLASHLOAD) X(ZX_PAUSE) X(ZX_CONSOLE) X(ZX_TURBOSOUND) \
    X(ZX_RESUME) X(ZX_AUTOLOAD) X(ZX_TAPECOUNTER) X(ZX_GRAIN)

void logport(word port, byte value, int is_out);
void outport(word port, byte value);
byte inport(word port);

void port_0x00fe(byte value);
void port_0x1ffd(byte value);
void port_0x7ffd(byte value);
void port_0xbffd(byte value);
void port_0xfffd(byte value);

enum { KEEP_MEDIA = 1, QUICK_RESET = 2 };
void boot(int model, unsigned flags);
void reset(unsigned flags);
void eject(int forget_media);

//void run(unsigned TS, int is_paper);

void frame_new();
void frame(int drawmode, int do_sim); // no render (<0), whole frame (0), scanlines (1)

void* quicksave(unsigned slot);
void* quickload(unsigned slot);

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

    #define GET_MAPPED_ROMBANK() ((page128 & 16 ? 1 : 0) | ((page2a & 5) == 4 ? 2 : 0)) // 0, 0/1 or 0/1/2/3
    #define GET_BASIC_ROMBANK() ((ZX>=128) + 2*(ZX>=210)) // 0 (16/48), 1 (128/+2) or 3 (+2A/+3)
    #define GET_EDITOR_ROMBANK() (!(ZX<=200)) // 0 (128/+2) or 1 (+2A/+3)
    #define GET_3DOS_ROMBANK() (2 * (ZX >= 210)) // 2 (+2A/+3)
    #define ROM_BASIC()  ROM_BANK(GET_BASIC_ROMBANK())
    #define ROM_EDITOR() ROM_BANK(GET_EDITOR_ROMBANK())
    #define ROM_3DOS()   ROM_BANK(GET_3DOS_ROMBANK())

    byte *floating_bus;

    byte *mem; // 48k
    byte *rom;
    byte *dum;

    // 16/48/128/+2: pages 1,3,5,7 are contended (1), 0,2,4,6 not contended (0) -> so mask is 0001 (1)
    // +2A/+3:       pages 4,5,6,7 are contended (1), 0,1,2,3 not contended (0) -> so mask is 0100 (4)
    //#define IS_CONTENDED_PAGE(addr) (!!(((addr - mem) >> 14) & (ZX >= 210 ? 4 : 1)))

    #define ADDR8(a)     ((byte *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define READ8(a)     (*(byte *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define READ16(a)    (*(word *)&MEMr[(a)>>14][(a)&0x3FFF])
    #define WRITE8(a,v)  (*(byte *)&MEMw[(a)>>14][(a)&0x3FFF]=(v))
    #define WRITE16(a,v) (*(word *)&MEMw[(a)>>14][(a)&0x3FFF]=(v))

// ula
int  ZXFlashFlag;
rgba ZXPalette[64];
byte ZXBorderColor;

// 128
byte page128;

// plus3/2a
byte page2a;

// betadisk
byte beta128;

// keyboard
int issue2;
int keymap[5][5];
int nextkey;

// joysticks
byte fuller,kempston,kempston2;

// mouse
byte kempston_mouse;

// beeper
#define TAPE_VOLUME 0.25f // relative to buzz
#define BUZZ_VOLUME 0.25f // relative to ay
beeper_t buzz;
byte ear;
byte spk;

// ay
ay38910_t ay[2];
struct ayumi ayumi[2];
byte ay_current_reg[2];
int/*byte*/ ay_registers[2][ 16 ];
int turbosound;
void ay_reset() {
    memset(ay_registers,0,sizeof(ay_registers));
    memset(ay_current_reg,0,sizeof(ay_current_reg));
    turbosound = 0;
}

// vsync
byte zx_int;

// ticks
uint64_t ticks, TS;

// tape
int tape_hz; // for vis
uint64_t tape_ticks;

// autoplay vars
int autoplay;
int autostop;
int autoplay_freq;
uint64_t autoplay_last;
unsigned autoplay_numreads;

// wd1793
WD1793 wd;
FDIDisk fdd[NUM_FDI_DRIVES];

// medias
uint64_t media_seek[16];

enum { ALL_FILES = 0, GAMES_AND_ZIPS = 5*4, GAMES_ONLY = 8*4, TAPES_AND_DISKS_ONLY = 13*4, DISKS_ONLY = 17*4 };
int file_is_supported(const char *filename, int skip) {
    const char *exts = ".fx .pal.pok.scr.ay .gz .zip.rar.rom.sna.z80.rzx.szx.tap.tzx.pzx.csw.dsk.img.mgt.trd.fdi.scl.$b.$c.$d.";
    const char *ext = strrchr(filename ? filename : "", '.');
    if( !ext ) return 0;
    char ext1[8]; snprintf(ext1, countof(ext1), "%s.", ext);
    char ext2[8]; snprintf(ext2, countof(ext2), "%s ", ext);
    return strstri(skip+exts, ext1) || strstri(skip+exts, ext2);
}

#include "zx_ay.h"
#include "zx_dis.h"
#include "zx_txt.h"
#include "zx_rom.h"
#include "zx_dsk.h"
#include "zx_tap.h" // requires page128
#include "zx_tzx.h"
#include "zx_sna.h" // requires page128, ZXBorderColor
#include "zx_db.h"
#include "zx_rzx.h"
#include "zx_pal.h"
#include "zx_ula.h"
#include "zx_lenslok.h"



static byte* merge;
static size_t merge_len;
void glue_init() {
    free(merge); merge = 0;
    merge_len = 0;
}
void *glue(const byte *data, size_t len) {
    merge = realloc(merge, merge_len + len);
    memcpy(merge + merge_len, data, len);
    merge_len += len;
    return merge;
}
size_t glue_len() {
    return merge_len;
}
void *zip_read(const char *filename, size_t *size) {
    glue_init();

    // up to 64 tapes per zip (usually up to 4 max)
    char *alpha_tapes[64] = {0};
    int count_tapes = 0;

    // quick rar test
    rar *r = rar_open(filename, "rb");
    if( r ) {
        for( unsigned i = 0 ; count_tapes < 64 && i < rar_count(r); ++i ) {
            if( !file_is_supported(rar_name(r,i), ALL_FILES) ) continue;

            printf("  %d) ", i+1);
            printf("[%08X] ", rar_hash(r,i));
            printf("$%02X ", rar_codec(r,i));
            printf("%s ", rar_modt(r,i));
            printf("%5s ", rar_file(r,i) ? "" : "<dir>");
            printf("%11u ", rar_size(r,i));
            printf("%s ", rar_name(r,i));
            void *data = rar_extract(r,i);
            printf("\r%c\n", data ? 'Y':'N'); // %.*s\n", rar_size(r,i), (char*)data);

            // append data to merge
            glue(data, rar_size(r,i));
            if(size) *size = merge_len;
            rar_free(r,data);

            // keeping glueing tapes
            if(strstr(rar_name(r,i),".tap") || strstr(rar_name(r,i),".tzx") || strstr(rar_name(r,i),".pzx")) {
            continue;
            }
            break;
        }

        rar_close(r);
        return merge;
    }


    // test contents of file
    zip *z = zip_open(filename, "rb");
    if( z ) {
        for( unsigned i = 0 ; count_tapes < 64 && i < zip_count(z); ++i ) {
            void *data = file_is_supported(zip_name(z,i),ALL_FILES) ? zip_extract(z,i) : 0;
            if(!data) continue;
            free(data);

            alpha_tapes[count_tapes++] = zip_name(z,i);
        }

        if( count_tapes )
        qsort(alpha_tapes, count_tapes, sizeof(char *), qsort_compare);

        for( unsigned j = 0 ; j < count_tapes; ++j ) {
            int i = zip_find(z, alpha_tapes[j]); // convert entry to index. returns <0 if not found.

            printf("  %d) ", i+1);
            printf("[%08X] ", zip_hash(z,i));
            printf("$%02X ", zip_codec(z,i));
            printf("%s ", zip_modt(z,i));
            printf("%5s ", zip_file(z,i) ? "" : "<dir>");
            printf("%11u ", zip_size(z,i));
            printf("%s ", zip_name(z,i));
            void *data = zip_extract(z,i);
            printf("\r%c\n", data ? 'Y':'N'); // %.*s\n", zip_size(z,i), (char*)data);

            // append data to merge
            glue(data, zip_size(z,i));
            if(size) *size = merge_len;
            free(data);

            // keeping glueing tapes
            if(strstr(zip_name(z,i),".tap") || strstr(zip_name(z,i),".tzx") || strstr(zip_name(z,i),".pzx")) {
            continue;
            }
            break;
        }

        zip_close(z);
        return merge;
    } else {
        printf("no zip/rar contents in %s archive\n", filename);
    }
    return 0;
}



enum SpecKeys {
    ZX_0,ZX_1,ZX_2,ZX_3,ZX_4,ZX_5,  ZX_6,ZX_7,ZX_8,ZX_9,ZX_A,ZX_B,  ZX_C,ZX_D,ZX_E,ZX_F,ZX_G,ZX_H,
    ZX_I,ZX_J,ZX_K,ZX_L,ZX_M,ZX_N,  ZX_O,ZX_P,ZX_Q,ZX_R,ZX_S,ZX_T,  ZX_U,ZX_V,ZX_W,ZX_X,ZX_Y,ZX_Z,
    ZX_SPACE,ZX_ENTER,ZX_SHIFT,ZX_SYMB
};
#define ZXKey(a) ( keymap[ keytbl[a][0] ][ keytbl[a][1] ] &= keytbl[a][2] )
#define ZXKeyboardClear() \
    keymap[1][1] = keymap[1][2] = \
    keymap[2][1] = keymap[2][2] = \
    keymap[3][1] = keymap[3][2] = \
    keymap[4][1] = keymap[4][2] = 0xFF;

const unsigned char keytbl[256][3] = {
    {1, 2, 0xFE}, {1, 1, 0xFE}, {1, 1, 0xFD}, {1, 1, 0xFB}, /* 0|1|2|3 */
    {1, 1, 0xF7}, {1, 1, 0xEF}, {1, 2, 0xEF}, {1, 2, 0xF7}, /* 4|5|6|7 */
    {1, 2, 0xFB}, {1, 2, 0xFD}, {3, 1, 0xFE}, {4, 2, 0xEF}, /* 8|9|a|b */
    {4, 1, 0xF7}, {3, 1, 0xFB}, {2, 1, 0xFB}, {3, 1, 0xF7}, /* c|d|e|f */
    {3, 1, 0xEF}, {3, 2, 0xEF}, {2, 2, 0xFB}, {3, 2, 0xF7}, /* g|h|i|j */
    {3, 2, 0xFB}, {3, 2, 0xFD}, {4, 2, 0xFB}, {4, 2, 0xF7}, /* k|l|m|n */
    {2, 2, 0xFD}, {2, 2, 0xFE}, {2, 1, 0xFE}, {2, 1, 0xF7}, /* o|p|q|r */
    {3, 1, 0xFD}, {2, 1, 0xEF}, {2, 2, 0xF7}, {4, 1, 0xEF}, /* s|t|u|v */
    {2, 1, 0xFD}, {4, 1, 0xFB}, {2, 2, 0xEF}, {4, 1, 0xFD}, /* w|x|y|z */
    {4, 2, 0xFE}, {3, 2, 0xFE}, {4, 1, 0xFE}, {4, 2, 0xFD}, /* SPC|ENT|SHF|SYM */
};

// 16/48
void port_0x00fe(byte value) {
    // border color
    ZXBorderColor = (value & 0x07);

    // speaker
    spk = value;
}

// zx128
void port_0x7ffd(byte value) {
    if(ZX >= 128)
    if(!(page128 & 32)) { //if bit5 not locked

        page128=value;

        // special paging mode has higher priority in +2A/+3 models (see: port_0x1ffd)
        if( ZX >= 210 && (page2a&1) ) return;

        // check bit 2-0: RAM0/7 -> 0xc000-0xffff
        MEMw[3]=MEMr[3]=RAM_BANK(value&7);

        //128:    check bit 4 : rom selection (ROM0/1 -> 0x0000-0x3FFF)
        //+2A/+3: check high bit of rom selection too (same as offset ROM selection to 0x8000) 1x0 101
        MEMr[0]=ROM_BANK((value & 16 ? 1 : 0) | ((page2a & 5) == 4 ? 2 : 0));
    }
}

void ZXJoystick(int *j, int up, int down, int left, int right, int fire) {
    if(left)  ZXKey(j[0]);
    if(right) ZXKey(j[1]);
    if(up)    ZXKey(j[2]);
    if(down)  ZXKey(j[3]);
    if(fire)  ZXKey(j[4]);
}

void ZXJoysticksClear() {
    kempston = 0;
    kempston2 = 0;
    fuller = 255;
}

void ZXJoysticks(int PLAYERNUM, int up, int down, int left, int right, int fire) {
    // 0:off,1:cursor,2:kempston,4:sinclair1,8:sinclair2,16:fuller,32:kempstonB,64:kempstonC
    int joy = ZX_JOYSTICKS[PLAYERNUM];

    // press SHIFT with cursor keys only under BASIC. ie, do not press SHIFT during games.
    // reasoning: conflicts with many games (Gauntlet, Emlyn Hughes, etc) that use SHIFT key actively
    bool basic = PC(cpu) < 0x4000; // && (GET_MAPPED_ROMBANK() == GET_BASIC_ROMBANK() || GET_MAPPED_ROMBANK() == GET_EDITOR_ROMBANK());

    // emulate cursor keys always while in basic, unless conflicting sinclairs are mapped. this is a UX decision
    if( basic ) if( !(joy & (4|8)) ) joy |= 1;

    // joystick as keyboard mappings
    int mappings[][6] = {
        // custom
        { ZX_O,ZX_P,ZX_Q,ZX_A,ZX_M,ZX_SPACE }, // OPQAM/SP
        { ZX_O,ZX_P,ZX_1,ZX_Q,ZX_M,ZX_SPACE }, // OP1QM/SP
        { ZX_K,ZX_L,ZX_A,ZX_Z,ZX_M,ZX_SPACE }, // KLAZM/SP
        { ZX_Z,ZX_X,ZX_P,ZX_L,ZX_0,ZX_SPACE }, // ZXPL0/SP
        { ZX_B,ZX_N,ZX_Q,ZX_A,ZX_M,ZX_SPACE }, // BNQAM/SP
        { ZX_I,ZX_P,ZX_Q,ZX_Z,ZX_M,ZX_SPACE }, // IPQZM/SP
        { ZX_I,ZX_P,ZX_1,ZX_Z,ZX_M,ZX_SPACE }, // IP1ZM/SP
        { ZX_6,ZX_7,ZX_0,ZX_O,ZX_M,ZX_SPACE }, // 670OM/SP
        { ZX_Q,ZX_W,ZX_O,ZX_K,ZX_C,ZX_SPACE }, // QWOKC/SP
        // common
        { ZX_1,ZX_2,ZX_4,ZX_3,ZX_5,ZX_5 }, // sinclair2
        { ZX_6,ZX_7,ZX_9,ZX_8,ZX_0,ZX_0 }, // sinclair1
        { ZX_5,ZX_8,ZX_7,ZX_6,ZX_0,ZX_0 }, // cursor
    }, *user = mappings[joy>>8];

    if(joy> 255) ZXJoystick(user, up, down, left, right, fire);
    if(joy &  8) ZXJoystick(mappings[countof(mappings)-3], up, down, left, right, fire);
    if(joy &  4) ZXJoystick(mappings[countof(mappings)-2], up, down, left, right, fire);
    if(joy &  1) ZXJoystick(mappings[countof(mappings)-1], up, down, left, right, fire), basic && (up|down|left|right) && ZXKey(ZX_SHIFT);

    if(joy &  2) kempston = (fire<<4)|(up<<3)|(down<<2)|(left<<1)|right;
    if(joy & 32) kempston2 = (fire<<4)|(up<<3)|(down<<2)|(left<<1)|right;
    if(joy & 16) fuller = 255 ^ ((fire<<7)|(right<<3)|(left<<2)|(down<<1)|up);
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


void config(int ZX) {
    // note about issue2/3:
    // i feel there are more issue3-only games (like GoldMine(1983)) than issue2-only
    // (like Rasputin48K, AbuSimbelProfanation(Gremlin), Spynads). cannot confirm this
    // claim for now, but we'll set 16K as issue2 by default, and anything else above as issue3.
        
    if(ZX >= 16) {
        MEMr[3]=DUMMY_BANK(0);
        MEMr[2]=DUMMY_BANK(0);
        MEMr[1]=RAM_BANK(5);
        MEMr[0]=ROM_BANK(0);

        MEMw[3]=RAM_BANK(0);
        MEMw[2]=RAM_BANK(2);
        MEMw[1]=RAM_BANK(5);
        MEMw[0]=DUMMY_BANK(0);

        issue2=1; // can be either 0 or 1
        nextkey=0;
        page128=32; // -1
        page2a=128; // -1

        ZX_TS = 69888;
        ZX_FREQ = 3500000;
    }

    if(ZX >= 48) {
        issue2=0; // can be =1 as well
        MEMr[3]=RAM_BANK(0);
        MEMr[2]=RAM_BANK(2);
    }

    if( ZX >= 128) {
        issue2=0; // cant be 1
        page128=0;
        port_0x7ffd(page128);

        ZX_TS = 70908;
        ZX_FREQ = 3546894;
    }

    if( ZX_PENTAGON ) { // ZX == 128
        ZX_TS = 71680;
        ZX_FREQ = 3500000; //< this is the correct one!
        //ZX_FREQ = 3546894; //< better quality?
    }

    if( ZX >= 210 ) { // +2A/+3
        page2a=0;
        port_0x1ffd(page2a);
    }

    floating_bus = NULL;
}



// ay
void port_0xfffd(byte value) {
    if( value >= 254 ) if( ZX_TURBOSOUND ) { turbosound = (255-value); return; }

    byte *v = &ay_current_reg[turbosound];
    int *r = ay_registers[turbosound];

    // select ay register
    *v=(value&15);
    // floooh's
    ay38910_iorq(&ay[turbosound], AY38910_BDIR|AY38910_BC1|((*v)<<16));
}
void port_0xbffd(byte value) {
    byte *v = &ay_current_reg[turbosound];
    int *r = ay_registers[turbosound];

    // update ay register
    r[*v]=value;
    // floooh's
    ay38910_iorq(&ay[turbosound], AY38910_BDIR|value<<16);
    // ayumi
    switch (*v)
    {
    case 0:
    case 1:
        ayumi_set_tone(&ayumi[turbosound], 0, (r[1] << 8) | r[0]);
        break;
    case 2:
    case 3:
        ayumi_set_tone(&ayumi[turbosound], 1, (r[3] << 8) | r[2]);
        break;
    case 4:
    case 5:
        ayumi_set_tone(&ayumi[turbosound], 2, (r[5] << 8) | r[4]);
        break;
    case 6:
        ayumi_set_noise(&ayumi[turbosound], r[6]);
        break;
    case 8:
        ayumi_set_mixer(&ayumi[turbosound], 0, r[7] & 1, (r[7] >> 3) & 1, r[8] >> 4);
        ayumi_set_volume(&ayumi[turbosound], 0, r[8] & 0xf);
        break;
    case 9:
        ayumi_set_mixer(&ayumi[turbosound], 1, (r[7] >> 1) & 1, (r[7] >> 4) & 1, r[9] >> 4);
        ayumi_set_volume(&ayumi[turbosound], 1, r[9] & 0xf);
        break;
    case 10:
        ayumi_set_mixer(&ayumi[turbosound], 2, (r[7] >> 2) & 1, (r[7] >> 5) & 1, r[10] >> 4);
        ayumi_set_volume(&ayumi[turbosound], 2, r[10] & 0xf);
        break;
    case 7:
        ayumi_set_mixer(&ayumi[turbosound], 0, r[7] & 1, (r[7] >> 3) & 1, r[8] >> 4);
        ayumi_set_mixer(&ayumi[turbosound], 1, (r[7] >> 1) & 1, (r[7] >> 4) & 1, r[9] >> 4);
        ayumi_set_mixer(&ayumi[turbosound], 2, (r[7] >> 2) & 1, (r[7] >> 5) & 1, r[10] >> 4);
        break;
    case 11:
    case 12:
        ayumi_set_envelope(&ayumi[turbosound], (r[12] << 8) | r[11]);
        break;
    case 13:
        if (r[13] != 255) //< 255 to continue current envelope
        ayumi_set_envelope_shape(&ayumi[turbosound], r[13]);
        break;
    }
}
byte inport_0xfffd(void) {
    byte *v = &ay_current_reg[turbosound];
    int *r = ay_registers[turbosound];

//  return ay38910_iorq(&ay, 0);

    if( ZX_GUNSTICK ) // magnum lightgun
    if( *v == 14 ) {

        // Magnum Light Phaser / Defender Light Gun for Spectrum >= 128
        // 11X0 1111 bit5: trigger button (0=Pressed, 1=Released)
        // 110X 1111 bit4: light sensor   (0=None, 1=Light)

        int lightgun();
        byte val = mouse().lb || ZX_MOUSE_AUTOFIRE_BUTTON ? 0xCF : 0xEF; // set button
        val |= 0x10 * lightgun(); // set light

        r[14] = val;

        //printf("\t\t\tmagnum (%02x vs %02x)\n", gunstick(0xFF), val);
        return val;
    }

    /* The AY I/O ports return input directly from the port when in
    input mode; but in output mode, they return an AND between the
    register value and the port input. So, allow for this when
    reading R14... */

    if( *v == 14 )
    return (r[7] & 0x40 ? 0xbf & r[14] : 0xbf);

    /* R15 is simpler to do, as the 8912 lacks the second I/O port, and
    the input-mode input is always 0xff */

    if( *v == 15 && !( r[7] & 0x80 ) ) return 0xff;

    /* Otherwise return register value, appropriately masked */
    unsigned char ay_registers_mask[ 16 ] = {
        0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x1f, 0xff,
        0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x0f, 0xff, 0xff,
    };
    return r[*v] & ay_registers_mask[*v];
}

uint64_t transact(uint64_t pins) {

#if DEV
    if( cpu.step == 0 ) {
        unsigned pc = Z80_GET_ADDR(pins);

#if PRINTER
        // traps rom print routine. useful for automation tests

        // check no int pending, +2a/+3, or rom1
        // if( !IFF1(cpu) && ZX <= 200 && !(page128 & 16) )

        if (pc == 0x09F4 /*&& SP(cpu) < 0x4000*/) {
            int is_channel_k = READ8( IY(cpu) + 0x30 ) & 16; // FLAGS2
            int is_lower_screen = READ8( IY(cpu) + 2 ) & 1; // TV_FLAG
            if (!is_channel_k /*&& !is_lower_screen*/ ) {
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

        // apply hot patch. K/L input mode
        //if( pc == 0x15E1 && ZX_KLMODE_PATCH_NEEDED ) rom_patch_klmode(pc);

        // disasm
        int do_disasm = pc >= 0x38 && pc < 0x45; // mouse().rb;
        if( 0 )
        if( do_disasm ) puts(dis(pc, 1));
    }
#endif

    // mem
    if( pins & Z80_MREQ ) {
        if( pins & Z80_RD ) {
            uint16_t addr = Z80_GET_ADDR(pins);

            // Install shadow TRDOS ROM when the PC is in ROM1 + [0x3D00..0x3DFF] range
            // Uninstall it as soon as PC is out of ROM space
            if( ZX_PENTAGON/*beta128*/ ) {
                unsigned pc = PC(cpu);
                if( ((pc & 0xFF00) == 0x3D00) && (MEMr[0] == (rom + 0x4000 * 1)) ) {
                    MEMr[0] = rom + 0x4000 * 2; // install shadow rom
                }
                else
                if( pc >= 0x4000 && (MEMr[0] == (rom + 0x4000 * 2)) ) {
                    MEMr[0] = rom + 0x4000 * (page128 & 16 ? 1 : 0); // * 1; restore rom1
                }
            }
            // @todo: expensive, but may be good place for MF1/MF3 shadow rom
            // @todo: expensive, but may be good place for 0x15E1 K/L mode hook
            // @todo: expensive, but may be good place for .tap traps

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

    return pins;
}


void frame_new() {
    // logic that ticks every new frame
    // this section has x50 faster rate than next section.
    if( 1 ) {
        // vsync (once per frame)
        // zx_int = 1;
    }
    if( tape_inserted() ) {
        // auto-plays tape
        if( autoplay && !tape_playing() ) {
            if( ZX_AUTOPLAY ) {
                printf("auto-play tape (%u Hz, %u reads)\n", tape_hz, autoplay_numreads),
                tape_play(1);
            }
            else
            if( PC(cpu) < 0x4000 && voc_pos == 0 ) { // if( (PC(cpu) & 0xff00) == 0x0500 && GET_MAPPED_ROMBANK() == GET_BASIC_ROMBANK() ) {
                // kick off the initial tape playing automatically.
                // user will need to pause/resume tape manually (via F2 key) from this point, as AUTOPLAY=off
                // if( 1 ) { // tape_inserted() && !tape_playing() ) {
                if( !tape_playing() ) {
                    puts("auto-play tape (rom)"),
                    tape_play(1);
                }
            }
        }

        autoplay = 0;
        autostop = 0;
    }
    // logic that ticks once per second
    static int frame = 0; ++frame;
    if( !(frame % 50) ) {
        // auto-stops tape. stopping a tape is a risky action and load will fail if not done at the right moment.
        // so, the thinking for this block has a x50 slower rate than playing a tape. we have to be sure we dont
        // want tape to be playing instead.
        if( autostop && tape_playing() ) {
            if( ZX_AUTOSTOP ) {
                printf("auto-stop tape (%u Hz, %u reads)\n", tape_hz, autoplay_numreads),
                tape_stop();
            }
        }

        autoplay = 0;
        autostop = 0;
    }
}

void sys_audio() {
    // tick the beeper (full frequency)
    bool sample_ready = beeper_tick(&buzz);
    // queue next sample
    if( sample_ready ) {
#if 1
    // mic/ear speaker
    // ref: https://piters.tripod.com/cassport.htm
    // ref: https://retrocomputing.stackexchange.com/a/27539
    // The threshold voltage for reading the input state (at the pin) of the ULA is 0.7V (*)
    // BIT4 EAR     BIT3 MIC     PIN 28 VOLTAGE  BIT6 IN READ
    //        0            0               0.34             0
    //        0            1               0.66             0 (borderline (*))
    //        1            0               3.56             1
    //        1            1               3.70             1
    //

    // test: ensure that: SAVE "P" : BEEP 1,1 : LOAD "" shall output sound
    // test: Cobra's Arc has menu music and in-game speech
    // test: Parapshock has menu music

    const float beeper_volumes[] = {
        TAPE_VOLUME, // rest volume
        TAPE_VOLUME, // tape volume (??%) see diagram above (*)
        1, // 0.96f, // beeper volume (96%)
        2, // 1.00f  // tape+beeper volume (100%)
    };

    int save_audio = spk & 0x8;
    int load_audio = ear;
    int tape = !!load_audio ^ !!save_audio;

    int beeper = !!(spk & 0x10);
    int combined = !!beeper * 2 + !!tape;

    beeper_set_volume(&buzz, beeper_volumes[combined]);
    beeper_set(&buzz, beeper | tape );
#endif
    }

    // tick the AY (half frequency)
    static float output[1+3+3] = {0};
    static float ay_sample = 0;
    if( ZX >= 128 ) {
        static float ay_sample1 = 0, ay_sample2 = 0; enum { ayumi_fast = 0 };
        static byte even = 255; ++even;

        if( ZX_AY == 0 ) ay_sample = ay_sample1 = ay_sample2 = 0; // no ay

        if( ZX_AY == 1 ) if( even & 1 ) {  // half frequency. every 128:256
            ay38910_tick(&ay[0], output+1), ay_sample1 = ay_sample2 = ay[0].sample;

            if( ZX_TURBOSOUND )
            ay38910_tick(&ay[1], output+4), ay_sample2 = ay[1].sample;

            ay_sample = (ay_sample1 + ay_sample2) * 0.5f; // both
        }

        if( ZX_AY == 2 ) if( !(even & (0x3F>>2)) ) { // every 16:256 calls
            ay_sample1 = ay_sample2 = ayumi_render(&ayumi[0], ayumi_fast, 1, output+1) * 2;

            if( ZX_TURBOSOUND )
            ay_sample2 = ayumi_render(&ayumi[1], ayumi_fast, 1, output+4) * 2;

            ay_sample = (ay_sample1 + ay_sample2) * 0.5f; // both
        }
    }

    if( do_audio && sample_ready ) {
#if 0
        static float buzz_prev = 0;
        if( ZX_AY == 2 ) buzz.sample = 1.1 * low_pass_filter_10khz(buzz.sample, &buzz_prev, AUDIO_FREQUENCY);
#endif
        output[0] = buzz.sample; // for 48K vis

        float master = 0.98f * !!ZX_AY; // @todo: expose ZX_AY_VOLUME / ZX_BEEPER_VOLUME instead
        float sample = (buzz.sample * 0.75f + ay_sample * 0.25f) * master;
        //float digital = mix(0.5); // 70908/44100. 69888/44100. // ZX_TS/s); 22050 11025 44100
        //sample += digital * 1; // increase volume

        audio_queue(sample, output, 1+(ZX&1));
    }
}


void logport(word port, byte value, int is_out) {
    return;

    // [ref] https://worldofspectrum.org/faq/reference/ports.htm
    // sorted by xxNN byte, then NNxx byte
    if( 0
        || port == 0xfefe // keyboard
        || port == 0xfdfe // keyboard
        || port == 0xfbfe // keyboard
        || port == 0xf7fe // keyboard, sinclair2
        || port == 0xeffe // keyboard, sinclair1
        || port == 0xdffe // keyboard
        || port == 0xbffe // keyboard
        || port == 0x7ffe // keyboard

        || port == 0x00fe // ear/mic border/beeper

        || port == 0xfffd // ay
        || port == 0xbffd // ay
        || port == 0x7ffd // mem128
        || port == 0x3ffd // fdc
        || port == 0x2ffd // fdc
        || port == 0x1ffd // mem+2a/fdcmotor
        || port == 0x0ffd // parallel

        || port == 0xffdf // kempston mouse
        || port == 0xfbdf // kempston mouse
        || port == 0xfadf // kempston mouse

        || port == 0x00df // kempston joystick (31)
        || port == 0x00c7 // kempston joystick (55)

        ||(port == 0x007f && !ZX_PENTAGON/*!beta128*/ ) // fuller joystick
        ||(port == 0x005f && !ZX_PENTAGON/*!beta128*/ ) // fuller ay
        ||(port == 0x003f && !ZX_PENTAGON/*!beta128*/ ) // fuller ay

        || port == 0xff3b // ulaplus
        || port == 0xbf3b // ulaplus
    )
        return;

    if( (port & 0xff) == 0xfe ) return; // ula

    unsigned pc = PC(cpu); // Z80_GET_ADDR(pins);
    if( is_out )
    printf("%04x OUT %4X, %X\n", pc, port, value);
    else
    printf("%04x IN  %4X (%X)\n", pc, port, value);
}


void outport(word port, byte value) {
#if NDEBUG <= 0
    logport(port, value, 1);
#endif

    // XXXXXXXX ---1 1111 when trdos is enabled. ports: 1F,3F,5F,7F,FF
    if( (port & 3) == 3 ) { // (port & 0xff1f) == 0x1f) { // 
        int trdos = ZX_PENTAGON/*beta128*/ && MEMr[0] == (rom + 0x4000 * 2);
        if( trdos ) {
            // command(0)/track(1)/sector(2)/data(3)/system(4) WD write registers

            int index;
#if 0
            // convert 1F,3F,5F,7F,FF ports into 0,1,2,3,4 index
            index = ((port & 0x80 ? 0xA0-1 : port) >> 5) & 7;
#else
            if(port & 0x80) index = WD1793_SYSTEM;
            else if( ( port & 0x60 ) == 0x60 ) index = WD1793_DATA;
            else if( ( port & 0x60 ) == 0x40 ) index = WD1793_SECTOR;
            else if( ( port & 0x60 ) == 0x20 ) index = WD1793_TRACK;
            else if( ( port & 0x60 ) == 0x00 ) index = WD1793_COMMAND;
#endif

            if( !index ) {
                int drive_state = ((value >> 5) & 7) < 4; // on, off
            }
            // Commands Register for index #0 (0x1F)
            // 0 0 0 0  h v x x  Init 0X
            // 0 0 0 1  h v x x  Seek 1X
            // 0 0 1 i  h v x x  Step 2X 3X
            // 0 1 0 i  h v x x  Step forward 4X 5X
            // 0 1 1 i  h v x x  Step back 6X 7X
            // 1 0 0 m  s e c 0  Read sectors 8X
            // 1 0 1 m  s e c a  Write sectors AX BX
            // 1 1 0 0  0 e 0 0  Read address CX
            // 1 1 0 1 J3J2J1J0  Forced Interrupt DX
            // 1 1 1 0  0 e 0 0  Read track EX
            // 1 1 1 1  0 e 0 0  Format track FX
            Write1793(&wd, index, value);
            return;
        }
    }


    // +2a/+3 (fixes fairlight games)
    if( ZX >= 210 )
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
    if( ZX >= 128 )
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
    if( ZX >= 128 )
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
int lightgun() { // true if sensor is lit
    int lit = 0;

    struct mouse m = mouse();
    int gunx = m.x - (_32 - mouse_offsets()[0]); // [0..255]
    int guny = m.y - (_24 - mouse_offsets()[1]); // [0..191]
    if( gunx < 0 || gunx >= 256 ) return 0;
    if( guny < 0 || guny >= 192 ) return 0;

#if 0
    int ts = 14336 + (gunx/32) * 4 + guny * 228;
    if( abs((ticks % 70908) - ts) > 32 ) return 0;
#else
    int ts = (ZX < 128 ? 14336 : 14364) + (gunx/2) + guny * (ZX < 128 ? 224 : 228);
    if( abs((int)(ticks % (ZX < 128 ? 69888 : 70908)) - ts) > 228*16 ) return 0;
#endif

    // light
    // check whether attribute is white
    int x = gunx / 8;
    int y = guny / 8;

    int cell = x;
    int line = guny;
    int offset = ((line & 7) << 8) + ((line & 0x38) << 2) + ((line & 0xC0) << 5) + cell;
    byte *pixel = VRAM + offset;
    byte *attr = VRAM + 6144 + 32 * y + x;
    int ink = *attr & 7;
    int paper = (*attr >> 3) & 7;
    int bright = *attr & 0x40;

    // find bright luma pixels (green 4, cyan 5, yellow 6, white 7)
    // if(ink >= 4 || paper >= 4 ) lit = 1;
    // find white attribs (gunstick)
    // if (*pixel ? ink >= 0x04 : paper >= 0x07) lit = 1;
    // pixel perfect
    // extern Tigr *canvas;
    // if( (canvas->pix[(gunx+_32) + (guny+_24) * _256].rgba & RGB(255,255,255)) > RGB(128,128,128)) lit = 1;
    // debug pixel perfect
    // canvas->pix[(gunx+_32) + (guny+_24) * _256].rgba = RGB(255,0,0);

    // lit = *pixel ? ink >= 4 : paper >= 6;
    lit = ink >= 4 || paper >= 6;

    #if DEV // debug
    if( ZX_DEVTOOLS ) {
        printf("lit:%d gun[%d,%d] attr[%d,%d]=%02x\n", lit, gunx,guny, x,y,(byte)*attr);
        *pixel = 0xFF, *attr = 0x38;
    }
    #endif

    return lit;
}
byte gunstick(byte code) { // out: FF(no), FE(fire), FB(lit)
    // lmb should last at least 6 frames (see: Guillermo Tell)
    static uint64_t down = 0;
    struct mouse m = mouse();
    if( m.lb || m.rb || ZX_MOUSE_AUTOFIRE_BUTTON ) down = ticks;
    int64_t lapsed = ticks - down;

    int hit = ( lapsed >= 0 && lapsed < (70908 * 6));
    if( hit ) code &= 0xFE; // if triggered

    if( hit ) {
        int lit = 0;

        int gunx = m.x - (_32 - mouse_offsets()[0]);
        int guny = m.y - (_24 - mouse_offsets()[1]);

        // light
        // check whether attribute is white
        if (gunx >= 0 && gunx < 256)
        if (guny >= 0 && guny < 192) {
            int x = gunx / 8;
            int y = guny / 8;

            int cell = x;
            int line = guny;
            int offset = ((line & 7) << 8) + ((line & 0x38) << 2) + ((line & 0xC0) << 5) + cell;
            byte *pixel = VRAM + offset;
            byte *attr = VRAM + 6144 + 32 * y + x;
            int ink = *attr & 7;
            int paper = (*attr >> 3) & 7;
            int bright = *attr & 0x40;

            // find bright luma pixels (green 4, cyan 5, yellow 6, white 7)
            // if(ink >= 4 || paper >= 4 ) lit = 1;
            // find white attribs (gunstick)
            // if (*pixel ? ink >= 0x04 : paper >= 0x07) lit = 1;
            // pixel perfect
            // extern Tigr *canvas;
            // if( (canvas->pix[(gunx+_32) + (guny+_24) * _256].rgba & RGB(255,255,255)) > RGB(128,128,128)) lit = 1;
            // debug pixel perfect
            // canvas->pix[(gunx+_32) + (guny+_24) * _256].rgba = RGB(255,0,0);

            lit = *pixel ? ink >= 4 : paper >= 6;

            #if DEV // debug
            if( ZX_DEVTOOLS ) {
                printf("lit:%d gun[%d,%d] attr[%d,%d]=%02x\n", lit, gunx,guny, x,y,(byte)*attr);
                *pixel = 0xFF, *attr = 0x38;
            }
            #endif
        }

        if( lit ) code &= 0xFB;
    }

    return code;
}


byte inport_(word port) {
    // if(port != 0x1ffd && port != 0x2ffd && port != 0x3ffd && port != 0x7ffe) { puts(regs("inport")); printf(" in port[%04x]\n", port); }
    //[0102] alien syndrome.dsk ??
    //[0002] alien syndrome.dsk ??
    //[fefe] alien syndrome.dsk


    // XXXXXXXX ---1 1111 when trdos is enabled. ports: 1F,3F,5F,7F,FF
    if( (port & 3) == 3 ) { //  (port & 0xff1f) == 0x1f) { // 
        int trdos = ZX_PENTAGON/*beta128*/ && MEMr[0] == (rom + 0x4000 * 2);
        if( trdos ) {
            // status(0)/track(1)/sector(2)/data(3)/ready(4) WD read registers

            int index;
#if 0
            // convert 1F,3F,5F,7F,FF ports into 0,1,2,3,4 index
            index = ((port & 0x80 ? 0xA0-1 : port) >> 5) & 7;
#else
            if(port & 0x80) index = WD1793_SYSTEM;
            else if( ( port & 0x60 ) == 0x60 ) index = WD1793_DATA;
            else if( ( port & 0x60 ) == 0x40 ) index = WD1793_SECTOR;
            else if( ( port & 0x60 ) == 0x20 ) index = WD1793_TRACK;
            else if( ( port & 0x60 ) == 0x00 ) index = WD1793_STATUS;
#endif

            // read from reg 0xFF(4) system 
            // D0, D1   - diskdrive select. 00 for drive A, 01 for drive B 10 for drive C, 11 for drive D
            // D2       - hardware microcontroller reset. by resetting and then setting this bit again, we can form impulse of microcontroller reset. usually this reset happens in very begin of TR-DOS session. 
            // D3       - this digit blocks signal HLT of microcontroller. For normal work must contain '1'. 
            // D4       - Diskdrive head select. contents of this digit translates directly to diskdrive. 0 means first head or 'bottom' side of disk, 1 - second head/'top' side of disk.
            // D5       - Density select. reset of this digit makes microcontroller works in FM mode, seted digit - MFM.
            // D6 DRQ   - signal showing request of data by microcontroller 
            // D7 INTRQ - signal of completion of execution of command.
            return Read1793(&wd, index);
        }
    }


    // +2a/+3
    if( ZX >= 210 )
    {
        if((port & 0xf002) == 0x2000) { return inport_0x2ffd(); } // 0010xxxx xxxxxx0x   fdc status
    }

    // Reads from port 0x7ffd cause a crash, as the 128's HAL10H8 chip does not distinguish between reads and writes to this port, resulting in a floating data bus being used to set the paging registers.
    // PRINT IN 32765 ; 0x7ffd
    // https://sinclair.wiki.zxnet.co.uk/wiki/ZX_Spectrum_128#HAL_bugs
    if ( (ZX|ZX_PENTAGON) == 128 || ZX == 200 ) 
        if( ZX_HAL10H8 )
            if(!(port & (0xFFFF^0x7FFD))) { uint8_t v = floating_bus ? *floating_bus : 0xFF; port_0x7ffd(v); return v; }

    if( ZX_MOUSE == 1 )
    {
        // kempston mouse detection
        if(port == 0xFADF) kempston_mouse |= 1; //button(s)
        if(port == 0xFBDF) kempston_mouse |= 2; //X
        if(port == 0xFFDF) kempston_mouse |= 4; //Y

        // kempston mouse
        if( (kempston_mouse & 3) == 3 ) { // was: ==7 before, but arkanoid does not use Y.
            struct mouse m = mouse();
            if(!(port & (0xFFFF^0xFADF))) return mouse_clip(1), mouse_cursor(0), 0xFF&~(1*m.rb+2*(m.lb|ZX_MOUSE_AUTOFIRE_BUTTON)+4*m.mb); // ----.--10.--0-.----  =  Buttons: D0 = RMB, D1 = LMB, D2 = MMB
            if(!(port & (0xFFFF^0xFBDF))) return mouse_clip(1), mouse_cursor(0),  m.x;                         // ----.-011.--0-.----  =  X-axis:  $00 … $FF
            if(!(port & (0xFFFF^0xFFDF))) return mouse_clip(1), mouse_cursor(0), -m.y;                         // ----.-111.--0-.----  =  Y-axis:  $00 … $FF
        }
    }

    // kempston port (31) @fixme: also seen mappings on port 55 and 95 (?)
    if( kempston_mouse != 3 ) { // was: 7 before. arkanoid does not use Y.
        // kempston joystick2 (55)
        if(!(port & (0xFF^55))) return kempston2;

        // gunstick (bestial warrior)
        if(ZX_GUNSTICK)
        if(!(port & (0xFF^0xDF))) return /*puts("gunstick kempston"),*/ gunstick(0xFF);

        // kempston joystick1 (31) (255^31-1)
        if(!(port & (0xFF^0xDF))) return kempston; //bit 5 low = reading kempston (as told pera putnik)
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

        // prevent emulator input being passed to zx. @fixme: is this check still needed?
        extern bool browser;
        if(!browser) {
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
    if( ZX >= 128 /*|| ZX_GUNSTICK*/ )
    if(!(port & (0xFFFF^0xFFFD))) return inport_0xfffd();

    // fuller joystick
    if(!(port & (0xFF^0x7F))) return fuller;

    // keyboard
    if(!(port & (0xFF^0xFE))) {
        byte code = 0xFF;

        // prevent emulator input being passed to zx. @fixme: is this check still needed?
        extern bool browser;
        if(!browser) {

        // test: Without this matrix behaviour Zynaps, for instance, 
        // won't pause when you press 5,6,7,8 and 0 simultaneously. @fixme
        if (!(port & 0x0100)) code &= keymap[4][1];
        if (!(port & 0x0200)) code &= keymap[3][1];
        if (!(port & 0x0400)) code &= keymap[2][1];
        if (!(port & 0x0800)) code &= keymap[1][1];
        if (!(port & 0x1000)) code &= keymap[1][2];
        if (!(port & 0x2000)) code &= keymap[2][2];
        if (!(port & 0x4000)) code &= keymap[3][2];
        if (!(port & 0x8000)) code &= keymap[4][2];

        }

        // issue2/3 handling
        // issue2 keyboard emulation (16K/48K only). bits 765 set. 560,000 48k sold models.
        // issue3 keyboard emulation (>=128 models). bits 7 5 set, 6 reset. 3,000,000 48k sold models.
        // On an Issue 3, an OUT 254 with bit 4 reset will give a reset bit 6 from IN 254.
        // On an Issue 2, both bits 3 and 4 must be reset for the same effect to occur.
        // test from css faq:
        // 10 OUT 254,BIN 11101111
        // 20 PRINT IN 254
        // 30 OUT 254,BIN 11111111
        // 40 PRINT IN 254
        // 50 GOTO 10
        // For a correct test do not press any key while running, and have no EAR input.
        // If the output is 191,255,191,255 etc, you are on real Spectrum Issue 3.
        // If output is always 191 or always 255, change the value in line 10 to BIN 11100111.
        // If output is then 191,255,191,255 etc, then you are on Spectrum Issue 2.
        // If output is still always 191 or always 255 you are on Spectrum emulator.

        if( (spk & ( issue2 ? 0x18 : 0x10 )) == 0 )
            code &= ~(1 << 6);

        // Bit 6 of IN-Port 0xfe is the EAR input bit
        // Note: Status of MIC line (block below), can alter the behavior of the last code assignment.
        // See: AbuSimbelProfanation(Gremlin) + polarity of ending TZX pause/stop blocks.

        // @todo: check this comment
        // Patrik Rak "In case there is no tape input, bit 6 depends on what was output
        // to bits 3 and 4. Issue 2 maps 00 to 0, issue 3 both 00 and 01 to 0.
        // We stick to issue 2 behavior as it is more compatible of the two (when it matters).
        // The +2A/+3 models however always return 0, so we honor that."

        if( 1 ) {
            ear = mic_read(tape_ticks);
            code = code ^ ear;
        }

        // issue2/3 keyboard tests:
        // - LOAD"" shall work for all issue2/3 models.
        // - keys shall work in BASIC for all issue2/3 models.
        // - AbuSimbelProfanation(Gremlin): issue2 can play the game
        // - Rasputin48k: issue3 can play the game. issue2 sounds like game crashed.
        // issue2/3 polarity tests:
        // - Lonewolf3-Side128 (+) shall load only in 128 models (issue3)
        // - Lonewolf3-Side48 (+) shall load only in 48 models (issue3 only)
        // - ForbiddenPlanet (-), V1 (-) and V2 (-) (issue2 only)
        // - Wizball.tap (+) (from PZXtools) shall load in 48/128 models (issue3)
        // - Basil Mouse Detective (+) and MASK (+) shall load on 48/128 issue3 models only
        // - Aforementioned games can still load if you toggle tape polarity; which is similar to switching issue2/3 behavior

        // now that we have read tape, let's update autoplay/autostop heuristics

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
            // - hint stop if strchr("uo",voc[voc_pos].debug) ; pa(u)se st(o)p
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

            if( tape_inserted() )
            {
                uint16_t pc = PC(cpu) - 2;
                byte *addr = ADDR8(pc);

                if( 0
                    || !memcmp(addr,  "\xDB\xFE\x1F", 3) // Common
                    || !memcmp(addr,  "\xDB\xFE\xE6\x40", 4) // Abadia del crimen
                    || !memcmp(addr,  "\xDB\xFE\xA9\xE6\x40", 5) // P-47, BloodBrothers
                    || !memcmp(addr,  "\xDB\xFE\xA0\xC2\x88\xFD", 6) // lonewolf3 v1
                    || !memcmp(addr,  "\xDB\xFE\xA0\xCA\x48\xFD", 6) // lonewolf3 v2
                    || !memcmp(addr,  "\xED\x78\xA8\xE6\x40", 5) // trivial pursuits' questions
                    || !memcmp(addr-1,"\x2C\xDB\xFE\xA4\xC2", 5) // gremlin2: Basil the Mouse Detective (10 Great Games 2),  Mask,  Mask (10 Great Games 2)
                    || !memcmp(addr-1,"\x2C\xDB\xFE\xA4\xCA", 5) // gremlin2: Basil the Mouse Detective (10 Great Games 2),  Mask,  Mask (10 Great Games 2)
                )
                autoplay_numreads++;

                // measure how large the slice of samples is. SLICE used to be 0.1, so it was a tenth (or slice) of a frame
                // it is 1 second now, so autoplay/autostop variables get updated once per second
                const float SLICE = 1.0; 

                ++autoplay_freq;

                if( (ticks - autoplay_last) > 69888*SLICE ) {
                    tape_hz = autoplay_freq/SLICE;
                    autoplay_last = ticks, autoplay_freq = 0;

                    // @fixme: wont stop tape or will burst CPU at max speed after load
                    // *ViajeAlCentroDeLaTierra (300Hz while redefining keys)
                    // *DynamiteDux (500Hz while playing intro)
                    // *OutRunEuropa (188Hz while displaying stop the tape message)
                    // beware of AbadiaDelCrimen (slowest Hz while loading probably, anti-turbo)

                    if( autoplay_numreads <= 9 ) { // 9Hz turrican, 16Hz plyuk, 228Hz outrun europa
                        ++autostop;
                    }
                    if( autoplay_numreads > 200 ) { // 273Hz turrican
                        ++autoplay;
                    }

                    autoplay_numreads = 0;
                }
            }
        }

        // LIGHTGUN (48)
        // LIGHTGUN GAMES 48K 1 (MISSILE GROUND ZERO,SOLAR INVASION,OPERATION WOLF)
        // LIGHTGUN GAMES 48K 2 (ROOKIE,ROBOT ATTACK,BULLSEYE)
        if(0)
        if( ZX < 128 && ZX_GUNSTICK ) {
            byte gunstick(byte); // out: FF(no), FE(fire), FB(lit)
            if( key_down(TK_SPACE) ) // lightgun() == 3 )
                code ^= 0x40;
            if( mouse().lb ) // lightgun() == 3 )
                code &= ~0x40;
            if( mouse().rb ) // lightgun() == 3 )
                code |= 0x40;
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
    if( ZX_PENTAGON ) return 0xFF;
    if( ZX >= 210 ) { // if +2A/+3 case (see: http://sky.relative-path.com/zx/floating_bus.html)
        if( (1 + (4 * port) && port < 0x1000) ) { // if in ports 1, 5, 9, 13 ... 4093
            if( !(page128 & (1<<5)) ) { // if paging is enabled
                if( floating_bus ) return *floating_bus | 1; // the value currently read by the ULA ORed with 1
                else return 0xFF; // @fixme: return last value that was written to, or read from, contended memory, and not strictly 0xFF
            }
        }
        return 0xFF;
    }
    return floating_bus ? *floating_bus : 0xFF;
}

byte inport(word port) {

    if( rzx.mode == RZX_PLAYBACK ) {
        rzx_last_port = rzx_get_input();
        if( rzx_last_port >= 0 ) return rzx_last_port;
    }

    byte v = inport_(port);
#if NDEBUG <= 0
    logport(port, v, 0);
#endif

    if( rzx.mode == RZX_RECORD ) {
        rzx_store_input(v);
    }

    return v;
}

#include "zx_sav.h"

#define FULL_QUICKSAVES 1 // 0 breaks run-a-head

struct quicksave {
    // cpu
    z80_t cpu;
    uint64_t pins;
    int int_counter;

    // rom patch
    int rom_patches;
    // control flags
    int ZX_TS;
    int ZX_FREQ;
    int ZX;
    // audio
    //int audio_pos;
    //float audio_buffer[AUDIO_BUFFER];
    // memory
    byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
    byte *MEMw[4]; //solid block of 16*4 = 64kb for writing

#if FULL_QUICKSAVES
    byte *floating_bus;

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
    int issue2;
    int nextkey;
    // joysticks
    byte kempston,kempston2,fuller;
    // mouse
    byte kempston_mouse;
    // ear
    byte ear;
    byte spk;
    // int
    byte zx_int;
    // ticks
    uint64_t ticks, TS;
    // plus3/2a
    byte page2a;
    // zx128
    byte page128;
    // audio
#if FULL_QUICKSAVES
    beeper_t buzz;
    struct ayumi ayumi[2];
    ay38910_t ay[2];
    int turbosound;
#endif
    // ay
    byte ay_current_reg[2];
    int ay_registers[2][ 16 ];
    // ula+
    byte ulaplus_mode;
    byte ulaplus_data;
    byte ulaplus_enabled;
    byte ulaplus_grayscale;
    byte ulaplus_registers[64+1];

    // @todo: pointers needed?
    //#include "dsk.c"
    FDC_DEFINES
#if FULL_QUICKSAVES
    WD1793 wd;
    FDIDisk fdd[NUM_FDI_DRIVES];
#endif

    VOC_DEFINES

    // tape
    int tape_hz;
    uint64_t tape_ticks;
    // autoplay
    int autoplay;
    int autostop;
    int autoplay_freq;
    uint64_t autoplay_last;
    unsigned autoplay_numreads;
    // flashload
    const byte *TAP_sof, *TAP_pof, *TAP_eof;

} quicksaves[10+1] = {0}; // user[0..9], sys/run-ahead reserved[10]

enum { quicksave_len = sizeof(struct quicksave) };

#define EXPORT(f) (c->f = f)
#define IMPORT(f) (f = c->f)

void* quicksave(unsigned slot) {
    if( slot > countof(quicksaves) ) return 0;

    struct quicksave *c = &quicksaves[slot];

    #define $(member) \
        printf("%x..%x %s;\n", (int)offsetof(struct quicksave, member), (int)offsetof(struct quicksave, member) + (int)(sizeof( ((struct quicksave *)0)->member )), #member);

    int rampages = 16; // ZX > 300 ? 16 : ZX > 48 ? 8 : ZX > 16 ? 3 : 1;

    // cpu
    c->cpu = cpu;
    c->pins = pins;
    c->int_counter = int_counter;
    // rom patch
    c->rom_patches = rom_patches;
    // control flags
    c->ZX_TS = ZX_TS;
    c->ZX_FREQ = ZX_FREQ;
    c->ZX = ZX;
    // audio
//  c->audio_pos = audio_pos;
//  float audio_buffer[AUDIO_BUFFER];
    // memory
    for( int i = 0; i < 4; ++i ) c->MEMr[i] = MEMr[i];
    for( int i = 0; i < 4; ++i ) c->MEMw[i] = MEMw[i];
#if FULL_QUICKSAVES
    c->floating_bus = NULL; // floating_bus;
//    memcpy(c->dum, dum, 16384);
//    memcpy(c->rom, rom, 16384*4);
    memcpy(c->mem, mem, 16384*rampages); //16);
#endif
    // screen
    c->ZXFlashFlag = ZXFlashFlag;
    c->ZXBorderColor = ZXBorderColor;
    memcpy(c->ZXPalette, ZXPalette, sizeof(rgba)*64);
    // input
    memcpy(c->keymap, keymap, sizeof(int)*5*5);
    c->issue2 = issue2;
    c->nextkey = nextkey;
    // joysticks
    c->kempston = kempston;
    c->kempston2 = kempston2;
    c->fuller = fuller;
    // mouse
    c->kempston_mouse = kempston_mouse;
    // beeper
    c->ear = ear;
    c->spk = spk;
    // vsync
    c->zx_int = zx_int;
    // ticks
    c->ticks = ticks;
    c->TS = TS;
    // plus3/2a
    c->page2a = page2a;
    // zx128
    c->page128 = page128;
    // audio
#if FULL_QUICKSAVES
    c->buzz = buzz;
    c->ay[0] = ay[0];
    c->ay[1] = ay[1];
    c->ayumi[0] = ayumi[0];
    c->ayumi[1] = ayumi[1];
    c->turbosound = turbosound;
#endif
    // ay
    memcpy(c->ay_current_reg, ay_current_reg, sizeof(ay_current_reg));
    memcpy(c->ay_registers, ay_registers, sizeof(ay_registers));
    // ula+
    c->ulaplus_mode = ulaplus_mode;
    c->ulaplus_data = ulaplus_data;
    c->ulaplus_enabled = ulaplus_enabled;
    c->ulaplus_grayscale = ulaplus_grayscale;
    memcpy(c->ulaplus_registers, ulaplus_registers, sizeof(ulaplus_registers[0])*(64+1));
    // @todo
    //#include "dsk.c"
    FDC_EXPORT
#if FULL_QUICKSAVES
    c->wd = wd;
    memcpy(c->fdd, fdd, sizeof(fdd[0]) * 4); // sizeof(fdd));
#endif

    // @todo
    //#include "tap.c"
    VOC_EXPORT

    // tape
    c->tape_hz = tape_hz;
    c->tape_ticks = tape_ticks;
    // autoplay
    c->autoplay = autoplay;
    c->autostop = autostop;
    c->autoplay_freq = autoplay_freq;
    c->autoplay_last = autoplay_last;
    c->autoplay_numreads = autoplay_numreads;
    // flashload
    c->TAP_sof = TAP_sof;
    c->TAP_pof = TAP_pof;
    c->TAP_eof = TAP_eof;

    return c;
}
void* quickload(unsigned slot) {
    if( slot > countof(quicksaves) ) return 0;

    struct quicksave *c = &quicksaves[slot];

    if( !c->ZX ) return 0; // return if slot was never saved

    /*config*/(ZX = c->ZX);

    // control flags
    ZX_TS = c->ZX_TS;
    ZX_FREQ = c->ZX_FREQ;

    // zx128
    port_0x7ffd((page128 = c->page128) & ~32);
    // plus3/2a
    port_0x1ffd(page2a = c->page2a);

    // cpu
    cpu = c->cpu;
    pins = c->pins;
    // pins = z80_prefetch(&cpu, PC(cpu));
    int_counter = c->int_counter;

    // rom patch
    rom_patches = c->rom_patches;

    int rampages = 16; // ZX > 300 ? 16 : ZX > 48 ? 8 : ZX > 16 ? 3 : 1;

    // audio
//  audio_pos = c->audio_pos;
//  float audio_buffer[AUDIO_BUFFER];
//  memset(audio_buffer, 0, sizeof(audio_buffer[0])*AUDIO_BUFFER);
    // memory
    for( int i = 0; i < 4; ++i ) MEMr[i] = c->MEMr[i];
    for( int i = 0; i < 4; ++i ) MEMw[i] = c->MEMw[i];
#if FULL_QUICKSAVES
    floating_bus = c->floating_bus;
//  memcpy(dum, c->dum, 16384);
//  memcpy(rom, c->rom, 16384*4);
    memcpy(mem, c->mem, 16384*rampages); //16);
#endif
    // screen
    ZXFlashFlag = c->ZXFlashFlag;
    ZXBorderColor = c->ZXBorderColor;
    memcpy(ZXPalette, c->ZXPalette, sizeof(rgba)*64);
    // input
    memcpy(keymap, c->keymap, sizeof(int)*5*5);
    issue2 = c->issue2;
    nextkey = c->nextkey;
    // joysticks
    kempston = c->kempston;
    kempston2 = c->kempston2;
    fuller = c->fuller;
    // mouse
    kempston_mouse = c->kempston_mouse;
    // beeper
    ear = c->ear;
    spk = c->spk;
    // vsync
    zx_int = c->zx_int;
    // ticks
    ticks = c->ticks;
    TS = c->TS;
    // audio
#if FULL_QUICKSAVES
    buzz = c->buzz;
    ay[0] = c->ay[0];
    ay[1] = c->ay[1];
    ayumi[0] = c->ayumi[0];
    ayumi[1] = c->ayumi[1];
    turbosound = c->turbosound;
#endif
    // ay
    memcpy(ay_current_reg, c->ay_current_reg, sizeof(ay_current_reg));
    memcpy(ay_registers, c->ay_registers, sizeof(ay_registers));
    // ula+
    ulaplus_mode = c->ulaplus_mode;
    ulaplus_data = c->ulaplus_data;
    ulaplus_enabled = c->ulaplus_enabled;
    ulaplus_grayscale = c->ulaplus_grayscale;
    memcpy(ulaplus_registers, c->ulaplus_registers, sizeof(ulaplus_registers[0])*(64+1));
    // @todo
    //#include "dsk.c"
    FDC_IMPORT
#if FULL_QUICKSAVES
    wd = c->wd;
    memcpy(fdd, c->fdd, sizeof(fdd[0]) * 4); // sizeof(fdd));
#endif

    //#include "tap.c"
    VOC_IMPORT

    // tape
    tape_hz = c->tape_hz;
    tape_ticks = c->tape_ticks;
    // autoplay
    autoplay = c->autoplay;
    autostop = c->autostop;
    autoplay_freq = c->autoplay_freq;
    autoplay_last = c->autoplay_last;
    autoplay_numreads = c->autoplay_numreads;
    // flashload
    TAP_sof = c->TAP_sof;
    TAP_pof = c->TAP_pof;
    TAP_eof = c->TAP_eof;

    return c;
}



// https://worldofspectrum.org/forums/discussion/comment/539714
// warm reset: IR set to 0000h. All other registers keep their previous values.
// cold reset: IR set to 0000h. All other registers are set to FFFFh.

void z80_quickreset(int warm) {
    if( warm ) {

        #if 0
            // http://www.z80.info/interrup.htm
            IFF1(cpu) = IFF2(cpu) = IM(cpu) = 0;
            PC(cpu) = I(cpu) = R(cpu) = 0;
            SP(cpu) = AF(cpu) = 0xffff;
            return;
        #endif

            PC(cpu) = I(cpu) = R(cpu) = 0;

    } else {
        #if 0
            #if NEWCORE
                pins = z80_reset(&cpu);
            #else
                z80_reset(&cpu);
            #endif
            return;
        #endif

            PC(cpu) = I(cpu) = R(cpu) = 0;
            
            AF(cpu) = BC(cpu) = DE(cpu) = HL(cpu) =
            WZ(cpu) = SP(cpu) = IX(cpu) = IY(cpu) =
            AF2(cpu) = BC2(cpu) = DE2(cpu) = HL2(cpu) = 0xFFFF;
    }
}

static struct zxdb ZXDB;

int reload(int model, int forced) {
    int load(const char *fname, int model);
    if( ZX_MEDIA ) if( load(ZX_MEDIA,model * !!forced) ) return 1;
    return 0;
}

void eject(int forget_media) {
    Reset1793(&wd,fdd,WD1793_EJECT);
    fdc_reset();
    tape_reset();
    media_seek[0] = 0; //media_reset();
    if( forget_media ) if( ZX_MEDIA ) free(ZX_MEDIA), ZX_MEDIA = 0;
    RZX_reset();
    ZXDB = zxdb_free(ZXDB);
}

void reset(unsigned FLAGS) {
#if 1
    ZX_KLMODE_PATCH_NEEDED = !!ZX_KLMODE;
#else
    ZX_KLMODE = 0, ZX_KLMODE_PATCH_NEEDED = 0;
#endif

    if( ZX != 128 ) ZX_PENTAGON = 0;

    page2a = ZX < 210 ? 128 : 0; port_0x1ffd(page2a);
    page128 = ZX < 128 ? 32 : 0; port_0x7ffd(page128);

    z80_quickreset(1);

    #if NEWCORE
        pins = z80_prefetch(&cpu, PC(cpu));
    #endif
        ticks=0;

    /*page128 = 0;*/
    //if(ZX>=128) port_0x7ffd(page128 = 0); // 128
    //if(ZX>=210) port_0x1ffd(page2a = 0); // +2a/+3

    ear = 0;
    spk = 0;
    beeper_reset(&buzz);

    ay_reset();
    ayumi_reset(&ayumi[0]);
    ayumi_reset(&ayumi[1]);
    ay38910_reset(&ay[0]);
    ay38910_reset(&ay[1]);

    mixer_reset();
    audio_reset();

    ula_reset();
    palette_use(ZX_PALETTE, 0);

    mouse_clip(0);
    mouse_cursor(1);
    kempston_mouse = 0;

    tape_hz = 0;
    tape_ticks = 0;
    autoplay = 0;
    autostop = 0;
    tape_rewind();

    if( FLAGS & KEEP_MEDIA ) {
        Reset1793(&wd,fdd,WD1793_KEEP);
    } else {
        eject(0);
    }

}

void boot0(int model, unsigned FLAGS) {
    dum = (char*)realloc(dum, 16384);    // dummy page
    rom = (char*)realloc(rom, 16384*4);  // +3
    mem = (char*)realloc(mem, 16384*16); // scorpion

    config(ZX);

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
    ay_reset();

    // floooh's
    ay38910_desc_t ay_desc = {0};
    ay_desc.type = AY38910_TYPE_8912;
    ay_desc.tick_hz = ZX_FREQ / 2;
    ay_desc.sound_hz = AUDIO_FREQUENCY; // * 0.75; // fix: -25% speed
    ay_desc.magnitude = 1.0f;
    ay38910_init(&ay[0], &ay_desc);
    ay38910_init(&ay[1], &ay_desc);

    // ayumi
    const int is_ym = 1; // should be 0, but 1 sounds more Speccy to me somehow (?)
    const int eqp_stereo_on = AUDIO_CHANNELS == 2;
    const double pan_modes[7][3] = { // pan modes, 7 stereo types
      {0.50, 0.50, 0.50}, // MONO, original
      {0.10, 0.50, 0.90}, // ABC, common in west-Europe
      {0.10, 0.90, 0.50}, // ACB, common in east-Europe
      {0.50, 0.10, 0.90}, // BAC
      {0.90, 0.10, 0.50}, // BCA
      {0.50, 0.90, 0.10}, // CAB
      {0.90, 0.50, 0.10}, // CBA
    };
    // assert( ayumi_freq < 2822400 );
    if (!ayumi_configure(&ayumi[0], is_ym, ((2822400/2)>>2) * (ZX_FREQ / 3546894.0), AUDIO_FREQUENCY)) { // ayumi is AtariST based, i guess. uses 2mhz clock instead
        die("ayumi_configure error (wrong ay freq?)");
    }
    const double *pan = pan_modes[1*(AUDIO_CHANNELS==2)];   // ABC or mono
    /*if(ZX_PENTAGON) pan = pan_modes[2*(AUDIO_CHANNELS==2)];*/ // ACB or mono, we handle this elsewhere now
    ayumi_set_pan(&ayumi[0], 0, pan[0], eqp_stereo_on);
    ayumi_set_pan(&ayumi[0], 1, pan[1], eqp_stereo_on);
    ayumi_set_pan(&ayumi[0], 2, pan[2], eqp_stereo_on);
    ayumi[1] = ayumi[0];

    fdc_reset();

    do_once Reset1793(&wd,fdd,WD1793_INIT);

    rom_restore();
    // if(ZX_TURBOROM) rom_patch_turbo();

    reset(FLAGS);
    z80_quickreset(0);

    if(0) // clear mem but leave vram entropy artifacts visible, because they're pretty when turning spectral for first time
    for( int i = 0; i < 16; ++i ) {
        byte *page = mem + 16384*i;
        if( page == VRAM ) continue;
        memset(page, 0x00, 16384);
    }    
}

void boot(int model, unsigned FLAGS) {
    if( model ) ZX = model & 0x1FE, ZX_PENTAGON = model & 1;

    boot0(ZX, FLAGS);

    // hack: force next cycle if something went wrong. @fixme: investigate why
    if( model & 1 ) if(!ZX_PENTAGON) ZX_PENTAGON = 1, rom_restore();
}

int gleck_mode() {
    unsigned pc = PC(cpu);
    if( pc < 0x4000 ) {
        byte MODE = READ8(0x5C41);
        byte FLAGS = READ8(0x5C3B);
        byte FLAGS2 = READ8(0x5C6A);
        //byte ERRNR = READ8(0x5C3A); // last known errno. 0xFF if running a prog, or fresh basic. !0xFF definitely in basic

        return
            MODE == 2 ? 'G' :
            MODE == 1 ? 'E' :
            FLAGS2 & 8 ? 'C' : FLAGS & 8 ? 'L' : 'K';
    }
    return 0;
}

// Function to check whether we're editing a listing in BASIC mode...
// (there must be a better way to detect it other than this)
int is_basic_mode() {
    unsigned pc = PC(cpu);
    if( pc < 0x4000 ) {
        unsigned sp = SP(cpu);
        for( int i = 0; i < 4; ++i ) {
            unsigned sp1 = sp + i;
            if( sp1 >= 0xFFFF ) break;
            pc = READ16(sp1);
            //printf("%04x, ", pc);
            if( pc < 0x4000 ) {
                pc &= 0xFF00;
                if( ZX >= 210 )
                if( pc == 0x0700 ) return /*puts("Y"),*/ 1; // +2A/+3: Main routine to process menus & editing functions
                if( pc == 0x0F00 ) return /*puts("Y"),*/ 1; // 16/48: 'editor' routines
                if( pc == 0x1500 ) return /*puts("Y"),*/ 1; // 16/48: input handler loop
                if( pc == 0x2600 ) return /*puts("Y"),*/ 1; // +2/128: editor
                if( pc == 0x2700 ) return /*puts("Y"),*/ 1; // p128: trdos
            }
        }
    }
    return /*puts("N"),*/ 0;
}

// 0: cannot load, 1: snapshot loaded, 2: tape loaded, 3: disk loaded, 4: ay loaded
int loadbin_(const byte *ptr, int size, int preloader) {

    if(!ZX_AUTOLOAD) preloader = 0;

    ZX_AUTOPLAY = 1;
    ZX_AUTOSTOP = 0;

    #define preload_snap(blob,len) ( sna_load(blob,len) || z80_load(blob,len) )

    // pre-loaders
    const byte*    bins[] = {
        ld16bas, ld48bas, ld128bas, ldplus2bas, ldplus2abas, ldplus3bas,
        ld16bin, ld48bin, ld128bin, ldplus2bin, ldplus2abin, ldplus3bin,
    };
    const unsigned lens[] = {
        sizeof ld16bas, sizeof ld48bas, sizeof ld128bas, sizeof ldplus2bas, sizeof ldplus2abas, sizeof ldplus3bas,
        sizeof ld16bin, sizeof ld48bin, sizeof ld128bin, sizeof ldplus2bin, sizeof ldplus2abin, sizeof ldplus3bin,
    };

    if(!(ptr && size > 10))
        return 0;

    if( load_ay(ptr, (int)size) ) {
        return 4;
    }

    // dsk first
    if(!memcmp(ptr, "MV - CPC", 8) || !memcmp(ptr, "EXTENDED", 8)) {
        if(preloader) preload_snap(ldplus3, sizeof(ldplus3));
        return dsk_load(ptr, size), 3;
    }

    // tapes first
    if( pzx_load(ptr, (int)size) ) {
        int slots[] = { [1]=0,[3]=1,[8]=2,[12]=3,[13]=4,[18]=5 };
        int is_bin = tape_type == 3, choose = slots[ZX/16] + 6 * is_bin;
        if(preloader) preload_snap(bins[choose], lens[choose]);
        ZX_AUTOSTOP = tape_num_stops > 1 ? 0 : size > 65535;
        //alert(va("numstops:%d", tape_num_stops));
        return 2;
    }
    if( tzx_load(ptr, (int)size) ) {
        int slots[] = { [1]=0,[3]=1,[8]=2,[12]=3,[13]=4,[18]=5 };
        int is_bin = tape_type == 3, choose = slots[ZX/16] + 6 * is_bin;
        if(preloader) preload_snap(bins[choose], lens[choose]);
        ZX_AUTOSTOP = tape_num_stops > 1 ? 0 : size > 65535;
        //alert(va("numstops:%d", tape_num_stops));
        return 2;
    }
    if( tap_load(ptr,(int)size) ) {
        int slots[] = { [1]=0,[3]=1,[8]=2,[12]=3,[13]=4,[18]=5 };
        int is_bin = tape_type == 3, choose = slots[ZX/16] + 6 * is_bin;
        if(preloader) preload_snap(bins[choose], lens[choose]);
        ZX_AUTOSTOP = size > 65535;
        return 2;
    }
    if( csw_load(ptr,(int)size) ) {
        int slots[] = { [1]=0,[3]=1,[8]=2,[12]=3,[13]=4,[18]=5 };
        int is_bin = tape_type == 3, choose = slots[ZX/16] + 6 * is_bin;
        if(preloader) preload_snap(bins[choose], lens[choose]);
        ZX_AUTOSTOP = 1;
        return 2;
    }

    if( RZX_load(ptr, size) ) {
        return 1;
    }
    if( szx_load(ptr, size) ) {
        return 1;
    }

    // headerless fixed-size formats now, sorted by ascending file size.
    if( scr_load(ptr, size) ) {
        return 1;
    }
    if( rom_load(ptr, size) ) {
        return 1;
    }
    if( sna_load(ptr, size) ) {
        return regs("load .sna"), 1;
    }

    // headerless variable-size formats now
    if( *ptr == 'N' && pok_load(ptr, size) ) {
        return 1;
    }
    if( size > 87 && z80_load(ptr, size) ) {
        return regs("load .z80"), 1;
    }

    puts("unknown file format");
    return 0;
}

int loadbin(const byte *ptr, int size, int use_preloader) {
    if(!(ptr && size))
        return 0;

    int ret = loadbin_(ptr, size, use_preloader);
    if( ret ) {
    int change_model = ret != 2; // you select zx model before loading any tape. however, other medias (disks,snapshots,ay,etc.) change model for you actually
    if( change_model ) ret = loadbin_(ptr, size, use_preloader); // retry. snapshots and disks need a model change because of preloaders
    if( change_model ) ret = loadbin_(ptr, size, use_preloader); // retry. tries to fix a stupid bug that wont initialize fdc/+3 properly (see: defendersoftheearth.dsk on a dev build + no .sav file)
    }
    return ret;
}

int mount(const char *file, byte *ptr, int len, int use_preloader) {
    int ok = 0;
    const char *ext = strrchr(file, '.');
    const char *extensions = ".img.mgt.trd.fdi.scl.$b .$c ."; // order must match FMT enum definitions in FDI header
    const char *extfound = ext ? strstri(extensions, ext) : NULL;
    if( !extfound ) {

        #if ZX_CUSTOM_ROMS
            if( ptr[0] == 0xF3 && len == 16384 &&               ZX <=  48 ) return rom48          = memcpy(malloc(0x4000*1), ptr, 0x4000*1), rom_restore(), PC(cpu) = 0, 1; // @leak
            if( ptr[0] == 0xF3 && len == 32768 && (ZX|ZX_PENTAGON) == 128 ) return rom128         = memcpy(malloc(0x4000*2), ptr, 0x4000*2), rom_restore(), PC(cpu) = 0, 1; // @leak
            if( ptr[0] == 0xF3 && len == 32768 &&               ZX == 200 ) return romplus2       = memcpy(malloc(0x4000*2), ptr, 0x4000*2), rom_restore(), PC(cpu) = 0, 1; // @leak
            if( ptr[0] == 0xF3 && len == 65536 &&               ZX >= 210 ) return romplus341     = memcpy(malloc(0x4000*4), ptr, 0x4000*4), rom_restore(), PC(cpu) = 0, 1; // @leak
            if( ptr[0] == 0xF3 && len == 32768 && (ZX|ZX_PENTAGON) == 129 ) return rompentagon128 = memcpy(malloc(0x4000*2), ptr, 0x4000*2), rom_restore(), PC(cpu) = 0, 1; // @leak
        #endif

        // regular load
        ok = loadbin(ptr, len, use_preloader);
    } else {
        // betadisk load
        printf("found ext: %d\n", (int)(extfound - extensions) );
        int format = FMT_IMG + ( extfound - extensions ) / 4;
        if( format > FMT_HOBETA ) format = FMT_HOBETA;
        printf(
            "FMT_IMG = %d\n"
            "FMT_MGT = %d\n"
            "FMT_TRD = %d\n"
            "FMT_FDI = %d\n"
            "FMT_SCL = %d\n"
            "FMT_HOB = %d\n" 
            "format = %d\n", FMT_IMG, FMT_MGT, FMT_TRD, FMT_FDI, FMT_SCL, FMT_HOBETA, format
        );

        // auto-patch .scl files to have a bootable basic file
        if( use_preloader && !memcmp(ptr, "SINCLAIR", 8) && !memmem(ptr, len, "boot    B", 9) ) {
            int num_files = ptr[8];
            for( int i = 0; i < num_files; ++i ) {
                char *file = ptr + 0x9 + 14 * i; // [x8] name + [x1] type + [x3] params + [x1] length
                char *ext = file + 8;
                if( strchr("Bb", *ext) ) {
                    memcpy(file, "boot    B", 9);
                }
            }
        }
        // auto-patch .trd files to have a bootable basic file. @todo: !strcmpi()->is_trd(ptr,len) instead?
        if( use_preloader && !strcmpi(ext, ".trd") && !memmem(ptr, len, "boot    B", 9) ) {
            int files_to_scan = 64;
            int bytes_per_file = 0x10;
            if( len > files_to_scan * bytes_per_file )
            for( int i = 0; i < files_to_scan; ++i ) {
                if( strchr("Bb", ptr[i * 0x10 + 8]) ) {
                    memcpy(&ptr[i * 0x10 + 0], "boot    B", 9);
                    break;
                }
            }
        }


        // this temp file is a hack for now. @fixme: implement a proper file/stream abstraction in FDI library
        for( FILE *fp = fopen(".Spectral/$$dsk", "wb"); fp; fwrite(ptr, len, 1, fp), fclose(fp), fp = 0);
        ok = LoadFDI(&fdd[0], ".Spectral/$$dsk", format);
        unlink(".Spectral/$$dsk");

        if( ok ) {
            // type RUN+ENTER if bootable disk found
            if( use_preloader && memmem(ptr, len, "boot    B", 9) ) {
                ok = loadbin(ldtrdos, ldtrdos_length, 0);
            }
        }
    }
    return ok;
}

char *guessnam_;
char *guessbin_;
int   guesslen_;

int guess_v2(const char *filename) {
    guessnam_ = NULL;
    guessbin_ = NULL;
    guesslen_ = 0;

    eject(0);

    // algorithm:
    // 1st) read zxdb info (Elite: 48k+128k, so choose 128k because it is the better choice), unless
    // 2nd) read zip filename recursively (Elite48.zip, so choose 48k), unless
    // 3rd) read filename info inside .zip (Elite128_2.tzx, so choose 128k), unless
    // 4th) read basic program "ELITE48", so choose 48k, unless
    // 5th) model is forced from UI
    // local cases: plyuk.zip/plyuk128.bas, ringo.zip/ringo128.bas td128/td128.bas
    // local cases: oldtower(48k).tap, oldtower(128k).tap, oldtower(pentagon).tap
    // zxdb  cases: elite, saboteur2, cabal, narc, nigelmansell, lonewolf3, ...
    //
    // @todo: provide hints to zxdb_search() like publisher or year if found in the filename. See: Jaws(1984).tap vs Jaws(AlternativeSoftwareLtd).tzx
    if( filename ) {

        static int len = 0; len = 0;
        static char *ptr = 0; if(ptr) free(ptr); ptr = 0;
        static int model = 0; model = 0;

        // [1] from zxdb
        {
            const char *id = filename;
            for( zxdb z = zxdb_search(id, 0); z.tok; zxdb_free(z), z.tok = 0) { // no multiple results
                model = zxdb_model(ZXDB = zxdb_dup(z));

                if( id[0] == '#' ) {
                    const
                    char *zxdb_url2(const char *id);
                    char *zxdb_download2(const char *id_, int *len);
                    ptr = zxdb_download2(id, &len);
                    if( ptr ) filename = zxdb_url2(id);
                }
            }
        }

        if( !ptr )
        ptr = readfile(filename, &len);

        if( !ptr )
        return 0;

        // [2] from filename

        int hint2 = 0;

repeat:;

        if( strrchr(filename, '/') ) filename = strrchr(filename, '/')+1;
        if( strrchr(filename,'\\') ) filename = strrchr(filename,'\\')+1;

        /**/ if( strstr (filename+1, "48") )       model = 48;
        else if( strstr (filename+1, "128") )      model = 128;
        else if( strstri(filename+1, "pentagon") ) model = 129;
        else if( strstri(filename+1, ".dsk") )     model = 300;
        else if( strstri(filename+1, ".$b") )      model = 129;
        else if( strstri(filename+1, ".$c") )      model = 129;
        else if( strstri(filename+1, ".scl") )     model = 129;
        else if( strstri(filename+1, ".img") )     model = 129;
        else if( strstri(filename+1, ".mgt") )     model = 129;
        else if( strstri(filename+1, ".fdi") )     model = 129;
        else if( strstri(filename+1, ".trd") )     model = 129;
        else if( strstr (filename+1, "ZX7") )      hint2 = 128;

        // [3] from container

            // is it zip,rar or gzip? uncompress & try to recurse...
            if( ptr && !memcmp(ptr, "\x1f\x8b\x08", 3) ) {
                unsigned unclen;
                char *unc = gunzip(ptr, len, &unclen);
                if( unc ) {
                    free( ptr );
                    ptr = unc;
                    len = unclen;

                    filename = va("%s", ptr + 10);
                    goto repeat;
                }
                else return 0; // cannot guess
            }

            // is it a zip? unzip & try to recurse...
            if( len > 8 && (!memcmp(ptr, "PK\3\4", 4) || !memcmp(ptr, "PK00PK\3\4", 8)) ) {
                for( FILE *fp = fopen(".Spectral/loadbin.zip","wb"); fp; fclose(fp), fp = 0) {
                    fwrite(ptr, len, 1, fp);
                }

                // scan archive. use 1st entry if multiple entries on archive are found
                char *bin = 0;
                for( zip *z = zip_open(".Spectral/loadbin.zip", "rb"); z; zip_close(z), z = 0 ) {
                    for( unsigned i = 0 ; !bin && i < zip_count(z); ++i ) {
                        if( !bin && file_is_supported(zip_name(z,i), GAMES_AND_ZIPS) ) {
                            bin = zip_extract(z,i);
                            if( bin )
                                filename = va("%s", zip_name(z,i)),
                                len = zip_size(z,i);
                        }
                    }
                    for( unsigned i = 0 ; !bin && i < zip_count(z); ++i ) {
                        if( !bin && file_is_supported(zip_name(z,i), ALL_FILES) ) {
                            bin = zip_extract(z,i);
                            if( bin )
                                filename = va("%s", zip_name(z,i)),
                                len = zip_size(z,i);
                        }
                    }
                }
                unlink(".Spectral/loadbin.zip");

                // rinse and repeat
                if( bin ) { if(ptr) free(ptr); ptr = bin; goto repeat; }
                else return 0; // cannot guess
            }

            // is it a rar? unrar & try to recurse...
            if( len > 4 && !memcmp(ptr, "Rar!", 4) ) {
                for( FILE *fp = fopen(".Spectral/loadbin.rar","wb"); fp; fclose(fp), fp = 0) {
                    fwrite(ptr, len, 1, fp);
                }

                // scan archive. use 1st entry if multiple entries on archive are found
                char *bin = 0;
                for( rar *z = rar_open(".Spectral/loadbin.rar", "rb"); z; rar_close(z), z = 0 ) {
                    for( unsigned i = 0 ; !bin && i < rar_count(z); ++i ) {
                        if( !bin && file_is_supported(rar_name(z,i), GAMES_AND_ZIPS) ) {
                            bin = rar_extract(z,i);
                            if( bin )
                                filename = va("%s", rar_name(z,i)),
                                len = rar_size(z,i);
                        }
                    }
                    for( unsigned i = 0 ; !bin && i < rar_count(z); ++i ) {
                        if( !bin && file_is_supported(rar_name(z,i), ALL_FILES) ) {
                            bin = rar_extract(z,i);
                            if( bin )
                                filename = va("%s", rar_name(z,i)),
                                len = rar_size(z,i);
                        }
                    }
                }
                unlink(".Spectral/loadbin.rar");

                // rinse and repeat
                if( bin ) { if(ptr) free(ptr); ptr = bin; goto repeat; }
                else return 0; // cannot guess
            }

        // [4] from zxdata

        int hint4 = guess_v1(ptr, len);
        if( hint4 ) model = hint4;
        if( hint2 ) if( !model ) model = hint2;

        guessbin_ = ptr;
        guesslen_ = len;
        guessnam_ = va("%s", filename);

        return model;
    }
    return 0; // cannot guess
}

// if( preloader ) zxdb_search(); // probably a game, so infer ZXDB from filename
// if( file_is_supported(zip_name(z,i), GAMES_ONLY) ) {

int load_should_clear = 1;
int load(const char *filename, int model) { // `model`: explicit model to use, or 0 to autodetect
#if TESTS
    printf("\n\n%s\n-------------\n\n", filename);
#endif

    if( strendi(filename, ".fx") ) {
        if( load_shader(filename) ) {
            if( ZX_SHADER ) free(ZX_SHADER), ZX_SHADER = 0;
            return crt(shader), ZX_SHADER = strdup(filename), ZX_SHADED = 1;
        }
    }
    if( strendi(filename, ".pal") ) {
        if( pal_load(filename) ) { // .pal
            palette_use(ZX_PALETTE = countof(ZXPalettes) - 1, 0);
            return 1;
        }
    }

    // guess media
    int hint = guess_v2(filename);
    if( model == 0 ) model = hint;
    if( model == 0 ) model = ZX|ZX_PENTAGON;
    if( model > 0 && load_should_clear && ZX_AUTOLOAD ) {
        /**/ if( model == 129 ) boot(ZX = 129, ~0u);
        else if( model == 300 ) boot(ZX = 300, ~0u);
        else if( model == 210 ) boot(ZX = 210, ~0u);
        else if( model == 210 ) boot(ZX = 210, ~0u); // @fixme +2B
        else if( model == 200 ) boot(ZX = 200, ~0u);
        else if( model == 128 ) boot(ZX = 128, ~0u); // @fixme USR0
        else if( model == 128 ) boot(ZX = 128, ~0u);
        else if( model ==  16 ) boot(ZX = 16, ~0u);
        else                    boot(ZX = 48, ~0u);
    }
    // rom_restore();

    int use_preloader = load_should_clear ? 1 : 0; // using a preloader would reset z80 state
    load_should_clear = 1;

    // mount media
    if( !guessnam_ ) return 0;
    if( !guessbin_ ) return 0;
    if( !guesslen_ ) return 0;
    int rc = mount(guessnam_, guessbin_, guesslen_, use_preloader);
    return rc;
}

// compares both urls. returns true they are apart by a single ascii step
int are_sequential_urls(const char *url1, const char *url2) {
    if( !url1 ) return 0;
    if( !url2 ) return 0;

    // basenames and their lengths
    const char *base1 = strrchr(url1, '/') ? strrchr(url1, '/')+1 : url1; int len1 = strlen(base1);
    const char *base2 = strrchr(url2, '/') ? strrchr(url2, '/')+1 : url2; int len2 = strlen(base2);

    // multi-load tapes and disks are well named (eg, Mutants - Side 1.tzx). 
    // following oneliner hack prevents some small filenames to be catched in the 
    // diff trap below. eg, 1942.tzx / 1943.tzx; they do not belong to each other
    // albeit their ascii diff is exactly `1`
    if( len1 > 8 )

    if( len1 == len2 ) {
        int diff = 0;
        for( int i = 0; i < len1; ++i ) {
            diff += base2[i] - base1[i];
        }
        return (diff * diff) == 1;
    }
    return 0;
}
