// # build (windows)
// cl app.c /FeSpectral.exe /O2 /MT /DNDEBUG=3 /GL /GF /arch:AVX
//
// # build (linux, debian)
// sudo apt-get install mesa-common-dev libx11-dev gcc libgl1-mesa-dev libasound2-dev
// gcc app.c -o Spectral -O3 -DNDEBUG=3 -Wno-unused-result -Wno-format -Wno-multichar -lm -ldl -lX11 -lGL -lasound -lpthread
//
// # done
// cpu, ula, mem, rom, 48/128, key, joy, ula+, tap, ay, beep, sna/128, fps, tzx, if2, zip, rf, menu, kms, z80, scr,
// key2/3, +2a/+3, fdc, dsk, autotape, gui, KL modes, load "" code, +3 fdc sounds, +3 speedlock, issue 2/3,
// pentagon, trdos, trdos (boot), translate game menus, 25/30/50/60 hz, game2exe,
// zxdb, custom tiny zxdb fmt, embedded zxdb, zxdb cache, zxdb download on demand, zxdb gallery
// ay player, pzx, rzx (wip), redefineable FN keys, mpg recording, mp4 recording, nmi, zx palettes,
// gamepads, gamepad bindings, turbosound (turboAY), autofire, alt z80 rates,
// glue sequential tzx/taps in zips (side A) -> side 1 etc)
// sequential tzx/taps/dsks do not reset model

#define SPECTRAL "v1.05-wip"

#if NDEBUG >= 2
#define DEV 0
#else
#define DEV 1
#endif

#define ALT_BORDER 0 // `1` not ready yet


// ref: http://www.zxdesign.info/vidparam.shtml
// https://worldofspectrum.org/faq/reference/48kreference.htm
// https://faqwiki.zxnet.co.uk/wiki/ULAplus
// https://foro.speccy.org/viewtopic.php?t=2319

// @todo:
// [ ] widescreen fake borders
// [ ] animated states
// [ ] auto-saves, then F11 to rewind. use bottom bar
// [ ] scan folder if dropped or supplied via cmdline
// [ ] live coding disasm (like bonzomatic)
// [ ] convert side-b/mp3s into voc/pulses
// [ ] db interface (F2 to rename)
//     on hover: show animated state if exists. show loading screen otherwise.
// [ ] embed torrent server/client to mirror the WOS/ZXDB/NVG/Archive.org distros
//     http://www.kristenwidman.com/blog/33/how-to-write-a-bittorrent-client-part-1/
//     https://wiki.theory.org/BitTorrentSpecification
//     http://bittorrent.org/beps/bep_0003.html
//     https://github.com/willemt/yabtorrent
//     https://github.com/jech/dht
//
// idea: when stop-block is off
// - turn autoplay=off
// - wait silently for any key to be pressed, then turn autoplay=on again
//
// profiler (auto)
// -           []
// -   []      [][]
// - [][][][][][][][][][] ...
// - 4000 4800 5000 5800  ...
// profiler (instrumented)
// IN TS
// OUT PROFILER+0, COLOR; OUT PROFILER+1, TS_DIFF
// profiler (instrumented, other; ticks get accumulated within a second; border is not rendered as usual)
// OUT 254, COLOR // on
// OUT 254, 0 // off
// analyzer
// https://www.youtube.com/watch?v=MPUL0LAUsck&ab_channel=theALFEST
// "Stuff that uses the keyboard" and "What wrote this pixel/attribute" is just awesome.
//

// todo (tapes)
// [ ] overlay ETA
// [ ] auto rewind
// [ ] auto-rewind at end of tape if multiload found (auto-stop detected)
// [ ] auto-insert next tape at end of tape (merge both during tzx_load! argv[1] argv[2])
// [ ] when first stop-the-tape block is reached, trim everything to the left, so first next block will be located at 0% progress bar
// [ ] trap rom loading, edge detection
// [ ] glue consecutive tzx/taps in disk
// [ ] glue: do not glue two consecutive tapes if size && hash match (both sides are same)
// [ ] glue: if tape1 does not start with a basic block, swap tapes
// [ ] prefer programs in tape with "128" string on them (barbarian 2; dragon ninja)
//     - if not found, and a "48" string is found, switch model to 48k automatically
// score128: 128 in filename + memcmp("19XY") X <= 8 Y < 5 + sizeof(tape) + memfind("in ayffe") + side b > 48k + program name "128" + filename128  -> load as 128, anything else: load as usr0
//      if single bank > 49k (navy seals), if size(tap)>128k multiload (outrun)
// test autotape with: test joeblade2,atlantis,vegasolaris,amc
// find 1bas-then-1code pairs within tapes. provide a prompt() call if there are more than 1 pair in the tape
//     then, deprecate 0x28 block

// notes about TESTS mode:
// - scans src/tests/ folder
// - creates log per test
// - 48k
// - exits automatically
// - 50% frames not drawn
// - 50% drawn in fastest mode
// @todo: tests
// - send keys via cmdline: "--keys 1,wait,wait,2"
// - send termination time "--maxidle 300"

// try
// https://github.com/anotherlin/z80emu
// https://github.com/kspalaiologos/tinyz80/
// https://github.com/jsanchezv/z80cpp/

// try
// https://damieng.com/blog/2020/05/02/pokes-for-spectrum/
// test(48): RANDOMIZE USR 46578
// test(2Aes): RANDOMIZE USR 20000

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#if !DEV // disable console logging in release builds (needed for linux/osx targets)
#define printf(...) 0
#define puts(...)   1
#endif

// 384x304 to fit border_break.trd resolution
enum { _320 = 384, _319 = _320-1, _321 = _320+1, _32 = (_320-256)/2 };
enum { _240 = 304, _239 = _240-1, _241 = _240+1, _24 = (_240-192)/2 };

#include "3rd.h"
#include "emu.h"
#include "sys.h"
#include "zx.h"
#include "app.h"

enum {
    SLOT_PNG,
    SLOT_MP4,
};

uint16_t file_counter(unsigned slot) {
    static uint16_t counter[16] = {0}; ++counter[slot %= 16];
    return counter[slot++] - 1;
}

int screenshot(const char *filename) {
    extern window *app;

    uint16_t count = file_counter(SLOT_PNG);

#if 0
    time_t timer = time(NULL);
    struct tm* tm_info = localtime(&timer);

    char stamp[32];
    strftime(stamp, 32, "%Y%m%d %H%M%S", tm_info);

    extern window* app;
    int ok1 = writefile(va("%s %s %04x.scr", filename, stamp, count), VRAM, 6912);
    int ok2 = tigrSaveImage(va("%s %s %04x.png", filename, stamp, count), app);
#else
    int ok1 = writefile(va("%s-%04x.scr", filename, count), VRAM, 6912);
    int ok2 = tigrSaveImage(va("%s-%04x.png", filename, count), app);
#endif

    return ok1 && ok2;
}

int load_config() {
    int errors = 0;
    if( !ZX_PLAYER ) for( FILE *fp = fopen(".Spectral/Spectral.ini", "rt"); fp; fclose(fp), fp = 0 ) {
        while( !feof(fp) ) {
        int tmp; char buf[128]; errors += fscanf(fp, "%[^=]=%d ", buf, &tmp) > 1;
        #define INI_LOAD(opt) if( strcmpi(buf, #opt) == 0 ) opt = tmp; else 
        INI_OPTIONS(INI_LOAD) {}
        }
    }
    return !errors;
}
int save_config() {
    mkdir(".Spectral", 0777);
    int errors = 0;
    if( !ZX_PLAYER ) for( FILE *fp = fopen(".Spectral/Spectral.ini", "wt"); fp; fclose(fp), fp = 0 ) {
        #define INI_SAVE(opt) errors += fprintf(fp, "%s=%d\n", #opt, opt) != 2;
        INI_OPTIONS(INI_SAVE)
    }
    return !errors;
}




// command keys: sent either physically (user) or virtually (ui)
int cmdkey;
const char *cmdarg;

const char *commands[] = {
    "♬",
    "  AY:Toggle AY core\n0:off, 1:fast, 2:accurate",
    "",
    "BOOT:Restart Game",
    "🗲",
    " CPU:Toggle Z80 speed\n0:25Hz, 1:30Hz, 2:50Hz, 3:60Hz, 4:75Hz, 5:7MHz, 6:14MHz",
    "",
    " DEV:Debugger",
    "d",
    "DEVT:Toggle DevTools",
    "",
    "FAST:Toggle FastTape\n0:off, 1:fast tape loading",
    "",
    "FIRE:Toggle Autofire\n0:off, 1:slow, 2:fast, 3:faster",
    PLAY_STR, //PAUSE_STR,
    "GAME:Game Browser",
    "\xB",
    "GUNS:Toggle Lightgun\n0:off, 1:lightgun+gunstick",
    "i",
    "HELP:About Dialog",
    JOYSTICK_STR,
    " JOY:Toggle Joysticks\n0:gamepad bindings, 1:sinclairI, 2:sinclair/interfaceII\n3: kempston+cursor+fuller+agf+protek",
    "k",
    "KEYB:Toggle Keyboard Issue 2/3\n2:older, 3:newer keyboard",
    "L",
    "  KL:Toggle 48-BASIC input mode\nK:token based, L:letter based",
    "⭡",
    "LOAD:Load savegame",
    "\x9",
    "MICE:Toggle Mouse\n0:off, 1:kempston",
    "",
    "MODE:Toggle Model\n16, 48, 128, +2, +2A, +3, Pentagon\n",
    PLAY_STR "\b\b\b" PLAY_STR "\b\b\b",
    "NEXT:Find next tape block",
    "",
    " NMI:Magic button (NMI)",
    "",
    "PAD0:Gamepad bindings",
    "",
    " PAL:Toggle Palette\n0:own, ...:others",
    SNAP_STR,
    " PIC:Screenshot",
    PLAY_STR,
    "PLAY:Play Tape",
    "+",
    "POLR:Toggle Tape Polarity",
    "\xf\b\b\b\xf\b\b\b",
    "PREV:Find previous tape block",
    "R",
    " REC:VideoREC\nRecords session to mp4 file (no sound)",
    "🯇",
    " RUN:Toggle Run-Ahead\n0:off, 1:improve input latency",
    "⭣",
    "SAVE:Save Game",
    FOLDER_STR,
    "SCAN:Scan local folder",
    "■",
    "STOP:Stop Tape",
    "T",
    "TENG:Translate game menu\n0:off, 1:poke game menu into English",
    "",
    "TURB:Toggle TurboROM\n0:off, 1:TurboROM .tap loader",
    "▒",
    "  TV:Toggle TV mode\n0:off, 1:rf, 2:crt, 3:rf+crt",
    "U",
    " ULA:Toggle ULAplus\n0:off, 1:on",
    "",
    "WIPE:Clear Medias",
//  "X",
//  "POKE:",
};

void vk_redefine2(const char *arg) {
    int vk, cmd, replace;
    if( sscanf(arg, "%d %d", &vk, &cmd)) {
        replace = vk < 0;
        vk = abs(vk);

        int in_use = 0;
        for( int j = 0; j < countof(ZX_FN); ++j ) in_use |= ( j != vk && cmd == ZX_FN[j] );
        if( cmd && in_use && !replace ) {
            ui_dialog_new(NULL);
            ui_dialog_option(0, "Command already bound to another key. Assign anyways?\n", NULL, 0,NULL);
            ui_dialog_separator();
            ui_dialog_option_ex(1, "Yes, continue\n", NULL, vk_redefine2,va("-%s",arg));
            ui_dialog_option(1, "Cancel\n", NULL, 0,NULL);
        }
        else {
            ZX_FN[vk] = cmd;
        }
    }
}
void vk_redefine(unsigned vk) { // 0=ESC,1..12=F1..F12
    ui_dialog_new(va("- Bind command to \5%s\7 key -\n",ZX_FN_STR[vk]));
    enum { COLUMNS = 3 };
    int num_commands = countof(commands)/2;
    for( int i = 0; i < num_commands; ++i ) {
        const char *icon = commands[i*2+0];
        const char *info = commands[i*2+1];
        union { struct { byte a,b,c,d; }; unsigned fourcc; } tag;
        tag.fourcc = bswap32(*((unsigned*)info));
        tag.a *= tag.a != 0x20;
        tag.b *= tag.b != 0x20;
        tag.c *= tag.c != 0x20;
        tag.d *= tag.d != 0x20;

        int in_use = 0;
        for( int j = 0; j < countof(ZX_FN); ++j ) in_use |= ( j != vk && tag.fourcc == ZX_FN[j] );

        int lf = !((i+1)%COLUMNS);
        const char *hint = va("%s",info+5); if(strchr(hint,'\n')) *strchr(hint,'\n') = '\0';
        ui_dialog_option_ex(1,va("%c%s %.*s%s",in_use ? '\2' : tag.fourcc == ZX_FN[vk] ? '\5':'\7',icon,4,info, lf ? "\n":""),hint,vk_redefine2,va("%d %d",vk,tag.fourcc));
    }
    for( int i = COLUMNS-(num_commands%COLUMNS); i--; ) {
        ui_dialog_option_ex(0,i?"      ":"      \n",NULL,0,0);
    }
    ui_dialog_separator();
    ui_dialog_option_ex(1,"Reset\n",NULL,vk_redefine2,va("%d %d",vk,0));
    ui_dialog_option(1,"Cancel\n",NULL,0,0);
}

void input() {
    // keyboard
    ZXKeyboardClear();

    // gamepad
    const unsigned pad = gamepad(), pad2 = pad;

        char* keys = tigrKeys(app);

        // custom gamepad mappings
        if( pad ) {
        if( pad&0x0001 ) keys[ZX_PAD[ 0]]|=1;
        if( pad&0x0002 ) keys[ZX_PAD[ 1]]|=1;
        if( pad&0x0004 ) keys[ZX_PAD[ 2]]|=1;
        if( pad&0x0008 ) keys[ZX_PAD[ 3]]|=1;
        if( pad&0x0010 ) keys[ZX_PAD[ 4]]|=1;
        if( pad&0x0020 ) keys[ZX_PAD[ 5]]|=1;
        if( pad&0x0040 ) keys[ZX_PAD[ 6]]|=1;
        if( pad&0x0080 ) keys[ZX_PAD[ 7]]|=1;
        if( pad&0x0100 ) keys[ZX_PAD[ 8]]|=1;
        if( pad&0x0200 ) keys[ZX_PAD[ 9]]|=1;
        if( pad&0x0400 ) keys[ZX_PAD[10]]|=1;
        if( pad&0x0800 ) keys[ZX_PAD[11]]|=1;
        if( pad&0x1000 ) keys[ZX_PAD[12]]|=1;
        if( pad&0x2000 ) keys[ZX_PAD[13]]|=1;
        if( pad&0x4000 ) keys[ZX_PAD[14]]|=1;
        if( pad&0x8000 ) keys[ZX_PAD[15]]|=1;
        }

    if( !ZX_DEVTOOLS ) {

        // autofire
        static byte frame = 0;
        int autofire = 0;
        /**/ if(ZX_AUTOFIRE == 1) autofire = !!(++frame & 0x08);
        else if(ZX_AUTOFIRE == 2) autofire = !!(++frame & 0x04);
        else if(ZX_AUTOFIRE == 3) autofire = !!(++frame & 0x02);

        // joysticks
        int left = keys[TK_LEFT], right = keys[TK_RIGHT];
        int up   = keys[TK_UP],   down = keys[TK_DOWN];
        int fire = keys[TK_TAB];
        ZXJoysticks(up,down,left,right,fire ^ (fire && ZX_AUTOFIRE ? autofire : 0));

        // keyboard
        #define KEYS(k) \
            k(0)k(1)k(2)k(3)k(4)k(5)k(6)k(7)k(8)k(9)\
            k(A)k(B)k(C)k(D)k(E)k(F)k(G)k(H)k(I)k(J)\
            k(K)k(L)k(M)k(N)k(O)k(P)k(Q)k(R)k(S)k(T)\
            k(U)k(V)k(W)k(X)k(Y)k(Z)
        #define K(x) if(keys[ 0[#x] ]) ZXKey(ZX_##x);
        KEYS(K);
        if(keys[TK_SPACE])      {ZXKey(ZX_SPACE); /*if(mic_on) mic_on = 0, tap_prev();*/ }
        if(keys[TK_BACKSPACE])  {ZXKey(ZX_SHIFT); ZXKey(ZX_0);}
        if(keys[TK_RETURN])      ZXKey(ZX_ENTER);
        if(keys[TK_SHIFT])       ZXKey(ZX_SHIFT);
        if(keys[TK_CONTROL])     ZXKey(ZX_SYMB);
        if(keys[TK_ALT])         ZXKey(ZX_CTRL);
    }

    // z80_opdone() returns 0 indefinitely while z80 is in halt state, often seen during BASIC sessions.
    // hack: force a benign keypress when user wants to close window; so z80_opdone() returns 1 hopefully.
    if( !window_alive(app) ) {
        static int flip = 0; if( flip ^= 1 ) ZXKey(ZX_SHIFT);
    }

    // prepare command keys

    #define KEYBINDING(N) \
    if( window_longpress(app, TK_F##N) ) vk_redefine(N); \
    else \
    if( window_released(app,  TK_F##N) ) if(!/*dialog_*/num_options) if((cmdkey = ZX_FN[N]) == 0) vk_redefine(N);

    KEYBINDING( 1);
    KEYBINDING( 2);
    KEYBINDING( 3);
    KEYBINDING( 4);
    KEYBINDING( 5);
    KEYBINDING( 6);
    KEYBINDING( 7);
    KEYBINDING( 8);
    KEYBINDING( 9);
    KEYBINDING(10);
    KEYBINDING(11);
    KEYBINDING(12);
}






enum { OVERLAY_ALPHA = 96 };
window *app, *ui, *dbg, *overlay, *dialog; int do_overlay;
int do_disasm;
float fps;





void help() {
    int total = numok+numwarn+numerr;

#if 0
    char buffer[1024] = {0};
    char *add = buffer;

    for( int i = 0; i < countof(ZX_FN); i++) {
        union { struct { uint8_t d,c,b,a; }; unsigned u; } x; x.u = bswap32(ZX_FN[i]);
        x.a += ' ' * !x.a;
        x.b += ' ' * !x.b;
        x.c += ' ' * !x.c;
        x.d += ' ' * !x.d;

        int found = -1;
        for( int j = 0; j < countof(commands); ++j ) {
            if( memcmp(commands[j], &x.d, 4) == 0 ) {
                found = j;
            }
        }

        if( found < 0 ) continue;

        if(!i && ZX_FN[i] ) add += sprintf(add, "- ESC: ");
        if( i && ZX_FN[i] ) add += sprintf(add, "- F%d: ", i);

        const char *text = commands[found] + 5;
        const char *lf = strchr(text,'\n');

        if( lf ) add += sprintf(add, "%.*s\n", (int)(lf - text), text);
        else add += sprintf(add, "%s\n", text);
    }

    char *help = va(
        "Spectral " SPECTRAL " (Public Domain).\n"
        "https://spectral.zxe.io\n\n"
        "ZXDB %s: %d entries\n"
        "Local Library: %d games found (%d%%)\n\n"
        "Spectral can be configured with a mouse.\n\n"
        "Here are some keyboard shortcuts, though:\n"
        "- ESC: Game browser\n"
        "%s"
        "- ALT+ENTER: Fullscreen\n"
        "- TAB+CURSORS (or GAMEPAD): Joystick\n\n"
        "Hold any F1..F12 key for 2 seconds to redefine it.\n",
        ZXDB_VERSION, zxdb_count(), numgames, 100 - (numerr * 100 / (total + !total)), 
        enum_mappings(buffer, 1));

    (alert)("Spectral " SPECTRAL, help);
#else
    char str1[512] = "ESC F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12\n", *ptr1 = str1;

    for( int i = 0; i < countof(ZX_FN); i++) {
        union { struct { uint8_t d,c,b,a; }; unsigned u; } x; x.u = bswap32(ZX_FN[i]);
        x.a += ' ' * !x.a;
        x.b += ' ' * !x.b;
        x.c += ' ' * !x.c;
        x.d += ' ' * !x.d;

        int found = -1;
        for( int j = 0; j < countof(commands); ++j ) {
            if( memcmp(commands[j], &x.d, 4) == 0 ) {
                found = j;
            }
        }

        int spaces = i > 0 && i < 9 ? 3 : 4;

        if( found >= 0 ) {
            const char *icon = commands[found - 1];
            const char *text = commands[found] + 5;

            ptr1 += sprintf(ptr1, "%s", icon);
        }

        ptr1 += sprintf(ptr1, "%*s", spaces - (found >= 0), "");
    }

    char *help = va(
        "<Spectral " SPECTRAL " (Public Domain).\n"
        "<https://spectral.zxe.io\n\n"
        "<ZXDB Library %s: %d entries\n"
        "<Local Library: %d games found (%d%%%% ✓)\n\n"
        "<ESC F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12\n"
        "<\f\f%s\n\n",
        ZXDB_VERSION + countof("version"), zxdb_count(), numgames, 100 - (numerr * 100 / (total + !total)),
        str1);

    ui_dialog_new(NULL);
    char *next;
    while( help && *help) {
        next = strchr(help, '\n');
        if( next ) {
            while(*next == '\n') ++next;
            char bak = *next; *next = 0;
            if(strstr(help, "https://"))
            ui_dialog_option(1, help,NULL, 'LINK',"https://spectral.zxe.io");
            else
            ui_dialog_option(0, help,NULL, 0,NULL);
            *next = bak;
        }
        help = next;
    }
    ui_dialog_option(1,"OK\n",NULL,0,NULL);
#endif
}

void titlebar(const char *filename) {
    filename = filename ? filename : "";
    const char *basename = strrchr(filename, *DIR_SEP_); basename += !!basename;
    const char *title = basename ? basename : filename;
    const char *models[] = { [1]="16",[3]="48",[8]="128",[9]="P128",[12]="+2",[13]="+2A",[18]="+3" };
    const char *titlebar = ZX_PLAYER ? __argv[0] : va("Spectral%s %s%s%s", DEV ? " DEV" : "", models[(ZX/16)|ZX_PENTAGON], title[0] ? " - " : "", title);
    window_title(app, titlebar);
}

void draw_ui() {

    // draw_compatibility_stats(ui);

    // ui
    int UI_LINE1 = (ZX_CRT ? 2 : 0); // first visible line

    struct mouse m = mouse();
    if( m.cursor == 0 ) {
        m.x = _320/2, m.y = _240/2; // ignore mouse; already clipped & hidden (in-game)
    } else {
        if( !active ) mouse_cursor(1);
    }

    // ui animation
    enum { _60 = 58 };
    int hovering_border = !active && !do_overlay && (m.x > (_320 - _60) || m.x < _60 );
    static float smooth; do_once smooth = hovering_border;
    smooth = smooth * 0.75 + hovering_border * 0.25;
    // left panel: game options
    if( smooth > 0.1 )
    {
        {
            // draw black panel
            TPixel transp = { 0,0,0, 192 * smooth };
            tigrFillRect(ui, -1,-1, smooth * _60, _240+2, transp);
            tigrLine(ui, smooth * _60-2,-1,smooth * _60-2,_240+2, ((TPixel){255,255,255,240*smooth}));
        }

        // left panel
        float chr_x = REMAP(smooth,0,1,-6,0.5) * 11, chr_y = REMAP(smooth,0,1,-3,2.5) * 11;
        int right = chr_x+8*4-4;
//        int bottom = chr_y+8*31.0-1;
        int bottom = _240-11*4+chr_y; // +8*31.0-1;

        // tape controls, top-left
        if( 0 && tape_inserted() ) {
            ui_at(ui,chr_x-4+1+1,chr_y-11*2-4+1);
            ui_label("     \f\f");
            // ui_at(ui,chr_x-4+1+1,bottom+1);
            //if( ui_click(NULL, REVPLAY_STR "\b\b\b" REVPLAY_STR "\f") ) cmdkey = 'PREV';
            if( ui_click(tape_playing() ? "Stop tape" : "Play tape", tape_playing() ? "■"/*PLAY_STR*/ "\f" : PLAY_STR "\b\b\b" PLAY_STR "\f" ) ) cmdkey = tape_playing() ? 'STOP' : 'PLAY';
            //if( ui_click(NULL, PLAY_STR "\b\b\b" PLAY_STR) ) cmdkey = 'NEXT';
        }

        // top-left zxdb column
        ui_at(ui,chr_x,chr_y);

        if( ZXDB.ids[0] ) {

        // zxdb
        if( ui_click(va("- %s -", ZXDB.ids[0]), "ZXDB\n")) visit(va("https://spectrumcomputing.co.uk/entry/%s", ZXDB.ids[0]));
        if( ui_click(va("- %s -", ZXDB.ids[2]), "Title\n"));
        if( ui_click(va("- %s -", ZXDB.ids[1]), "Year\n"));
        if( ui_click(va("- %s -", ZXDB.ids[4]), "Brand\n"));
        if( ui_click(va("- %s -", ZXDB.ids[7] + strspn(ZXDB.ids[7],"0123456789")), "Genre\n"));
        if( ZXDB.ids[6][0] && ui_click(va("- %s -", ZXDB.ids[6]), "Score\n"));
        if( ui_click(va("- %s -", strchr(ZXDB.ids[5], ',')+1), "Model\n"));

        int len;

        if( zxdb_url(ZXDB, "inlay") && ui_click(va("- Toggle Inlay -"), "Inlays\n")) { // @todo: include scanned instructions and tape scan, and mp3s
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "inlay"), &len); data; free(data), data = 0 ) {
                do_overlay ^= 1;
                tigrClear(overlay, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                if( do_overlay ) {
                    rgba *bitmap = ui_image(data,len, _320,_240, 0);
                    if( bitmap ) {
                        memcpy(overlay->pix, bitmap, _320 * _240 * 4);
                        free( bitmap );
                    }
                }
            }
        }
        if( zxdb_url(ZXDB, "screen") && ui_click(va("- Toggle Screen$ -"), "Screen\n")) {
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "screen"), &len); data; free(data), data = 0 ) {
                // loadbin(data, len, false);
                if( len == 6912 ) memcpy(VRAM, data, len);
            }
        }
        if( zxdb_url(ZXDB, "ay") && ui_click(va("- Toggle Music Tracks -"), "Tunes\n")) {
            int scrlen; char *scrdata = zxdb_download(ZXDB,zxdb_url(ZXDB, "screen"), &scrlen);

            // load & play tune
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "ay"), &len); data; free(data), data = 0 ) {
                loadbin(data, len, false);
            }

            // use loading screen as a background
            if( scrlen == 6912 ) memcpy(VRAM, scrdata, scrlen);
            free(scrdata);
        }
        if( zxdb_url(ZXDB, "mp3") && ui_click(va("- Toggle Bonus Track -"), "Bonus\n")) {
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "mp3"), &len); data; free(data), data = 0 ) {
                // loadbin(data, len, false);
            }
        }
        if( zxdb_url(ZXDB, "map") && ui_click(va("- Toggle Game Map -"), "Maps\n")) {
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "map"), &len); data; free(data), data = 0 ) {
                tigrClear(overlay, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                do_overlay ^= 1;
                if( do_overlay ) {
                    rgba *bitmap = ui_image(data,len, _320,_240, 0);
                    if( bitmap ) {
                        memcpy(overlay->pix, bitmap, _320 * _240 * 4);
                        free( bitmap );
                    }
                }
            }
        }
        if( zxdb_url(ZXDB, "poke") && ui_click("- Cheats -", "Cheats\n") ) { // @todo: selector
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "poke"), &len); data; free(data), data = 0 ) {
                loadbin(data, len, false);
            }
        }
        if( zxdb_url(ZXDB, "instructions") && ui_click(va("- Toggle Instructions -"), "Help\n")) { // @todo: word wrap. mouse panning. rmb close
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "instructions"), &len); data; free(data), data = 0 ) {
                do_overlay ^= 1;
                tigrClear(ui, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                if( do_overlay ) ui_monospaced = 0, ui_print(overlay, 4,4, ui_colors, as_utf8(replace(data, "\t", " ")));
            }
        }

        const char *roles[] = {
            ['?'] = "",
            ['C'] = "Code: ",
            ['D'] = "Design: ",
            ['G'] = "Graphics: ",
            ['A'] = "Inlay: ",
            ['V'] = "Levels: ",
            ['S'] = "Screen: ",
            ['T'] = "Translation: ",
            ['M'] = "Music: ",
            ['X'] = "Sound Effects: ",
            ['W'] = "Story Writing: ",
        };

        for( int i = 0; i < 9/*countof(ZXDB.authors)*/; ++i )
        if( ZXDB.authors[i] )
        if( ui_click(va("- %s%s -", roles[ZXDB.authors[i][0]], ZXDB.authors[i]+1), "Author\n"));

        if( ZXDB.ids[8] )
        if( ui_click(ZXDB.ids[8], "Tags\n"));
//          if( ui_click("- AY Sound -", "Feat.\n"));
//          if( ui_click("- Multicolour (Rainbow Graphics) -", "Feat.\n"));

        }

        // mags reviews
        // netplay lobby
        // #tags
    }

    int shift = window_pressed(app, TK_SHIFT);

    // right panel: emulator options
    if( 1 )
    {
        int chr_x = REMAP(smooth,0,1,_320+11,_320-6*11) + 0, chr_y = REMAP(smooth,0,1,-4,2.5) * 11;
        int right = chr_x+8*4-4;
        int bottom = _240-11*4+chr_y; // +8*31.0-1;

        {
            // draw black panel
            TPixel transp = { 0,0,0, 192 * smooth };
            tigrFillRect(ui, REMAP(smooth,0,1,_320,_320-_60), -1, _320*1/2, _240+2, transp);
            tigrLine(ui, REMAP(smooth,0,1,_320,_320-_60)+1,-1,REMAP(smooth,0,1,_320,_320-_60)+1,_240+2, ((TPixel){255,255,255,240*smooth}));
        }

        // bat anim
        static byte about = 0, frames = 26;
        static const char *abouts[] = {"",""};
        const char *bat = abouts[(about = (about+1)%frames) / (frames/2)];

        // record anim
        static byte rec_frame = 0; ++rec_frame;

        // expert mode
        int rmb = mouse().rb;

        ui_at(ui,chr_x - 8,chr_y-11*2-2-1);
        ui_y++;
        if( ui_press("- Frames per second -\n(hold to boost)", "\b%s\f%d", bat, (int)fps) ) cmdkey = 'MAX';
        ui_y--;

        ui_x = chr_x + 3 * 8;
        if( ui_click("- Screenshot -\nClick+SHIFT captures UI", "%c", SNAP_CHR) ) cmdkey = 'PIC'; // send screenshot command
//      if( ui_click("- VideoREC -\n.mp4 requires FFMPEG" ifdef(win32,".exe","") "; .mp1 otherwise", (rec_frame & 0x20) && record_active() ? "\2•\f":"•\f" )) cmdkey = 'REC';

        ///if( ui_press("- Full Throttle -\n(hold)", "%c\b\b\b%c\b\b\b%c%d\n\n", PLAY_CHR,PLAY_CHR,PLAY_CHR,(int)fps) ) cmdkey = 'MAX';

        ui_at(ui,chr_x - 4/*ui_x - 8*/,ui_y + 11*3);

        const char *models[] = { [1]=" 16 ",[3]=" 48 ",[8]="128K",[9]="P128",[12]=" +2 ",[13]=" +2A",[18]=" +3 " };
        ui_y--;
        if( ui_click("- Clear Medias -", "") ) cmdkey = 'WIPE';
        ui_y++;
        if( ui_click(rmb*17+"- Toggle Model -\0- Toggle Model -\n16, 48, 128, +2, +2A, +3, Pentagon\n", "%s%s",models[(ZX/16)|ZX_PENTAGON],ZX_ALTROMS ? "!":"")) if(rmb) cmdkey = 'MODE'; else
        {
            int mode = ZX + ZX_PENTAGON;
            ui_dialog_new("- Toggle Model -");
            ui_dialog_option(1,(mode!=16)+"\00516K\n", "Basic model with 16KB RAM", 'MODE',"16");
            ui_dialog_option(1,(mode!=48)+"\00548K\n", "Popular model with 48KB RAM", 'MODE',"48");
            ui_dialog_option(1,(mode!=128)+"\005128K\n", "Upgraded model with 128KB RAM and AY sound chip", 'MODE',"128");
            ui_dialog_option(1,(mode!=200)+"\005+2\n", "128K with built-in tape recorder", 'MODE',"200");
            ui_dialog_option(1,(mode!=210)+"\005+2A\n", "Rebranded +2 model with 64K ROM", 'MODE',"210");
            ui_dialog_option(1,(mode!=300)+"\005+3\n", "+2A compatible with built-in 3\" floppy disk drive", 'MODE',"300");
            ui_dialog_option(1,(mode!=129)+"\005Pentagon\n", "Soviet ZX Spectrum 128 clone with BetaDisk drive", 'MODE',"129");
        }
        ui_y-=1;
        if( ui_click(rmb*23+"- Magic button (NMI) -\0- Magic button (NMI) -\nGenerates a Non-Maskable Interrupt", "\f\n") ) cmdkey = 'NMI';
        ui_y+=1;

        int tableHz[] = {[50]=0,[60]=1,[100]=2,[120]=3,[150]=4,[200]=5,[400]=6};
        if( ui_click(rmb*21+"- Toggle Z80 speed -\0- Toggle Z80 speed -\n0:25Hz, 1:30Hz, 2:50Hz, 3:60Hz, 4:75Hz, 5:7MHz, 6:14MHz", "🗲\f%d", tableHz[(int)ZX_FPSMUL]) ) if(rmb) cmdkey = 'CPU'; else
        {
            ui_dialog_new("- Toggle Z80 speed -");
            ui_dialog_option(1,(ZX_FPSMUL!=400)+"\005400%% (14 MHz)\n",NULL,'CPU',"400");
            ui_dialog_option(1,(ZX_FPSMUL!=200)+"\005200%% (7 MHz)\n",NULL,'CPU',"200");
            ui_dialog_option(1,(ZX_FPSMUL!=150)+"\005150%% (75 Hz)\n",NULL,'CPU',"150");
            ui_dialog_option(1,(ZX_FPSMUL!=120)+"\005120%% (60 Hz)\n",NULL,'CPU',"120");
            ui_dialog_option(1,(ZX_FPSMUL!=100)+"\005100%% (50 Hz)\n",NULL,'CPU',"100");
            ui_dialog_option(1,(ZX_FPSMUL!= 60)+"\00560%% (30 Hz)\n",NULL,'CPU',"60");
            ui_dialog_option(1,(ZX_FPSMUL!= 50)+"\00550%% (25 Hz)\n",NULL,'CPU',"50");
        }
        ui_x += 8;
        if( ui_click(rmb*20+"- Toggle TurboROM -\0- Toggle TurboROM -\n0:off, 1:TurboROM .tap loader", !ZX_TURBOROM ? "\f0\n" : "\f1\n")) if(rmb) cmdkey = 'TURB'; else
        {
            ui_dialog_new("- Toggle TurboROM -");
            ui_dialog_option(1,(!ZX_TURBOROM)+"\5Turbo ROM loader\n",NULL,'TURB',"1");
            ui_dialog_option(1,( ZX_TURBOROM)+"\5Compatible ROM loader\n",NULL,'TURB',"0");
        }

        if( ui_click(rmb*21+"- Toggle TV mode -\0- Toggle TV mode -\n0:off, 1:rf, 2:crt, 3:crt+rf", "▒\f%d", (ZX_CRT << 1 | ZX_RF)) ) if(rmb) cmdkey = 'TV'; else
        {
            int mode = (ZX_CRT << 1 | ZX_RF);
            ui_dialog_new("- Toggle TV mode -");
            ui_dialog_option(1,(mode!=3)+"\5CRT and RF\n",NULL,'TV',"3");
            ui_dialog_option(1,(mode!=2)+"\5CRT only\n",NULL,'TV',"2");
            ui_dialog_option(1,(mode!=1)+"\5RF only\n",NULL,'TV',"1");
            ui_dialog_option(1,(mode!=0)+"\5Crisp\n",NULL,'TV',"0");
        }
        ui_x += 8;
        if( ui_click(rmb*19+"- Toggle Palette -\0- Toggle Palette -\n0:Spectral, X:others", "\f%d\n", ZX_PALETTE) ) if(rmb) cmdkey = 'PAL'; else
        {
            ui_dialog_new("- Toggle Palette -");
            for( int i = 0; i < countof(ZXPaletteNames); ++i)
            ui_dialog_option(1,va((ZX_PALETTE!=i)+"\005%s\n",ZXPaletteNames[i]),NULL,'PAL',va("%d",i));
        }

        if( ui_click(rmb*19+"- Toggle AY core -\0- Toggle AY core -\n0:off, 1:fast, 2:accurate", /*𝄞*/"♬\f%d",ZX_AY) ) if(rmb) cmdkey = 'AY'; else
        {
            ui_dialog_new("- Toggle AY core -");
            ui_dialog_option(1,(ZX_AY!=2)+"\5Accurate AY\n",NULL,'AY',"2");
            ui_dialog_option(1,(ZX_AY!=1)+"\5Fast AY\n",NULL,'AY',"1");
            ui_dialog_option(1,(ZX_AY!=0)+"\5Off\n",NULL,'AY',"0");
        }
        ui_x += 8;
        if( ui_click(rmb*19+"- Toggle ULAplus -\0- Toggle ULAplus -\n0:off, 1:on", "%c\f%d\n", ZX_ULAPLUS ? 'U':'u'/*CHIP_CHR '+'*/, ZX_ULAPLUS) ) if(rmb) cmdkey = 'ULA'; else
        {
            ui_dialog_new("- Toggle ULAplus -");
            ui_dialog_option(1,(!ZX_ULAPLUS)+"\5ULA+\n",NULL,'ULA',"1");
            ui_dialog_option(1,( ZX_ULAPLUS)+"\5Classic\n",NULL,'ULA',"0");
        }

        if( ui_click(rmb*21+"- Toggle Joysticks -\0- Toggle Joysticks -\n0:gamepad bindings, 1:sinclairI, 2:sinclair/interfaceII\n3: kempston+cursor+fuller+agf+protek", "%c\f%d", JOYSTICK_CHR, ZX_JOYSTICK)) if(rmb) cmdkey = 'JOY'; else
        {
            ui_dialog_new("- Toggle Joysticks -");
            ui_dialog_option(1,(ZX_JOYSTICK!=3)+"\5Kempston+Cursor+Fuller+Agf+Protek\n",NULL,'JOY',"3");
            ui_dialog_option(1,(ZX_JOYSTICK!=2)+"\5Sinclair/Interface II\n",NULL,'JOY',"2");
            ui_dialog_option(1,(ZX_JOYSTICK!=1)+"\5Sinclair I\n",NULL,'JOY',"1");
            ui_dialog_option(1,(ZX_JOYSTICK!=0)+"\5No joystick\n",NULL,'JOY',"0");
        }
        ui_x += 8;
        if( ui_click("- Gamepad bindings -", "\f0\n") ) // if(rmb) cmdkey = 'PAD0'; else
        {
            cmdkey = 'PAD0';
        }

        if( ui_click(rmb*20+"- Toggle Lightgun -\0- Toggle Lightgun -\n0:off, 1:lightgun+gunstick", /**/"\xB\f%d", ZX_GUNSTICK) ) if(rmb) cmdkey = 'GUNS'; else
        {
            ui_dialog_new("- Toggle Lightgun -");
            ui_dialog_option(1,(!ZX_GUNSTICK)+"\5Lightgun + Gunstick\n",NULL,'GUNS',"1");
            ui_dialog_option(1,( ZX_GUNSTICK)+"\5No lightguns\n",NULL,'GUNS',"0");
        }
        ui_x += 8;
        if( ui_click(rmb*20+"- Toggle Autofire -\0- Toggle Autofire -\n0:off, 1:slow, 2:fast, 3:faster", "\f%d\n", ZX_AUTOFIRE) ) if(rmb) cmdkey = 'FIRE'; else
        {
            ui_dialog_new("- Toggle Autofire -");
            ui_dialog_option(1,(ZX_AUTOFIRE!=3)+"\5Faster autofire\n",NULL,'FIRE',"3");
            ui_dialog_option(1,(ZX_AUTOFIRE!=2)+"\5Fast autofire\n",NULL,'FIRE',"2");
            ui_dialog_option(1,(ZX_AUTOFIRE!=1)+"\5Slow autofire\n",NULL,'FIRE',"1");
            ui_dialog_option(1,(ZX_AUTOFIRE!=0)+"\5No autofire\n",NULL,'FIRE',"0");
        }

        if( ui_click(rmb*17+"- Toggle Mouse -\0- Toggle Mouse -\n0:off, 1:kempston", "\x9\f%d", ZX_MOUSE) ) if(rmb) cmdkey = 'MICE'; else
        {
            ui_dialog_new("- Toggle Mouse -");
            ui_dialog_option(1,(!ZX_MOUSE)+"\5Kempston Mouse\n",NULL,'MICE',"1");
            ui_dialog_option(1,( ZX_MOUSE)+"\5No mouse\n",NULL,'MICE',"0");
        }
        ui_x += 8;
        if( ui_click(rmb*21+"- Toggle Run-Ahead -\0- Toggle Run-Ahead -\n0:off, 1:improve input latency", !ZX_RUNAHEAD ? "🯆\f0\n" : "🯇\f1\n") ) if(rmb) cmdkey = 'RUN'; else
        {
            ui_dialog_new("- Toggle Run-Ahead -");
            ui_dialog_option(1,(!ZX_RUNAHEAD)+"\5Improve input latency\n",NULL,'RUN',"1");
            ui_dialog_option(1,( ZX_RUNAHEAD)+"\5Off\n",NULL,'RUN',"0");
        }

        if( ui_click(rmb*31+"- Toggle 48-BASIC input mode -\0- Toggle 48-BASIC input mode -\nK:token based, L:letter based", "~%c~\f%d", ZX_KLMODE ? 'L' : 'K', ZX_KLMODE) ) if(rmb) cmdkey = 'KL'; else
        {
            ui_dialog_new("- Toggle 48-BASIC input mode -");
            ui_dialog_option(1,(!ZX_KLMODE)+"\5Tokens\n","Use traditional input mode",'KL',"0");
            ui_dialog_option(1,( ZX_KLMODE)+"\5Letters\n","Use modern input mode",'KL',"1");
        }
        ui_x += 8;
        if( ui_click(rmb*30+"- Toggle Keyboard Issue 2/3 -\0- Toggle Keyboard Issue 2/3 -\n2:older, 3:newer keyboard", "k\f%d\n", issue2 ? 2 : 3)) if(rmb) cmdkey = 'KEYB'; else
        {
            ui_dialog_new("- Toggle Keyboard Issue 2/3");
            ui_dialog_option(1,(!!issue2)+"\5Newer keyboard\n",NULL,'KEYB',"3");
            ui_dialog_option(1,( !issue2)+"\5Early keyboard\n",NULL,'KEYB',"2");
        }

        if( ui_click(rmb*20+"- Toggle FastTape -\0- Toggle FastTape -\n0:off, 1:fast tape loading", "\f%d", ZX_FASTTAPE )) if(rmb) cmdkey = 'FAST'; else
        {
            ui_dialog_new("- Toggle FastTape -");
            ui_dialog_option(1,(!ZX_FASTTAPE)+"\5Fast tape loading\n",NULL,'FAST',"1");
            ui_dialog_option(1,( ZX_FASTTAPE)+"\5Normal tape speed\n",NULL,'FAST',"0");
        }
        ui_x += 8;
        if( ui_click(rmb*25+"- Translate game menu -\0- Translate game menu -\n0:off, 1:poke game menu into English", "T\f%d\n", ZX_AUTOLOCALE)) if(rmb) cmdkey = 'TENG'; else
        {
            ui_dialog_new("- Translate game menu -");
            ui_dialog_option(1,1/*(!ZX_AUTOLOCALE)*/+"\5Poke translation\n","Poke game menu into English",'TENG',"1");
            ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5Cancel\n",NULL,'TENG',"0");
        }

        //if( ui_click("- Toggle TapePolarity -", "%c\f%d\n", mic_low ? '+':'-', !mic_low) ) cmdkey = 'POLR';

        if( DEV )
        if( ui_click("- Toggle DevTools -", "d\f%d\n", ZX_DEVTOOLS)) cmdkey = 'DEVT';

#if 0
        static int use_console = 0;
//        if( DEV )
        if( ui_click("- Toggle Console -", "\f%d\n", use_console)) { // cmdkey = 'TERM';
            void logo();
            use_console ^= 1;
            if( use_console ) AllocConsole(), /*enable_ansi(),*/ logo();
            else FreeConsole();
        }
#endif

        ui_at(ui,chr_x - 8,bottom+1);
        if( ui_click(NULL, "i") ) cmdkey = 'HELP';

        // debug
        ui_at(ui,right,bottom);
        if( ui_click("- Debug -", "") ) cmdkey = 'DEV'; // send disassemble command
    }

    ui_at(ui, _320-8*1-3, 0*11+4-2+2+1-2-1);
    if( ui_click("- VideoREC -\nClick+SHIFT captures UI (.mp4 requires FFMPEG)" ifdef(win32,".exe","") "; .mp1 otherwise", record_active() ? "\2\f":"\f" )) cmdkey = 'REC';

    if( ZX_BROWSER == 2 ) {
        // ZXDB builds
        ui_at(ui, 1*11-4+2-8+2, 0*11+4-2);
        if( 1/*!active*/ ) {
            if( ui_click(active ? "Resume" : "Pause", active ? PLAY_STR : PAUSE_STR) ) active ^= 1;
        }
    } else {
        // NOZXDB builds
        ui_at(ui, 1*11, 1*11);
        if( numgames ) {
            if( ui_click(active ? "Resume" : "Pause", active ? PLAY_STR : PAUSE_STR) ) active ^= 1;
        }
        else {
            if( ui_click("- Scan games folder -", "%c\n", FOLDER_CHR) ) cmdkey = 'SCAN';
        }
    }

    // bottom slider: tape browser. @todo: rewrite this into a RZX player/recorder too
    #define MOUSE_HOVERING()  (m.y >= (_240-11-11*(m.x>_320/6&&m.x<_320*5/6)) && m.y < (_240+11)) // m.y > 0 && m.y < 11 
    #define MOUSE_ACTION(pct) tape_seekf(pct)
    #define BAR_Y()           (_240-REMAP(smoothY,0,1,-7,7)) // REMAP(smoothY,0,1,-10,UI_LINE1)
    #define BAR_PROGRESS()    tape_tellf()
    #define BAR_VISIBLE()     ( (BAR_PROGRESS() <= 1. && mic_on/*tape_playing()*/) || MOUSE_HOVERING() ) // (m.y > -10 && m.y < _240/10) )
    #define BAR_FILLING(...)  if(tape_preview[x]) { __VA_ARGS__; }

    // slider
    int visible = !active && !do_overlay && BAR_VISIBLE();
    static float smoothY; do_once smoothY = visible;
    smoothY = smoothY * 0.75 + visible * 0.25;
    if( smoothY > 0.01 )
    {
        int y = BAR_Y();

        TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + y * _320];

        if( ZX_CRT && (y > _240/2) ) // scanline correction to circumvent CRT edge distortion
            bar -= _320 * 2;

        unsigned mark = BAR_PROGRESS() * _320;
        if( y < (_240/2) ) {
            // bars & progress (top)
            if(y>= 0) for( int x = 0; x < _320; ++x ) bar[x] = white;
            if(y>=-2) for( int x = 0; x < _320; ++x ) bar[x+2*_320] = white;
            if(y>= 1) for( int x = 0; x<=mark; ++x )  bar[x+_320] = white;
            if(y>=-2) for( int x = 1; x<=mark; ++x )  bar[-1+2*_320] = black;
            if(y>=-1) for( int x = 0; x < _320; ++x ) BAR_FILLING(bar[x+1*_320] = white);
            // triangle marker (top)
            if(y>=-4) bar[mark+4*_320] = white;
            if(y>=-5) for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+5*_320] = white;
            if(y>=-6) for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+6*_320] = white;
        } else {
            // triangle marker (bottom)
            if(y<=_239-0) for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+0*_320] = white;
            if(y<=_239-1) for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+1*_320] = white;
            if(y<=_239-2) bar[mark+2*_320] = white;
            // bars & progress (bottom)
            //if(y<=_239-4) for( int x = 1; x<=mark; ++x )  bar[-1+4*_320] = black;
            if(y<=_239-4) for( int x = 0; x < _320; ++x ) bar[x+4*_320] = white;
            if(y<=_239-5) for( int x = 0; x < _320; ++x ) BAR_FILLING(bar[x+5*_320] = white);
            if(y<=_239-5) for( int x = 0; x<=mark; ++x )  bar[x+5*_320] = white;
            if(y<=_239-6) for( int x = 0; x < _320; ++x ) bar[x+6*_320] = white;
        }

        // is mouse hovering
        if( MOUSE_HOVERING() ) {
            mouse_cursor(2);
            if( m.buttons ) {
                m.x = m.x < 0 ? 0 : m.x > _320 ? _320 : m.x;
                MOUSE_ACTION(m.x / (float)_320);
            }
        }
    }

    // manual tape handling
    if( 1 ) {
        int visible = !active && !do_overlay ? (m.y > -10 && m.y < _240/10) : 0;
        static float smoothY; do_once smoothY = visible;
        smoothY = smoothY * 0.75 + visible * 0.25;

        int y = REMAP(smoothY,0,1,-10,1*11);
        ui_at(ui, ui_x, y+1 );
#if 0
        if( ui_click(NULL, "\xf\b\b\b\xf") ) cmdkey = 'PREV';
        if( ui_click(NULL, "%c\b\b\b%c", PLAY_CHR, PLAY_CHR) ) cmdkey = 'NEXT';
        if( ui_click(NULL, "■") ) cmdkey = 'STOP';

        ui_x += 2;
        if( ui_click(NULL, "\xe") ) active ^= 1;
        ui_x += 1;
        if( ZX_AUTOPLAY )
        if( ui_click(NULL, "P%d,", autoplay));
        if( ZX_AUTOSTOP )
        if( ui_click(NULL, "S%d", autostop));
#endif
    }

    // bottom slider. @todo: rewrite this into a RZX player/recorder
    if( 0 )
    if( ZX_DEBUG )
    if( !active && !do_overlay ) {
        static float my_var = 0; // [-2,2]

        TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + (_240-7) * _320];
        unsigned mark = REMAP(my_var, -2,2, 0,1) * _320;
        // triangle marker (bottom)
        for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+0*_320] = white;
        for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+1*_320] = white;
        bar[mark+2*_320] = white;
        bar += _320 * 4;
        // bars & progress
        for( int x = 0; x < _320; ++x ) bar[x] = bar[x+2*_320] = white;
        for( int x = 0; x<=mark; ++x ) bar[x+_320] = white; bar[_320-1+_320] = white;
        // mouse seeking
        if( m.y >= (_240-11) && m.y < _240 ) {
            mouse_cursor(2);
            if( m.buttons/*&4*/ ) {
                m.x = m.x < 0 ? 0 : m.x > _320 ? _320 : m.x;
                float target = REMAP(m.x, 0,_320, 0.98,1.2);
                my_var = my_var * 0.50f + target * 0.50f ; // animate seeking
                // print my_var value
                char text[32]; sprintf(text, "%.4f", my_var);
                window_printxy(ui, text, (mark+5)/11.f,(_240-12.0)/11);
            }
        }
    }
}


char* game_browser(int version) { // returns true if loaded
    // scan files
    if( !numgames && !zxdb_loaded() ) {
        do_once {
            uint64_t then = time_ns();
            const char *folder = "./games/";
            #if TESTS
            folder = "./src/tests/";
            #endif
            rescan(folder);
            printf("%5.2fs rescan\n", (time_ns() - then)/1e9);
        }
    }

    if( !numgames && !zxdb_loaded() ) return 0;

    if( !active ) return 0;

    // game browser
    if( active ) {
        // disable overlay
        if( do_overlay ) tigrClear(overlay, tigrRGBA(0,0,0,0));
        do_overlay = 0;

        // restore mouse interaction in case it is being clipped (see: kempston mouse)
        mouse_clip(0);
        mouse_cursor(1);
    }

    char *entry = version == 2 ? game_browser_v2() : game_browser_v1();
    if( entry ) {
        return strendi(entry, "/") ? rescan(entry), NULL : entry;
    }

    return NULL;
}

void logo(void) {
    cputs("\3 \3 \3 \3 \3 \3 \3 \3 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \1 \1 \1 \1 \1█");
    cputs("\3█\3▀\3▀\3▀\3▀\3▀\2▀\2▀\2▀\2▀\2 \2█\2▀\2▀\2▀\2▀\2▀\2▀\2▀\2▀\2█\2 \6█\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6 \6█\6▀\4▀\4▀\4▀\4▀\4▀\4▀\4▀\4▀\4 \4▀\4▀\4▀\4▀\4█\4▀\4▀\4▀\4▀\5▀\5 \5█\5▀\5▀\5▀\5▀\5▀\5▀\5▀\5▀\5▀\5 \5▀\5▀\5▀\5▀\5▀\1▀\1▀\1▀\1▀\1█\1 \1█");
    cputs("\3▀\3▀\3▀\3▀\2▀\2▀\2▀\2▀\2▀\2█\2 \2█\2 \2 \2 \2 \2 \2 \2 \2 \6█\6 \6█\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6 \4█\4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4█\4 \4 \5 \5 \5 \5 \5█\5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5█\5▀\5▀\1▀\1▀\1▀\1▀\1▀\1▀\1█\1 \1█");
    cputs("\3▀\3▀\2▀\2▀\2▀\2▀\2▀\2▀\2▀\2▀\2 \2█\2▀\2▀\2▀\2▀\2▀\2▀\6▀\6▀\6▀\6 \6▀\6▀\6▀\6▀\6▀\6▀\6▀\6▀\6▀\4▀\4 \4▀\4▀\4▀\4▀\4▀\4▀\4▀\4▀\4▀\4▀\4 \4 \4 \4 \4 \5▀\5 \5 \5 \5 \5 \5 \5▀\5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5▀\1▀\1▀\1▀\1▀\1▀\1▀\1▀\1▀\1▀\1 \1▀" "\007 " SPECTRAL);
    cputs("\3 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2 \2█\2 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \6 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \4 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \5 \1 \1 \1 \1 \1 \1 \1 \1 \1 \1 \1" ANSI_RESET);

    // works on win, lin, osx. wont work on lubuntu/lxterminal, though.
    //cputs("\3┏\2┓\2 \6 \6 \6 \4 \4 \4 \5 \5 \5 \1┓");
    //cputs("\2┗\2┓\6┏\6┓\6┏\4┓\4┏\4╋\5┏\5┓\5┏\1┓\1┃");
    //cputs("\2┗\6┛\6┣\6┛\4┗\4 \4┗\5┗\5┛\5 \1┗\1┻\1┗" "\007" SPECTRAL);
    //cputs("\6 \6 \4┛\4 \4 \4 \5 \5 \5 \1 \1 \1 \1 " ANSI_RESET);

    //puts("┌┐          ┐");
    //puts("└┐┌┐┌┐┌┼┌┐┌┐│");
    //puts("└┘├┘└ └└┘ └┴└");
    //puts("  ┘          ");
    //puts("┏┐          ┐");
    //puts("└┐┏┐┏┐┏┼┏┐┏┐┃");
    //puts("└┛├┛└ └└┛ └┻└");
    //puts("  ┛          ");
    //puts("┏┓          ┓");
    //puts("┗┓┏┓┏┓┏╋┏┓┏┓┃");
    //puts("┗┛┣┛┗ ┗┗┛ ┗┻┗");
    //puts("  ┛          ");
}

#if defined _WIN32 && defined NDEBUG && NDEBUG > 0
int WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    if(1) {
        //NOTE: these calls are the closest i'm aware you can get to /SUBSYSTEM:CONSOLE
        //      in a gui program while cleanly handling existing consoles (cmd.exe),
        //      pipes (ninja) and no console (VS/RemedyBG; double-clicking the game).
        //      the other option is to compile with /SUBSYSTEM:CONSOLE and call FreeConsole()
        //      if no console is needed but it is amateur to flash a console for a second
        if (!AttachConsole(ATTACH_PARENT_PROCESS) && GetLastError() != ERROR_ACCESS_DENIED) assert(AllocConsole());
        printf("\n"); //print >= 1 byte to distinguish empty stdout from a redirected stdout (fgetpos() position <= 0)
        fpos_t pos = {0};
        if (fgetpos(stdout, &pos) != 0 || pos <= 0) {
            assert(freopen("CONIN$" , "r", stdin ));
            assert(freopen("CONOUT$", "w", stderr));
            assert(freopen("CONOUT$", "w", stdout));
        }
    }
#else
int main() {
#endif

#ifdef _WIN32 // 3rd_tfd.h requires this
    HRESULT lHResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif

#if !DEV && defined(_WIN32)
    // do not print logo on win32 (release). we dont want to flash a console window
#else
    // logo can be printed on linux/macos builds (dev+release) and also on win32 (dev) builds.
    logo();
#endif

    // install icon hooks for any upcoming window or modal creation
    window_override_icons();

    // convert relative paths
    for( int i = 1; i < __argc; ++i ) {
        if( __argv[i][0] != '-' ) {
            char full[MAX_PATH] = {0};
            realpath(__argv[i], full);
            __argv[i] = strdup(full); // @leak
        }
    }

    // initialize tests
    printer = stdout;
#if TESTS
    {
        if( __argc <= 1 ) die("error: no test file provided");
        printer = fopen(va("%s.txt", __argv[1]), "wt"); //"a+t");
        if(!printer) die("cant open file for logging");

        ZX_FASTCPU = 1;
    }
#endif

    // relocate to exedir
    cwdexe();
    // normalize argv[0] extension
#ifdef _WIN32
    __argv[0] = strdup(va("%s%s", __argv[0], strendi(__argv[0], ".exe") ? "" : ".exe" ));
#endif

    // init external zxdb (higher precedence)
    zxdb_init("Spectral.db");

    // init embedded zxdb (lower precedence)
    // also change app behavior to ZX_PLAYER if embedded games are detected.
    {
        int embedlen; const char *embed;
        for( unsigned id = 0; (embed = embedded(id, &embedlen)); ++id ) {
            if(!memcmp(embed + 0000, "\x1f\x8b\x08",3))
            if(!memcmp(embed + 0x0A, "Spectral.db",11)) {
                zxdb_initmem(embed, embedlen);
                continue;
            }
            if( embedlen ) ZX_PLAYER |= 1;
        }
    }

    // import config
    if(!ZX_PLAYER)
    load_config();

    // fixed settings on zxplayer builds
    if( ZX_PLAYER ) {
        ZX_RF = 1;
        ZX_CRT = 1;
    }

    // prepare title based on argv[0]
    char *apptitle = va("%s", __argv[0]);
    while( strrchr(apptitle,'\\') ) apptitle = strrchr(apptitle,'\\') + 1;
    while( strrchr(apptitle, '/') ) apptitle = strrchr(apptitle, '/') + 1;
    if( strrchr(apptitle, '.') ) *strrchr(apptitle, '.') = '\0';
    apptitle = va("%s%s", apptitle, DEV ? " DEV" : "");

    // app
    app = window_open(_32+256+_32, _24+192+_24, apptitle);
    ui = window_bitmap(_320, _240);
    dbg = window_bitmap(_320, _240);
    overlay = window_bitmap(_320, _240);
    dialog = window_bitmap(_320, _240);

    // postfx
    crt(ZX_CRT);

    // must be as close to frame() as possible
    audio_init();

    // zx
    boot(128, 0);

    // load embedded games (if any)
    {
        int embedlen; const char *embed;
        for( unsigned id = 0; (embed = embedded(id, &embedlen)); ++id ) {
            if(!memcmp(embed + 0000, "\x1f\x8b\x08",3))
            if(!memcmp(embed + 0x0A, "Spectral.db",11))
                continue;
            loadbin(embed, embedlen, 1);
        }
    }

    // zxplayer does not load/save state
    if(!ZX_PLAYER)

    {
        // import state
        for( FILE *state = fopen("Spectral.sav","rb"); state; fclose(state), state = 0) {
            if( import_state(state) )
                // pins = z80_prefetch(&cpu, cpu.pc),
                titlebar(0);
        }
    }


    // main loop
    do {

    #if NEWCORE
    do {
    #endif

#if 1
        // 4 parameters in our shader. we use parameters[0] to track time
        if( ZX_CRT )
        tigrSetPostFX(app, (ticks / (69888 * 50.)), 0, 0, 0);
        else
        tigrSetPostFX(app, 0, 0, 0, 1);

        // update background color so it matches. this is especially visible during fullscreen
        glColor[0] = glColor[1] = glColor[2] = 0, glColor[3] = 1;
#if 0
        if( !ZX_CRT )
        glColor[0] = ((ZXPalette[ZXBorderColor] >> 0) & 255) / 255.0,
        glColor[1] = ((ZXPalette[ZXBorderColor] >> 8) & 255) / 255.0,
        glColor[2] = ((ZXPalette[ZXBorderColor] >>16) & 255) / 255.0;
#endif

#endif

        // update titlebar
        if( ZXDB.ids[0] )
        titlebar( ZXDB.ids[2] );

        ui_frame_begin();
        input();
        if(do_overlay) ZXKeyboardClear(); // do not submit keys to ZX while overlay is drawn on top

        // handle ESC/RMB (cancel buttons)
        int escape = window_trigger(app, TK_ESCAPE);
        int cancel = escape || mouse().rb;
        if( cancel ) {
            // cancel dialog
            if( num_options ) ui_dialog_new(NULL);
            // cancel overlay
            else if( do_overlay ) do_overlay = 0, tigrClear(overlay, tigrRGBA(0,0,0,0));
            // else toggle browser (ESC key only)
            else if( !mouse().rb ) cmdkey = 'GAME';
        }

        int likely_loading = (PC(cpu) & 0xFF00) == 0x0500 ? 1 : tape_hz > 40;

        int tape_accelerated = ZX_FASTCPU ? 1
            : tape_inserted() && tape_peek() == 'o' ? 0 
            : tape_playing() && likely_loading && ZX_FASTTAPE;
        if( active ) tape_accelerated = 0;

        // z80, ula, audio, etc
        // static int frame = 0; ++frame;
        int do_sim = active ? 0 : 1;
        int do_drawmode = 1; // no render (<0), full frame (0), scanlines (1)
        int do_flashbit = tape_accelerated ? 0 : 1;
        int do_runahead = tape_accelerated ? 0 : ZX_RUNAHEAD;

#if TESTS
        // be fast. 50% frames not drawn. the other 50% are drawn in the fastest mode
        static byte even = 0; ++even;
        do_drawmode = even & 1; 

        // monitor test for completion
        static byte check_tests = 0;
        if( !check_tests++ )
        {
            static unsigned prev = 0;
            static unsigned stalled = 0;

            struct stat st;
            if( fstat(fileno(printer), &st) == 0 ) {
                if( prev == st.st_size ) ++stalled;
                else prev = st.st_size, stalled = 0;
            }

            // finish test after being idle for 15,000,000 frames
            if( stalled >= (50*300000/256) ) {
                fprintf(printer, "Quitting test because of inactivity.\n");
                exit(0);
            }
        }
#endif

        if( ZX_TURBOROM )
        rom_patch_turbo();
        rom_patch_klmode();

        static byte counter = 0; // flip flash every 16 frames @ 50hz
        if( !((++counter) & 15) ) if(do_flashbit) ZXFlashFlag ^= 1;

if( do_runahead == 0 ) {
        do_audio = 1;
        frame(do_drawmode, do_sim); //tape_accelerated ? (frame%50?0:1) : 1 );
} else {
        // runahead:
        // - https://near.sh/articles/input/run-ahead https://www.youtube.com/watch?v=_qys9sdzJKI // https://docs.libretro.com/guides/runahead/

        do_audio = 1;
        frame(-1, do_sim);

        quicksave(10);

        do_audio = 0;
        frame(do_drawmode, do_sim);

        quickload(10);
}

        tigrClear(ui, !active && !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,128));
            
        char* game = game_browser(ZX_BROWSER);
        if( game ) {
            active = 0;

            bool insert_next_disk_or_tape = false;
            if( last_load ) {
                if( 0 != strcmp(game, last_load) ) {
                    const char *a1 = game, *a2 = last_load;

                    // basenames and their lengths
                    const char *b1 = strrchr(a1, '/') ? strrchr(a1, '/')+1 : a1; int l1 = strlen(b1);
                    const char *b2 = strrchr(a2, '/') ? strrchr(a2, '/')+1 : a2; int l2 = strlen(b2);
                    // printf("%s(%d) %s(%d)\n", b1,l1, b2,l2);

                    // multi-load tapes and disks are well named (eg, Mutants - Side 1.tzx). 
                    // following oneliner hack prevents some small filenames to be catched in the 
                    // diff trap below. eg, 1942.tzx / 1943.tzx; they do not belong to each other
                    // albeit their ascii diff is exactly `1`.
                    if( l1 > 8 )

                    if( l1 == l2 ) {
                        int diff = 0;
                        for( int i = 0; i < l1; ++i ) {
                            diff += b1[i] - b2[i];
                        }
                        insert_next_disk_or_tape = diff == 1;
                    }
                }
            }

            int model = strstri(game, ".dsk") ? 300 : window_pressed(app, TK_SHIFT) ? 48 : 128;
            int must_clear = insert_next_disk_or_tape || strstr(game, ".pok") || strstr(game, ".POK") ? 0 : 1;
            int must_turbo = window_pressed(app,TK_CONTROL) || ZX_TURBOROM ? 1 : 0;
            int use_preloader = must_clear ? 1 : 0;

            if( must_clear ) boot(model, 0);
            if( must_turbo ) rom_patch_turbo();

            if( loadfile(game,use_preloader) ) {
                titlebar(game);

                // clear window keys so the current key presses are not being sent to the 
                // next emulation frame. @fixme: use ZXKeyboardClear(); instead
                memset(tigrInternal(app)->keys, 0, sizeof(tigrInternal(app)->keys));
                memset(tigrInternal(app)->prev, 0, sizeof(tigrInternal(app)->prev));
            }
        }

        // measure time & frame lock (50.01 fps)
        int max_speed = tape_accelerated || !ZX_FPSMUL || ZX_FASTCPU; // max speed if tape_accelerated or no fps lock
        if( max_speed ) {
            dt = tigrTime();
            // constant time flashing when loading accelerated tapes (every 16 frames @ 50hz)
            static float accum = 0; accum += dt;
            if( accum >= 0.32f ) accum = 0, ZXFlashFlag ^= 1;
        } else {
#if 0 // no lock
            dt = tigrTime();
#elif 0 // naive
            sys_sleep(1000/50.f); // 50 -> 39 fps
            dt = tigrTime();
#elif 0 // less naive
            dt = tigrTime();
            if( dt < (1000/50.f) ) sys_sleep( (1000/50) - dt );
#elif 0 // try precise sleep (requires SDL_DelayPrecise backend)
            static uint64_t prev_dt = 0;
            uint64_t now_dt = time_ns();
            int64_t diff = now_dt - prev_dt;
            prev_dt = now_dt;

            float target = (ZX_FPSMUL/100.0) * (ZX < 128 ? 50.08:50.01);
            int64_t frame_in_ns = ((1000/target)*SDL_NS_PER_MS);
            if( diff < frame_in_ns ) sys_sleep_precisens( frame_in_ns - dt );

            dt = tigrTime();
#else // accurate (beware of CPU usage)
            float target = (ZX_FPSMUL/100.0) * (ZX < 128 ? 50.08:50.01);

            // be nice to os
            sys_sleep(ZX_FPSMUL > 120 ? 1 : 5);
            // complete with shortest sleeps (yields) until we hit target fps
            dt = tigrTime();
            for( float target_fps = 1.f/(target+!target); dt < target_fps; ) {
                sys_yield();
                dt += tigrTime();
            }
#endif
        }

        // zxplayer quits loop early, so...
        // it wont be drawing UI
        // it wont be drawing game browser
        // it wont be processing cmdline/FN-keys commands
        // it wont be processing drag 'n drops
        if( ZX_PLAYER ) {
            window_update(app);
            continue;
        }

        // calc fps
        static int frames = 0; ++frames;
        static double time_now = 0; time_now += dt;
        if( time_now >= 1 ) { fps = frames / time_now; time_now = frames = 0; }

        // tape timer
        if(tape_playing()) timer += dt;

        static char dev_status[128] = "";
        if( DEV || ZX_DEBUG ) {
            char *ptr = dev_status;
            ptr += sprintf(ptr, "%dm%02ds ", (unsigned)(timer) / 60, (unsigned)(timer) % 60);
            ptr += sprintf(ptr, "%5.2ffps%s %d mem%s%d%d%d%d ", fps, do_runahead ? "!":"", ZX, rom_patches ? "!":"", GET_MAPPED_ROMBANK(), (page128&8?7:5), 2, page128&7);
            ptr += sprintf(ptr, "%02X%c%02X %04X ", page128, page128&32?'!':' ', page2a, PC(cpu));
            ptr += sprintf(ptr, "%c%c %4dHz ", "  +-"[tape_inserted()*2+tape_level()], toupper(tape_peek()), tape_hz);
        }

        // rec before/after UI,
        int rec();
        int gui(const char *);
        if( ZX_PRINTUI )
            gui(dev_status), rec();
        else
            rec(), gui(dev_status);

        // flush
        window_update(app);

        #define LOAD(ZX,TURBO,file) if(file) do { \
                boot(ZX, 0); if(TURBO || window_pressed(app,TK_CONTROL)) rom_patch_turbo(); \
                if( !loadfile(file,1) ) { \
                    if( !load_shader( file ) ) { \
                        if( is_folder(file) ) cmdkey = 'SCAN', cmdarg = file; \
                        else alert(va("cannot open '%s' file\n", file)); \
                    } \
                } \
            } while(0)

        // parse drag 'n drops. reload if needed
        for( char **list = tigrDropFiles(app,0,0); list; list = 0)
        for( int i = 0; list[i]; ++i ) {
            #if TESTS
            LOAD(48,1,list[i]);
            #else
            LOAD(ZX,ZX_TURBOROM,list[i]);
            #endif
        }

        // parse cmdline. reload if needed
        do_once
        for( int i = 1; i < __argc; ++i )
        if( __argv[i][0] != '-' ) {
            #if TESTS
            LOAD(48,1,__argv[i]);
            #else
            LOAD(ZX,ZX_TURBOROM,__argv[i]);
            #endif
        }
        else if( __argv[i][1] == 'v' ) cmdkey = 'HELP';


        // clear command
        int cmdkey_ = cmdkey; cmdkey = 0;
        const char *cmdarg_ = cmdarg; cmdarg = 0;

        // parse commands
        ZX_FASTCPU = 0;
        switch(cmdkey_) { default:
            break; case  'GAME':  if( ZX_BROWSER == 1 ? numgames : 1 ) active ^= 1;
            break; case  'MAX':   ZX_FASTCPU = 1; // fast-forward cpu (hold)

            break; case 'PLAY':  tape_play(1); // tape_play(!tape_playing()); /*if(!tape_inserted()) active ^= 1; else tape_play(!tape_playing());*/ // open browser if start_tape is requested but no tape has been ever inserted
            break; case 'PREV':  tape_prev();
            break; case 'NEXT':  tape_next();
            break; case 'STOP':  tape_stop();
            break; case 'FAST':  ZX_FASTTAPE ^= 1;

            break; case  'TV':   { static int mode = 0; do_once mode = ZX_CRT << 1 | ZX_RF; mode = (mode + 1) & 3; if(cmdarg_) mode = atoi(cmdarg_); ZX_RF = mode & 1; crt(ZX_CRT = !!(mode & 2) ); }
            break; case 'SAVE':   quicksave(0);
            break; case 'LOAD':   quickload(0);

            break; case 'PIC': cmdkey = 'PIC2', ZX_PRINTUI = !!window_pressed(app, TK_SHIFT); // resend screenshot cmd
            break; case 'REC': cmdkey = 'REC2', ZX_PRINTUI = !!window_pressed(app, TK_SHIFT); // resend recording cmd

            break; case 'TURB':  ZX_TURBOROM ^= 1; boot(ZX, 0|KEEP_MEDIA); if(!loadfile(last_load,1)) zxdb_reload(0); // toggle turborom and reload
            break; case 'BOOT':  reset(0|KEEP_MEDIA); if(!loadfile(last_load, 1)) zxdb_reload(0);
            break; case 'NMI':   if( pins & Z80_NMI ) pins &= ~Z80_NMI; else pins |= Z80_NMI; RZX_reset(); // @todo: verify
            break; case 'WIPE':  reset(0); ZXDB = zxdb_free(ZXDB); // clear media    KEEP_MEDIA/*|QUICK_RESET*/); // if(last_load) free(last_load), last_load = 0;
            break; case 'POKE':  pok_apply(cmdarg_);
            break; case 'MODE': {
                // cycle
                static int models[] = { 16, 48, 128, 200, 210, 300, 128|1 };
                static int find = 0;
                while(models[find] != (ZX | ZX_PENTAGON))
                find = (find + 1) % countof(models);
                find = (find + 1) % countof(models);
                int model = models[find];

                // override model if arg supplied
                if(cmdarg_) model = atoi(cmdarg_);

                // toggle model and reload
                ZX = model & 0x1FE;
                ZX_PENTAGON = model & 1;

                boot(ZX, 0|KEEP_MEDIA); if(!loadfile(last_load,1)) zxdb_reload(ZX); // toggle rom
                titlebar(last_load); // refresh titlebar

                // hack: force next cycle if something went wrong. @fixme: investigate why
                if( model & 1 ) if(!ZX_PENTAGON) ZX_PENTAGON = 1, rom_restore();
            }
            // cycle AY cores
            break; case 'AY':    { const int table[] = { 1,2,0,0 }; ZX_AY = table[ZX_AY]; if(cmdarg_) ZX_AY=atoi(cmdarg_); }
            break; case 'PAL':   ZX_PALETTE = (ZX_PALETTE+1)%countof(ZXPalettes); if(cmdarg_) ZX_PALETTE=atoi(cmdarg_);    palette_use(ZX_PALETTE, ZX_PALETTE ? 0 : 1);
            break; case 'JOY':   ZX_JOYSTICK = (ZX_JOYSTICK+1)%4;                 if(cmdarg_) ZX_JOYSTICK=atoi(cmdarg_);   if(ZX_JOYSTICK) ZX_GUNSTICK = 0;  // cycle Cursor/Kempston/Fuller,Sinclair1,Sinclair2
            break; case 'FIRE':  ZX_AUTOFIRE = (ZX_AUTOFIRE+1)%4;                 if(cmdarg_) ZX_AUTOFIRE=atoi(cmdarg_);
            break; case 'GUNS':  ZX_GUNSTICK ^= 1;                                if(cmdarg_) ZX_GUNSTICK=atoi(cmdarg_);   if(ZX_GUNSTICK) ZX_MOUSE = 0, ZX_JOYSTICK = 0; // cycle guns
            break; case 'MICE':  ZX_MOUSE ^= 1;                                   if(cmdarg_) ZX_MOUSE=atoi(cmdarg_);      if(ZX_MOUSE) ZX_GUNSTICK = 0;                  // cycle kempston mouse(s)
            break; case 'ULA':   ZX_ULAPLUS ^= 1;                                 if(cmdarg_) ZX_ULAPLUS=atoi(cmdarg_);    // cycle ulaplus
            break; case 'RUN':   ZX_RUNAHEAD ^= 1;                                if(cmdarg_) ZX_RUNAHEAD=atoi(cmdarg_);   // cycle runahead mode
            break; case 'DEV':   ZX_DEBUG ^= 1;                                   if(cmdarg_) ZX_DEBUG=atoi(cmdarg_);
            break; case 'KL':    ZX_KLMODE ^= 1;                                  if(cmdarg_) ZX_KLMODE=atoi(cmdarg_);     ZX_KLMODE_PATCH_NEEDED = 1;
            break; case 'TENG':  ZX_AUTOLOCALE ^=1;                               if(cmdarg_) ZX_AUTOLOCALE=atoi(cmdarg_); if(ZX_AUTOLOCALE) translate(mem, 0x4000*16, 'en');
            if(ZX_AUTOLOCALE) {
                ui_dialog_new(NULL);
                ui_dialog_option(0,"Game patched. Play, then go to main menu.\n\n",NULL,0,NULL);
                ui_dialog_option(1,"OK\n",NULL,0,NULL);
            }
            // break; case 'POLR':  mic_low ^= 64;                                if(cmdarg_) mic_low=atoi(cmdarg_);
            break; case 'KEYB':  issue2 ^= 1;                                     if(cmdarg_) issue2=atoi(cmdarg_)==2;
#if 0
                                    int do_reset = tape_playing() && q.debug && !strchr("uol", q.debug);
                                    if( do_reset ) {
                                        reset(KEEP_MEDIA), loadfile(last_load,1);
                                    }
#endif
            break; case 'CPU':   { const float table[] = { [50]=60,[60]=100,[100]=120,[120]=150,[150]=200,[200]=400,[400]=50 };
                                 ZX_FPSMUL = table[(int)(ZX_FPSMUL)]; if(cmdarg_) ZX_FPSMUL=atoi(cmdarg_); }

            break; case 'HELP':  help();

            break; case 'SCAN':  for( const char *f = cmdarg_ ? cmdarg_ : app_selectfolder("Select games folder"); f ; f = 0 )
                                    rescan( f ), active = !!numgames;

            break; case 'DEVT': ZX_DEVTOOLS ^= 1;

            break; case 'LINK':
                visit(cmdarg_);

            break; case 'PAD0': { // setup remap
                memcpy(ZX_PAD_, ZX_PAD, sizeof(ZX_PAD_));

                ui_dialog_new("- Gamepad to joystick/keyboard bindings -");

    #if 1
                ui_dialog_option(0,"⭠ ⭢ ⭡ ⭣ \4A \2B \5X \6Y \7LB RB LT RT LS RS Bk St\n",NULL,0,0);
                ui_dialog_option(2,"▁ . . . . . . . .. .. .. .. .. .. .. ..\n",NULL,0,0);
    #else
                ui_dialog_option(0,"   ⭠ ⭢ ⭡ ⭣ \4A \2B \5X \6Y \7LB RB LT RT LS RS Bk St\n",NULL,0,0);
                ui_dialog_option(2,"   ▁ . . . . . . . .. .. .. .. .. .. .. ..\n",NULL,0,0);
                ui_dialog_option(2, "LT . . . . . . . . .. .. .. .. .. .. .. ..\n",NULL,0,0);
                ui_dialog_option(2, "RT . . . . . . . . .. .. .. .. .. .. .. ..\n",NULL,0,0);
    #endif

                ui_dialog_separator();
                ui_dialog_option(1,"Save\n",0,'PAD2',0);
                ui_dialog_option(1,"Cancel\n",0,0,0);

                cmdkey = 'PAD1';
            }

            break; case 'PAD1': // tick remap
            {
                cmdkey = 'PAD1'; // repeat

                // scan keyboard
                int vk = -1;
                char *keys = tigrKeys(app);
                for( int i = 0; i < 256; ++i ) {
                    if( keys[i] > keys[i+256] ) {
                        vk = i;
                        break;
                    }
                }
                // scan option. @fixme: this is hacky
                int option = -1;
                for( int i = 0; i < num_options; ++i) {
                    if( options[i].flags & 2 ) {
                        option = i;
                        break;
                    }
                }
                //
                static const char *keysicons[256] = {0}, **keysallowed = keysicons;
                do_once {
                    static char ascii[256][2] = {0};
                    for( int i = 32; i < 128; ++i ) {
                        ascii[i][0] = i;
                        keysicons[i] = ascii[i];
                    }
                    keysicons[TK_SPACE] = "□";
                    keysicons[TK_LEFT] = "🮤";
                    keysicons[TK_RIGHT] = "🮥";
                    keysicons[TK_UP] = "🮧";
                    keysicons[TK_DOWN] = "🮦";
                    keysicons[TK_TAB] = "";
                //    TK_PAD0=128,TK_PAD1,TK_PAD2,TK_PAD3,TK_PAD4,TK_PAD5,TK_PAD6,TK_PAD7,TK_PAD8,TK_PAD9,
                //    TK_PADMUL,TK_PADADD,TK_PADENTER,TK_PADSUB,TK_PADDOT,TK_PADDIV,
                //    TK_F1,TK_F2,TK_F3,TK_F4,TK_F5,TK_F6,TK_F7,TK_F8,TK_F9,TK_F10,TK_F11,TK_F12,
                //    TK_BACKSPACE,TK_TAB,TK_RETURN,TK_SHIFT,TK_CONTROL,TK_ALT,TK_PAUSE,TK_CAPSLOCK,
                //    TK_ESCAPE,TK_SPACE,TK_PAGEUP,TK_PAGEDN,TK_END,TK_HOME,TK_LEFT,TK_UP,TK_RIGHT,TK_DOWN,
                //    TK_INSERT,TK_DELETE,TK_LWIN,TK_RWIN,TK_NUMLOCK,TK_SCROLL,TK_LSHIFT,TK_RSHIFT,
                //    TK_LCONTROL,TK_RCONTROL,TK_LALT,TK_RALT,TK_SEMICOLON,TK_EQUALS,TK_COMMA,TK_MINUS,
                //    TK_DOT,TK_SLASH,TK_BACKTICK,TK_LSQUARE,TK_BACKSLASH,TK_RSQUARE,TK_TICK
                //    ,TK_PRINT //< @r-lyeh
                }

                // num_buttons
                enum { _12 = countof(ZX_PAD_) };

                // apply (1x8 chars + 2x4 chars)
                if( option >= 0 && vk >= 0 && (vk == TK_BACKSPACE || keysallowed[vk]) ) {

                    // add key to mappings. terminate if no more room.
                    int slot = 0; while(ZX_PAD_[slot] && slot < _12) slot++;
                    if( slot < _12 ) ZX_PAD_[slot] = vk;

                    // delete last mapping if requested
                    if( vk == TK_BACKSPACE ) {
                        if(slot < _12 ) ZX_PAD_[slot] = 0;
                        if(slot > 0) ZX_PAD_[slot-1] = 0;
                    }
                }

                if( window_longpress(app,TK_BACKSPACE) )
                    memset(ZX_PAD_, 0, sizeof(ZX_PAD_));

                if( option >= 0 )
                {
                    // redraw mappings
                    const char *cursor = "▁";
                    char buf[256], *ptr = buf;
                    for( int i = 0; i < _12; ++i ) {
                        if( ZX_PAD_[i] ) {
                            if( i < 8 )
                            ptr += sprintf(ptr, " %s", keysicons[ZX_PAD_[i]]);
                            else
                            ptr += sprintf(ptr, " %s.", keysicons[ZX_PAD_[i]]);
                        } else {
                            if( i < 8 )
                            ptr += sprintf(ptr, " %s", cursor);
                            else
                            ptr += sprintf(ptr, " %s.", cursor);

                            cursor = ".";
                        }
                    }

                    (void)REALLOC((void*)options[option].text, 0);
                    options[option].text = STRDUP(buf+1);
                }
            }

            break; case 'PAD2': // apply remap
                memcpy(ZX_PAD, ZX_PAD_, sizeof(ZX_PAD_));

        }

    // shutdown zxdb browser if user is closing window app. reasoning for this:
    // next z80_opdone() will never finish because we dont tick z80 when zxdb browser is enabled
    active *= window_alive(app);

    #if NEWCORE
    // ensure there is no pending opcode before exiting main loop: spectral.sav would be corrupt otherwise.
    // also, do not loop indefinitely on invalid DI+HALT combo, which we use in our .SCR viewer.
    // update: moved logic that bypasses z80_opdone(&cpu) in HALT state. rewritten as a forced/benign ZX_Key(ZX_SHIFT) operation; see: input() function.
    // printf("%d %018llx %d %d %d\n", z80_opdone(&cpu), cpu.pins, cpu.step, IFF1(cpu), IFF2(cpu));
    } while( z80_opdone(&cpu) ? 0 : 1 ); // (cpu.pins & Z80_HALT) && (cpu.step == 1) ? 0 : 1 ); // (cpu.pins & Z80_HALT) && (cpu.step == 1 || cpu.step==_z80_special_optable[4] || cpu.step==_z80_special_optable[5]) ? 0 : 1 );
    #endif

    } while( window_alive(app) );

    // zxplayer does not save state
    if(!ZX_PLAYER)

    {
        // export config
        save_config();

        // export state
        if(medias) media[0].pos = voc_pos / (double)(voc_len+!voc_len);
        for( FILE *state = fopen("Spectral.sav","wb"); state; fclose(state), state = 0) {
            if( !export_state(state) )
                alert("Error exporting state");
        }
    }

    window_close(app);
    return 0;
}

void on_cmd(unsigned key, const char *arg) {
    cmdkey = key;
    cmdarg = arg;
}

int gui(const char *status) {
    if( DEV ) {
        float x = 0.5, y = 25.5;
        ui_print(ui,  x*11, y*11, ui_colors, status);
    }

    if( ZX_DEBUG ) {
        tigrClear(dbg, tigrRGBA(0,0,0,128));

        float x = 0.5, y = 2;

        ui_print(dbg, x*11, y*11, ui_colors, status), y += 1.5;

        ui_print(dbg, x*11, y*11, ui_colors, regs(0)), y += 5;

        ui_print(dbg, x*11, y*11, ui_colors, dis(PC(cpu), 22)), y += 22;
    }

    // stats & debug
    draw_ui();

    if( ZX_DEBUG )
    tigrBlitAlpha(app, dbg, 0,0, 0,0, _320,_240, 1.0f);

    // flush ui
    ui_frame_end();

    // draw ui on top
    tigrBlitAlpha(app, ui, 0,0, 0,0, _320,_240, 1.0f);

    // draw overlay on top
    if( do_overlay ) {
        static int x0 = 0, y0 = 0; static int x = 0, y = 0;
        static struct mouse prev = {0};
        struct mouse now = mouse();
        if( now.lb && !prev.lb ) x0 = now.x, y0 = now.y;
        if( now.lb ) x = (now.x - x0), y = (now.y - y0);
        else x *= 0.95, y *= 0.95;
        prev = now;

        tigrBlitAlpha(app, overlay, x,y, 0,0, _320,_240, 1.0f);
    }

    // render dialog modal on top
    if( ui_dialog_render(dialog) ) {
        // composite result
        tigrBlitAlpha(app, dialog, 0,0, 0,0, _320,_240, 1.0f);
    }

    // test gamepad bindings
    if( DEV ) {
        static int gx = _320/2, gy = _240*0.75, bt = 0;
        if( ZX_PAD[1] ) if( tigrKeyHeld(app, ZX_PAD[1]) ) gx -= 1;
        if( ZX_PAD[2] ) if( tigrKeyHeld(app, ZX_PAD[2]) ) gx += 1;
        if( ZX_PAD[3] ) if( tigrKeyHeld(app, ZX_PAD[3]) ) gy -= 1;
        if( ZX_PAD[4] ) if( tigrKeyHeld(app, ZX_PAD[4]) ) gy += 1;
        if( ZX_PAD[5] ) bt= tigrKeyHeld(app, ZX_PAD[5]);
        ui_print(app, gx % _320, gy % _240, ui_colors, bt ? "" : "");
    }

    return 1;
}

int rec() {
    // screenshots
    if( cmdkey == 'PIC2' ) {
        screenshot( window_title(app, NULL) );
        play('cam', 1);
    }
    // videos
    if( cmdkey == 'REC2' ) {
        /**/ if( record_active() ) record_stop(), play('moto', 0);
        else switch( record_start(va("%s-%04x.mp4", window_title(app, NULL), file_counter(SLOT_MP4)), app->w, app->h, 50) ) {
                default: alert("Cannot record video");
                break; case 2: play('moto', 1/*-1*/);
                break; case 1: alert("FFMPEG" ifdef(win32,".exe","") " not " ifdef(win32,"found","installed") ".\nUsing suboptimal MPEG1 encoder.\n"); play('moto', 1/*-1*/);
        }
    }
    // video
    if( record_active() ) {
        record_frame( app->pix, app->w, app->h );
    }
    return 1;
}
