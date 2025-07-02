#include <stdio.h>

int dummy1(const char *s) { return 1; }
int dummy0(const char *fmt, ...) { return 0; }

#if !DEV // disable console logging in release builds (needed for linux/osx targets)
int (*putx)(const char *t) = dummy1;
int (*printfx)(const char *fmt, ...) = dummy0;
#else
int (*putx)(const char *t) = puts;
int (*printfx)(const char *fmt, ...) = printf;
#endif

static void loggers_(int m) {
    if(m) {
        putx = puts;
        printfx = printf;
    } else {
        putx = dummy1;
        printfx = dummy0;
    }
}
#define puts putx
#define printf printfx

void loggers(int m);

#define ui_alert(title, ...) do { \
    ui_dialog_new(NULL); \
    ui_dialog_option(0,title,NULL, 0,NULL ); \
    ui_dialog_separator(), ui_dialog_separator(); \
    ui_dialog_option(0,va("<" __VA_ARGS__),NULL, 0,NULL ); \
    ui_dialog_separator(), ui_dialog_separator(), ui_dialog_separator(); \
    ui_dialog_ok(); \
} while(0)

// # build (windows)
// cl app.c /FeSpectral.exe /O2 /MT /DNDEBUG=3 /GL /GF /arch:AVX
//
// # build (linux, debian)
// sudo apt-get install mesa-common-dev libx11-dev gcc libgl1-mesa-dev libasound2-dev
// gcc app.c -o Spectral -O3 -DNDEBUG=3 -Wno-unused-result -Wno-format -Wno-multichar -lm -ldl -lX11 -lGL -lasound -lpthread
//
// # done
// cpu, ula, mem, rom, 48/128, key, joy, ula+, tap, ay/ym, beep, sna/128, fps, tzx, if2, zip, rf, menu, kms, z80, scr,
// key2/3, +2a/+3, fdc, dsk, autotape, gui, KL modes, load "" code, +3 fdc sounds, +3 speedlock, issue 2/3,
// pentagon, trdos, trdos (boot), translate game menus, 25/30/50/60 hz, game2exe,
// zxdb, custom tiny zxdb fmt, embedded zxdb, zxdb cache, zxdb download on demand, zxdb gallery
// ay player, pzx, rzx (wip), redefineable FN keys, mpg recording, mp4 recording, nmi, zx palettes,
// gamepads, gamepad bindings, turbosound (turboAY), autofire, alt z80 rates, media selector,
// border effects, border overscan, rainbow graphics, multicolor, HAL10H8 bugs, zoom x1..x4,
// lenslok, resizeable, ultrawide ula, host keyboard, custom shaders, screenmate, tape counter,
// stereo ABC/ACB, lobby chat,
// glue sequential tzx/taps in zips (side A) -> side 1 etc)
// sequential tzx/taps/dsks do not reset model
// scan folder if dropped or supplied via cmdline

// @todo:
// [ ] X<3<3<3
// [ ] bind all commands to cmdline: spectral -tv:0 --ULA=1 AY2
// [ ] try duimon's bezels
// [ ] audio sn76489
// [ ] audio filters
// [ ] read mags: concatenate all pags, and invoke external pdf viewer since our window resolution forbids reading hq text
// [ ] phantom typist to autoload some .dsks: rex, after the war re-imagined
// [ ] pok: mask menu + undo
// [ ] zx: multiface1, multiface128/genie128, multiface3
// [ ] autofps: exolon/alien8 @ 50fps
// [x] overlay: optimize: clear & redraw only if mouse == mouse_prev
// [ ] snow: Robocop 3, iLogicAll, Fantasy World Dizzy, Astro Marine Corps, Vectron... https://spectrumcomputing.co.uk/forums/viewtopic.php?p=114970&sid=54c80b833347da3e9f5398d4e9e0a0c5#p114970
// [ ] coop: ghost mode (single player vs ghost) time-based games: outrun, enduro
// [ ] disasm: breakpoint on a source code expression with wildcards. ie, break on "HALT\n*\nJ*\n"
// [ ] beeper: 10khz @dmitryurbanovich4748 "The music sounds harsh only because it was never supposed to be played at such high fidelity. It was expected to be played through low-cost amplifier and a speaker, which can't produce anything above 10kHz. Also, since it is only "full on" and "off", it caused all sorts of subtle distortion in the amplifier, so it actually sounded quite warm and pleasant (in comparison to what we hear in your video)."
// [ ] media: load romtrap $559
// [ ] media: push/pop zx state for thumbnails and miniatures
// [x] zxdb: emit warning on failed game downloads. or, do not proceed into boot() at least
// [ ] tape buttons
// [ ] tape 'tape' wav
// [ ] tape idea: add initial leading blank silence
// [ ] tape idea: do not add mega silence if "side A/1" is first block (HuntForRedOctober, italy1990winnersedition)
// [ ] tape idea: remove most of extremely large ending silences (krakatoa)
// [ ] tape idea: eat contiguous silences while tape polarity does not change and autoplay is on (welcometohollywood)
// [ ] try 48k syntax highlighting: https://github.com/reclaimed/prettybasic
// [ ] gallery: search should use our textinput widget
// [ ] gallery: local favourites not showing up in fav tab
// [ ] gallery: find tab misses key shortcuts, bookmark/issue icons and thumbnails mode, and filtering
// [ ] gallery: normalize zxdb/local tape numbers
// 
//  +-------------- game title: positive for zxdb ids, negative for local file ids in current folder
//  |  +----------- zxdb: 0 or id in case of multiple downloads. local: 0 or id in case of multiple files in zip
//  |  |   +------- zxdb: 0 or id in case of multiple files. local: 0 or id in case of multiple basic programs
//  |  |   |   +--- zxdb: 0 or id in case of multiple basic programs
//  |  |   |   |
// #ID#SEQ#BIN#BAS@MEDIASEEK
// 
// #zxdbID#downloadID#diskID#basID
// #zxdbID#downloadID#tapeID#basID
// 
// #localID#fileID#basID etc

// @fixme:
// [ ] zxdb: afterburner/arkanoid: cheats
// [ ] mp3: @leak
// [ ] .sav: mask3.sav, cybernoid2.sav, jack2.sav
// [ ] .sav: 2bits off for tapes? (close the app while attribs for cauldron2 screen$ being loaded. restart the emulator; tape_ticks/ticks?)
// [x] ui_print(): dims not very accurate when ui_monospaced==0. see chasehq2 instructions boundings
// [x] DEV remove beep during .tzx debugprints to console - Twister(zxdb), LaAbadiaDelCrimen (turbo), AndyCapp(zxdb)

// @fixme
// [ ] la abadia del crimen > maps
// [ ] el cartero > Libreria_de_Software_Spectrum_issue_12 > B-4
// [ ] contention+pzx: lonewolf128k.pzx
// [ ] flashing console window while opening https links (windows, release builds), probably recording too (popen)
// [ ] load snowhold.tap, somehow I==0x40 can last after reset/app restart
// [ ] dsk sideB : spaceharrier2 and myth cases
// [ ] tzx sideB : alien syndrome (Dro), alien syndrome (RAD)
// [ ] pzx/csw/gdb + turborom
// [ ] TTRacer WITHOUT turborom
// [ ] combatschool.ay
// [ ] robocop.ay + oscilloscopes in AY2 mode. space harrier 2 is fine (??)
// [ ] p128 bottom line (128 menu)
// [ ] lower bottom megashock/
// [ ] linux polyfill (does not work yet)
// [ ] contention: gauntlet3.dsk
// [ ] floating: arkanoid way too fast, sidewize too slow and/or blinks
// [ ] floating: floatspy (all models need +1 TS; 128 also needs +2TS because of bonanzadsk)
// [ ] dsk/corrupt vram: cesare plus3.dsk, italia90.dsk, alien storm.dsk, gng.dsk (paging bug?)
// [*] lightgun: billy the kid, rookie #4239, space smugglers
// [ ] audio: after 3 mins of play, user got white noise (jetpac)
// [ ] gallery: wordwrap needed when thumbnail names are too large: "Sherlock Holmes: The Lamberley Mystery (1990)(Zenobi Software)"
// [ ] mouse: wheel linux (@juntelart)
// [ ] mouse: wheel inverted (@hexaee, @korb)
// [x] exploding fist+
// [x] turborom + bleepload (sentinel, sidewize, crosswize, jaws)
// [x] too much audio latency?
// [x] 48irons.trd
// [x] p128 .sna
// [x] KLmode broken
// [x] zxdb platoon.tap.zip with .txt file in zipentry #0
// [x] zxdb: do not infer zxdb model from local files with entries >1 hits. eg, jaws.tap
// [x] zxdb: improve 48/128 model detection
//     1st) read zxdb info (Elite: 48k/128k, so 128k), unless
//     2nd) read filename info (Elite48_2.tzx, so 48k), unless
//     3rd) read basic program "ELITE128", so 128k, unless
//     4th) re-selected model from UI
//     cases: plyuk.zip/plyuk128.bas, ringo.zip/ringo128.bas td128/td128.bas
//     cases: oldtower(48k).tap, oldtower(128k).tap, oldtower(pentagon).tap
//     cases: elite, saboteur2, cabal, narc, nigelmansell, lonewolf3, ...

// @timings
// [ ] overscan/border: sentinel (48), defenders of the earth (+3), dark star (hiscore), gyroscope II (?), super wonder boy (pause)
// [ ] contended mem/multicolor: venom mask3, shock megademo, MDA, action force 2, old tower, hardy
// [ ] contended IO/border: venom mask3 (border), aquaplane, olympic games, vectron, jaws, platoon (trainer), treasure island 84
// [ ] contended mem: both venom mask3 + exolon main tunes require contended memory; tempo is wrong otherwise
// [ ] floating: arkanoid (original), cobra (original), sidewize, short circuit, emlyn hughes
// [ ] detection: borderbreak, overscan demo
// [ ] multicolor title: puzznic, eliminator, cabal, bad dream
//
// title (model)                | detection | contended mem:rainbow/multicolor | contended io:overscan/border effects | flick:floating bus
// defenders of the earth (+3)                                                                                ✓
//

// proposal for game mappings
//
//   [P]lay [K]empston [F]uller Sinclair[1] Sinclair[2] [C]ursor key[B]oard [_]others
//   +
//   KEY
//
// as many times as needed, then default assigned keys in following order:
//
//   UpDownLeftRightFire1Fire2Fire3... Pause Break Resume
//
// Example, atic atac: B1 K2 C3 P0 _456 ERQWTZ Space
// becomes,
//
// aticatac.joy:
// P0B1K2C3_456 ERQWTZ Space

#if 0
// mp3 (see: https://worldofspectrum.net/pub/sinclair/music/bonustracks/)
/sinclair/music/bonustracks/720Degrees.mp3.zip
/sinclair/music/bonustracks/Afterburner.mp3.zip
/sinclair/music/bonustracks/CP-M2.2-ModuloDeEntrenamientoSide1.mp3.zip
/sinclair/music/bonustracks/CP-M2.2-ModuloDeEntrenamientoSide2.mp3.zip
/sinclair/music/bonustracks/CZSpectrum-ProgramaDeIniciacion_SideA(LongVersion).mp3.zip
/sinclair/music/bonustracks/CZSpectrum-ProgramaDeIniciacion_SideA(ShortVersion).mp3.zip
/sinclair/music/bonustracks/CaptainAmericaInTheDoomTubeOfDrMegalomann_Resister-WhosCryingNow.mp3.zip
/sinclair/music/bonustracks/CarrierCommand.mp3.zip
/sinclair/music/bonustracks/ChallengeOfTheGobots.mp3.zip
/sinclair/music/bonustracks/ChallengeOfTheGobots_LoFi.mp3.zip
/sinclair/music/bonustracks/Confuzion.mp3.zip
/sinclair/music/bonustracks/Corruption.mp3.zip
/sinclair/music/bonustracks/CusterdsQuest.mp3.zip
/sinclair/music/bonustracks/DaleyThompsonsOlympicChallenge_TheChallenge.mp3.zip
/sinclair/music/bonustracks/DeseertIsalndFloppies(DanDefoe&TheCastaways).mp3.zip
/sinclair/music/bonustracks/DeusExMachinaPart1.mp3.zip
/sinclair/music/bonustracks/DeusExMachinaPart2.mp3.zip
/sinclair/music/bonustracks/DodgyGeezers.mp3.zip
/sinclair/music/bonustracks/DoomdarksRevenge_TheStory.mp3.zip
/sinclair/music/bonustracks/EnormousTurnipThe.mp3.zip
/sinclair/music/bonustracks/EveryonesAWally.mp3.zip
/sinclair/music/bonustracks/ExtricatorThe.mp3.zip
/sinclair/music/bonustracks/FiveLittleDucks(VijfKleineEendjes)(WoltersSoftware).mp3.zip
/sinclair/music/bonustracks/FiveLittleDucks_Side1.mp3.zip
/sinclair/music/bonustracks/HoldMyHand.mp3.zip
/sinclair/music/bonustracks/InsertCoins2_OriginalSoundtrack.mp3.zip
/sinclair/music/bonustracks/JamesBond007ActionPack.mp3.zip
/sinclair/music/bonustracks/KingdomOfKrellThe.mp3.zip
/sinclair/music/bonustracks/LOKOsoftsBestCrap_BonusTrack.mp3.zip
/sinclair/music/bonustracks/MorrisMeetsTheBikers.mp3.zip
/sinclair/music/bonustracks/MusicMachineThe-DemoSong-SideA.mp3.zip
/sinclair/music/bonustracks/MusicMachineThe-DemoSong-SideB.mp3.zip
/sinclair/music/bonustracks/OutRun.mp3.zip
/sinclair/music/bonustracks/PilandInternationalAnthemThe.mp3.zip
/sinclair/music/bonustracks/SU+QuicksilvaPresentTheMegaTape.mp3.zip
/sinclair/music/bonustracks/Sabrina.mp3.zip
/sinclair/music/bonustracks/SideArms_Resister-RightThisTime.mp3.zip
/sinclair/music/bonustracks/Slingshot.mp3.zip
/sinclair/music/bonustracks/SoftCuddly.mp3.zip
/sinclair/music/bonustracks/SpectrumStarterPack1.mp3.zip
/sinclair/music/bonustracks/Starglider2_ExtendedStereoSoundTrack.mp3.zip
/sinclair/music/bonustracks/SuperBowl.mp3.zip
/sinclair/music/bonustracks/ThompsonTwinsAdventureThe_Introduction.mp3.zip
/sinclair/music/bonustracks/TombOfSyrinx.mp3.zip
/sinclair/music/bonustracks/Trantor-TheLastStormtrooper.mp3.zip
/sinclair/music/bonustracks/UnDosTresRespondaOtraVez(3-2-1)(CheetahsoftLtd).mp3.zip
/sinclair/music/bonustracks/UnDosTresRespondaOtraVez.mp3.zip
/sinclair/music/bonustracks/UncleCroucho(LadyClairSinclive&ThePiman).mp3.zip
/sinclair/music/bonustracks/Valkyrie17.mp3.zip
/sinclair/music/bonustracks/Wriggler.mp3.zip
/sinclair/music/bonustracks/automata/01-AllayPallyWally.mp3.zip
/sinclair/music/bonustracks/automata/02-BankruptStock.mp3.zip
/sinclair/music/bonustracks/automata/03-Dartz.mp3.zip
/sinclair/music/bonustracks/automata/04-DeusExMachinaOuttakes.mp3.zip
/sinclair/music/bonustracks/automata/05-ExtremelySillySong.mp3.zip
/sinclair/music/bonustracks/automata/06-Fertilizer.mp3.zip
/sinclair/music/bonustracks/automata/07-IGotBugs.mp3.zip
/sinclair/music/bonustracks/automata/08-NewWheelsJohn.mp3.zip
/sinclair/music/bonustracks/automata/09-Olympimania.mp3.zip
/sinclair/music/bonustracks/automata/10-Pi-Balled.mp3.zip
/sinclair/music/bonustracks/automata/11-Pi-Eyed.mp3.zip
/sinclair/music/bonustracks/automata/12-PutTheCatOutMother.mp3.zip
/sinclair/music/bonustracks/automata/13-SomePeople.mp3.zip
/sinclair/music/bonustracks/automata/14-ThePiracyTango.mp3.zip
/sinclair/music/bonustracks/automata/15-Angel.mp3.zip
/sinclair/music/bonustracks/automata/16-BitOfACult.mp3.zip
/sinclair/music/bonustracks/automata/17-ComputerAlphabet.mp3.zip
/sinclair/music/bonustracks/automata/18-CountryMusac.mp3.zip
/sinclair/music/bonustracks/automata/19-CrummySong.mp3.zip
/sinclair/music/bonustracks/automata/20-DonkayHotay.mp3.zip
/sinclair/music/bonustracks/automata/21-Groucho.mp3.zip
/sinclair/music/bonustracks/automata/22-LeaderOfThePack.mp3.zip
/sinclair/music/bonustracks/automata/23-Pi-BalledBlues.mp3.zip
/sinclair/music/bonustracks/automata/24-Pimania.mp3.zip
/sinclair/music/bonustracks/automata/25-PompeyRock.mp3.zip
/sinclair/music/bonustracks/automata/26-VideoNasty.mp3.zip

// Missing
// [?] Adidas Championship Football
// [?] Frankie Goes to Hollywood
// [?] Hercules
// [?] Platoon
// [?] The Biz (Virgin)
// [?] Sheepwalk (Virgin)
// [?] Starfire (Virgin)
// [?] Flying Train (RandomRecords)
// [?] The Shaky Game (Olympic)
// [?] Titanic (R&R)
// [?] Power drift
// [ ] Soft Aid (Compilation)
// [ ] SU.92 [Nov.89] Megatape 21 
// [ ] XL1 (Compilation)
#endif

#define SPECTRAL "v1.13-WIP"

#ifndef DEV
#if NDEBUG >= 2
#define DEV 0
#else
#define DEV 1
#endif
#endif

#if DEV
#define ifdef_DEV(t,...)  t
#define ifndef_DEV(t,...) __VA_ARGS__
#else
#define ifdef_DEV(t,...)  __VA_ARGS__
#define ifndef_DEV(t,...) t
#endif

#define DEV_HIGHLIGHT_VRAM_CHANGES (DEV & 2)


// ref: http://www.zxdesign.info/vidparam.shtml
// https://worldofspectrum.org/faq/reference/48kreference.htm
// https://faqwiki.zxnet.co.uk/wiki/ULAplus
// https://foro.speccy.org/viewtopic.php?t=2319

// @todo:
// [ ] animated states
// [ ] zxdb on hover: show animated state if exists. show loading screen otherwise.
// [ ] auto-saves, then F11 to rewind. use bottom bar. when loading, ask: restart or continue?
// [ ] live coding disasm (like bonzomatic)
// [ ] convert side-b/mp3s into voc/pulses
// [ ] embed torrent server/client to mirror the WOS/ZXDB/NVG/Archive.org distros
//     http://www.kristenwidman.com/blog/33/how-to-write-a-bittorrent-client-part-1/
//     https://wiki.theory.org/BitTorrentSpecification
//     http://bittorrent.org/beps/bep_0003.html
//     https://github.com/willemt/yabtorrent
//     https://github.com/jech/dht
//
// idea: when stop-block is off
// - turn autoplay=off
// - wait silently for any key/joy button to be pressed, then turn autoplay=on again
//
// z80 profiler (auto)
// -           []
// -   []      [][]
// - [][][][][][][][][][] ...
// - 4000 4800 5000 5800  ...
// z80 profiler (instrumented)
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

// try
// rodolfo guerra's roms: https://sites.google.com/view/rodolfoguerra
// paul farrow's zx81 roms: http://www.fruitcake.plus.com/Sinclair/Interface2/Cartridges/Interface2_RC_New_ZX81.htm

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

// 384x304 to fit border_break.trd resolution
enum { _256 = 384, _255 = _256-1, _257 = _256+1, _32 = (_256-256)/2 };
enum { _192 = 304, _191 = _192-1, _193 = _192+1, _24 = (_192-192)/2 };

extern struct Tigr* app;
#define _319 (_320-1)
#define _320 (app->w)
#define _321 (_320+1)
#define _239 (_240-1)
#define _240 (app->h)
#define _241 (_240+1)

#include "3rd.h"
#include "emu.h"
#include "sys.h"
#include "zx.h"
#include "app.h"

enum { OVERLAY_ALPHA = 96 };
window *app, *canvas, *ui, *dbg, *overlay, *dialog;
int do_overlay;

#include "app_chat.h"
//#include "res/maps/maps.h" // extra gamemap infos from pavero's site

enum {
    SLOT_PNG,
    SLOT_MP4,
    SLOT_EXE,
};

uint16_t file_counter(unsigned slot) {
    static uint16_t counter[16] = {0}; ++counter[slot %= 16];
    return counter[slot++] - 1;
}

int screenshot(const char *filename, Tigr *app) {
    // escape invalid chars in filenames
    filename = va("%s", filename);
    for( const char *forbidden = "<>:\"/\\|?*"; *forbidden; ++forbidden ) {
        char mask[2] = "-"; mask[0] = *forbidden;
        replace((char*)filename, ":", "-");
    }

    uint16_t count = file_counter(SLOT_PNG);
    FILE *png = fopen8(va("%s-%04x.png", filename, count), "wb");
    int ok1 = tigrSaveImageFile(png, app); if(png) fclose(png);
    int ok2 = writefile(va("%s-%04x.scr", filename, count), VRAM, 6912);

    return ok1 && ok2;
}

int save_config() {
    if( ZX_PLAYER ) return 1;

    // convert absolute to relative. so they survive windows/unix cross-saves.
    if(ZX_MEDIA  && ZX_MEDIA[0] ) ZX_MEDIA  = strdup(relpath(ZX_MEDIA,cwd())); // @leak
    if(ZX_FOLDER && ZX_FOLDER[0]) ZX_FOLDER = strdup(relpath(ZX_FOLDER,cwd())); // @leak
    //if(ZX_SHADER && ZX_SHADER[0]) ZX_SHADER = strdup(relpath(ZX_SHADER,cwd())); // @leak

    mkdir(".Spectral", 0777);
    int errors = 0;
    for( FILE *fp = fopen(".Spectral/Spectral.ini", "wt"); fp; fclose(fp), fp = 0 ) {
        #define INI_SAVE_NUM(opt) errors += fprintf(fp, "%s=%d\n", #opt, opt) != 2;
        #define INI_SAVE_STR(opt) errors += fprintf(fp, "%s=%s\n", #opt, opt?opt:"") != 2;
        INI_OPTIONS_NUM(INI_SAVE_NUM)
        INI_OPTIONS_STR(INI_SAVE_STR)
    }
    return !errors;
}
int load_config() {
    int errors = 0;
    if( !ZX_PLAYER ) for( FILE *fp = fopen(".Spectral/Spectral.ini", "rt"); fp; fclose(fp), fp = 0 ) {
        while( !feof(fp) ) {
        int tmp = 0; char buf[128] = {0}; errors += fscanf(fp, "%[^=]=%d\n", buf, &tmp) != 2;
        #define INI_LOAD_NUM(opt) if( strcmpi(buf, #opt) == 0 ) opt = tmp; else 
        INI_OPTIONS_NUM(INI_LOAD_NUM) {}
        }
        rewind(fp);
        while( !feof(fp) ) {
        char key[128] = {0},val[128] = {0}; errors += !(fscanf(fp, " %127[^=]=%127[^\n]\n", key, val) >= 1);
        #define INI_LOAD_STR(opt) if( strcmpi(key, #opt) == 0 ) opt = STRDUP(val); else 
        INI_OPTIONS_STR(INI_LOAD_STR) {}
        }
        extern int cmdkey;
        extern const char* cmdarg;
        ZX_PENTAGON = ZX & 1; ZX &= ~1;
        if(ZX_FOLDER && ZX_FOLDER[0] > 32) cmdkey = 'SCAN', cmdarg = ZX_FOLDER;
    }
    {
        int size;
        // load custom shader. revert option if file cannot be loaded or does not exist
        unsigned char *fx = readfile(".Spectral/Spectral.fx", &size);
        ZX_SHADED *= fx && size ? load_shaderbin(fx, size), free(fx), 1 : 0;
        // load custom palette. revert option if file cannot be loaded or does not exist
        unsigned char *pal = readfile(".Spectral/Spectral.pal", &size);
        ZX_PALETTE *= pal && size ? pal_loadbin(pal, size), free(pal), 1 : 0;
    }
    {
        // convert relative to absolute paths
        if(ZX_MEDIA  && ZX_MEDIA[0] ) ZX_MEDIA  = strdup(abspath(ZX_MEDIA)); // @leak
        if(ZX_FOLDER && ZX_FOLDER[0]) ZX_FOLDER = strdup(abspath(ZX_FOLDER)); // @leak
        //if(ZX_SHADER && ZX_SHADER[0]) ZX_SHADER = strdup(abspath(ZX_SHADER)); // @leak
    }
    return !errors;
}


bool load_overlay_rgba(const rgba *bitmap, unsigned w, unsigned h) {
    extern window *overlay;

    if( bitmap ) {
        if(overlay) tigrFree(overlay);
        overlay = tigrBitmap(w,h);
        memcpy(overlay->pix, bitmap, w * h * 4);

        tigrRenderInitMap();
        return true;
    }

    return false;
}
bool load_overlay(const void *data, int len) {
    unsigned w, h;
    if( ui_image_info(data,len,&w,&h) ) {
        rgba *bitmap = ui_image(data,len,w,h,0);
        if( bitmap ) {
            bool rc = load_overlay_rgba(bitmap, w, h);
            free( bitmap );
            return rc;
        }
    }
    return false;
}

int desktop_w, desktop_h;
int app_wouldfit(int zoom) { // given some zoom level, check whether an upcoming window would fit desktop size
    if(zoom>4) zoom=8;
    return 1; // unlock all zoom levels unconditionally
    float scale = 0.90; // 0.90 to allow for some boundary clipping
    return (zoom * _256 * scale) <= desktop_w && (zoom * _192 * scale) <= desktop_h;
}
int app_resize() {
    if(ui) tigrFree(ui);            ui = tigrBitmap(_320, _240);
    if(dbg) tigrFree(dbg);          dbg = tigrBitmap(_320, _240);
    if(overlay) tigrFree(overlay);  overlay = tigrBitmap(_320, _240); do_overlay = 0;
    if(dialog) tigrFree(dialog);    dialog = tigrBitmap(_320, _240);
    tigrClear(app, tigrRGB(0,0,0));
    return 1;
}
void app_apply_shader() {
    if( ZX_SHADED && ZX_CRT ) ZX_SHADED = ZX_CRT = 0, alert("Both internal+external shaders are not yet supported.");
    if( ZX_SHADED ) ZX_SHADED = !!is_file(".Spectral/Spectral.fx");
    if( ZX_SHADED ) ZX_CRT = 0;
    if( ZX_CRT ) ZX_SHADED = 0;
    crt(ZX_SHADED ? shader : ZX_CRT ? shader_spectral : shader_tigr);
}
void app_browse_shader() {
    const char *file = app_loadfile();
    if( file && strendi(file, ".fx") ) {
        if( load_shader(file) ) {
            ZX_SHADED = 1;
            app_apply_shader();
        }
    }
}
int app_create(const char *title, int fs, int zoom) {
    if(zoom>4) zoom=8;
    tigrGetDesktop(&desktop_w, &desktop_h);
    while( !app_wouldfit(zoom) ) zoom--;
    zoom += !zoom;

    if( app && (zoom == ZX_ZOOM && fs == ZX_FULLSCREEN) ) {
        // do nothing
    } else {
        // force change view
        if(app) tigrFree(app);          app = tigrWindow(_256, _192, title, window_flags(fs, zoom));  // _32+256+_32, (_24+192+_24),

        // postfx. reloads user shader too
        #if 0
        if( ZX_SHADER && ZX_SHADER[0] )
            if( !load_shader( ZX_SHADER ) )
                free(ZX_SHADER), ZX_SHADER = 0, ZX_SHADED = 0;
        #endif

        app_apply_shader();

        //
        app_resize();
    }

    return !!app;
}


// x1 Beeper (+ x3 AY1 (+ x3 AY2)) oscilloscopes
void draw_audio(Tigr *ui, int CHANNELS, float *channels[7], int count) {
    // based on code by SteveJohn (Zen emulator)
    // MIT licensed, https://github.com/stevehjohn/Zen/blob/master/LICENSE
    // see: https://www.youtube.com/watch?v=EXFngtcADc8

    static dcr_t dc[1+3+3];
    do_once
        dcr_init(&dc[0], 44100, 100.0f), // 44.1KHz samplerate, 100Hz cutoff
        dcr_init(&dc[1], 44100, 100.0f), // 44.1KHz samplerate, 100Hz cutoff
        dcr_init(&dc[2], 44100, 100.0f), // 44.1KHz samplerate, 100Hz cutoff
        dcr_init(&dc[3], 44100, 100.0f), // 44.1KHz samplerate, 100Hz cutoff
        dcr_init(&dc[4], 44100, 100.0f), // 44.1KHz samplerate, 100Hz cutoff
        dcr_init(&dc[5], 44100, 100.0f), // 44.1KHz samplerate, 100Hz cutoff
        dcr_init(&dc[6], 44100, 100.0f); // 44.1KHz samplerate, 100Hz cutoff

    unsigned white = tigrRGBA(255,255,255,32).rgba; //ui_colors[1]; //RGB(255,0,0); //ui_colors[1];
    unsigned paper = ZXPalette[ZXBorderColor];

    for( int ch = 0; ch < CHANNELS; ++ch ) {
        float *sample = channels[ch];
        dcr_t* D = &dc[ch];

#if 1
        int beginX = 0;
#else
        // find max/min sample in stream
        // display wave so max is in middle of wave
        int maxp = 0; float maxv = -1000;
        int minp = 0; float minv = 1000;
        for( int p = 0; p < count; ++p ) {
            if( sample[p] > maxv ) maxv = sample[p], maxp = p;
            if( sample[p] < minv ) minv = sample[p], minv = p;
        }
        int beginX = maxp - (maxp-minp) / 2; 
        beginX += (beginX < 0) * count;
#endif

        enum { GAIN = 4*2*2 };
        int yOff = (ch+1) * (_240 / (CHANNELS+1));
        int y0 = yOff;

        for( int x = 0; x < _320; x++ ) {
            float v = sample[(beginX + x) % count];
#if 0
            v = dcr_filter(D, v);
#endif
            int y = yOff - v * GAIN;

            if( y >= 0 && y < _240 ) {
                unsigned *dst = (unsigned *)&ui->pix[x + y * _320];
                *dst++ = white;
            }

            // draw vertical line (y0..y)
            for( int inc = y0 < y ? 1 : -1; y0 != y; y0 += inc ) {
                if( y0 >= 0 && y0 < _240 ) {
                    unsigned *dst = (unsigned *)&ui->pix[x + y0 * _320];
                    *dst++ = white;
                }
            }
        }
    }
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
    "FAST:Toggle FastLoad\n0:off, 1:faster loading speed",
    "",
    "FIRE:Toggle Autofire\n0:off, 1:slow, 2:fast, 3:faster",
    PLAY_STR, //PAUSE_STR,
    "GAME:Game Browser",
    "\xB",
    "GUNS:Toggle Lightgun\n0:off, 1:lightgun+gunstick",
    "i",
    "HELP:About Dialog",
    JOYSTICK_STR,
    " JOY:Toggle Joysticks\n0:off, 1:cursor, 2:kempston, 3:custom",
    "k",
    "KEYB:Toggle Keyboard Issue 2/3\n2:earlier, 3:classic keyboard",
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
    " RUN:Toggle Run-Ahead\n0:off, 1:reduced input latency, 2:improved input latency",
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
    "  TV:Toggle TV mode\n0:off, 1:crt, 2:rf, 3:rf+crt",
    "U",
    " ULA:Toggle ULAplus\n0:off, 1:on, 2:on+ultrawide",
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
        char *hint = va("%s",info+5); if(strchr(hint,'\n')) *strchr(hint,'\n') = '\0';
        ui_dialog_option_ex(1,va("%c%s %.*s%s",in_use ? '\2' : tag.fourcc == ZX_FN[vk] ? '\5':'\7',icon,4,info, lf ? "\n":""),hint,vk_redefine2,va("%d %d",vk,tag.fourcc));
    }
    for( int i = COLUMNS-(num_commands%COLUMNS); i--; ) {
        ui_dialog_option_ex(0,i?"      ":"      \n",NULL,0,0);
    }
    ui_dialog_separator();
    ui_dialog_option_ex(1,"Reset\n",NULL,vk_redefine2,va("%d %d",vk,0));
    ui_dialog_option(1,"Cancel\n",NULL,0,0);
}
int vk_find( int vk ) {
    union { struct { uint8_t d,c,b,a; }; unsigned u; } x; x.u = bswap32(vk);
    x.a += ' ' * !x.a;
    x.b += ' ' * !x.b;
    x.c += ' ' * !x.c;
    x.d += ' ' * !x.d;

    for( int j = 0; j < countof(commands); ++j ) {
        if( memcmp(commands[j], &x.d, 4) == 0 ) {
            return j;
        }
    }
    return -1;
}
void input() {
    // keyboard
    ZXKeyboardClear();
    if(nextkey>0) ZXKey((nextkey&255)-1),nextkey=nextkey>>8;

    // gamepad
    const unsigned pad = gamepad(), pad2 = pad;

        char* keys = tigrKeys(app);

        // custom gamepad mappings
        enum { hit = 2 }; // use bitmask that works differently than any of tigr backends: win,osx,linux
        if( pad&0x0001 ) keys[ZX_PAD[ 0]]|=hit; else keys[ZX_PAD[ 0]] &= ~hit;
        if( pad&0x0002 ) keys[ZX_PAD[ 1]]|=hit; else keys[ZX_PAD[ 1]] &= ~hit;
        if( pad&0x0004 ) keys[ZX_PAD[ 2]]|=hit; else keys[ZX_PAD[ 2]] &= ~hit;
        if( pad&0x0008 ) keys[ZX_PAD[ 3]]|=hit; else keys[ZX_PAD[ 3]] &= ~hit;
        if( pad&0x0010 ) keys[ZX_PAD[ 4]]|=hit; else keys[ZX_PAD[ 4]] &= ~hit;
        if( pad&0x0020 ) keys[ZX_PAD[ 5]]|=hit; else keys[ZX_PAD[ 5]] &= ~hit;
        if( pad&0x0040 ) keys[ZX_PAD[ 6]]|=hit; else keys[ZX_PAD[ 6]] &= ~hit;
        if( pad&0x0080 ) keys[ZX_PAD[ 7]]|=hit; else keys[ZX_PAD[ 7]] &= ~hit;
        if( pad&0x0100 ) keys[ZX_PAD[ 8]]|=hit; else keys[ZX_PAD[ 8]] &= ~hit;
        if( pad&0x0200 ) keys[ZX_PAD[ 9]]|=hit; else keys[ZX_PAD[ 9]] &= ~hit;
        if( pad&0x0400 ) keys[ZX_PAD[10]]|=hit; else keys[ZX_PAD[10]] &= ~hit;
        if( pad&0x0800 ) keys[ZX_PAD[11]]|=hit; else keys[ZX_PAD[11]] &= ~hit;
        if( pad&0x1000 ) keys[ZX_PAD[12]]|=hit; else keys[ZX_PAD[12]] &= ~hit;
        if( pad&0x2000 ) keys[ZX_PAD[13]]|=hit; else keys[ZX_PAD[13]] &= ~hit;
        if( pad&0x4000 ) keys[ZX_PAD[14]]|=hit; else keys[ZX_PAD[14]] &= ~hit;
        if( pad&0x8000 ) keys[ZX_PAD[15]]|=hit; else keys[ZX_PAD[15]] &= ~hit;

    if( !ZX_DEVTOOLS ) {

        // autofire per joystick/mouse devices
        static byte frameJ = 0, frameM = 0;
        ZX_AUTOFIRE_ = 0;
        /**/ if(ZX_JOYSTICK_AUTOFIRE == 1) ZX_AUTOFIRE_ |= 1*!!(++frameJ & 0x08);
        else if(ZX_JOYSTICK_AUTOFIRE == 2) ZX_AUTOFIRE_ |= 1*!!(++frameJ & 0x04);
        else if(ZX_JOYSTICK_AUTOFIRE == 3) ZX_AUTOFIRE_ |= 1*!!(++frameJ & 0x02);
        /**/ if(ZX_MOUSE_AUTOFIRE    == 1) ZX_AUTOFIRE_ |= 2*!!(++frameM & 0x08);
        else if(ZX_MOUSE_AUTOFIRE    == 2) ZX_AUTOFIRE_ |= 2*!!(++frameM & 0x04);
        else if(ZX_MOUSE_AUTOFIRE    == 3) ZX_AUTOFIRE_ |= 2*!!(++frameM & 0x02);

        // joysticks
        int left = keys[TK_LEFT], right = keys[TK_RIGHT];
        int up   = keys[TK_UP],   down = keys[TK_DOWN];
        int fire = keys[TK_TAB];
        ZXJoysticks(up,down,left,right,fire ^ (fire && ZX_JOYSTICK_AUTOFIRE ? ZX_JOYSTICK_AUTOFIRE_BUTTON : 0));

        // keyboard
        #define KEYS(k) \
            k(0)k(1)k(2)k(3)k(4)k(5)k(6)k(7)k(8)k(9)\
            k(A)k(B)k(C)k(D)k(E)k(F)k(G)k(H)k(I)k(J)\
            k(K)k(L)k(M)k(N)k(O)k(P)k(Q)k(R)k(S)k(T)\
            k(U)k(V)k(W)k(X)k(Y)k(Z)
        #define K(x) if(keys[ 0[#x] ]) ZXKey(ZX_##x);

#if 1 // ZX_USE_HOST_KEYBOARD
        if( PC(cpu) < 0x4000 && is_basic_mode() ) {
            int chr = 0;

            // read clipboard (slowly)
            if( app_clipboard ) {
                static char *utf8 = 0;
                if( !utf8 ) utf8 = app_clipboard;

                static byte delay = 0; delay = (delay+1) & 7; if(!delay)
                while( !chr && *utf8 ) // bypass bad encodings
                    utf8 = (char *)extract_utf32(utf8, &chr);

                if( !*utf8 )
                    free(app_clipboard), app_clipboard = 0, utf8 = 0;
            }
            
            // read keyboard
            if( !chr ) {
                chr = key_char(0);
            }

            // special keys
            if( !chr ) {
                //if(keys[TK_ESCAPE])    ZXKey(ZX_SHIFT), ZXKey(ZX_1);
                if(keys[TK_BACKSPACE])   ZXKey(ZX_SHIFT), ZXKey(ZX_0);
                if(keys[TK_CAPSLOCK])    ZXKey(ZX_SHIFT), ZXKey(ZX_2);
                if(keys[TK_SHIFT])       ZXKey(ZX_SHIFT);
                if(keys[TK_CONTROL] && !keys[TK_ALT] ) { // CTRL only, no ALTGR
                    // emulate old zx keyboard with ctrl key
                    // ctrl-1 would edit line, ctrl-2 caps, ctrl-5678 cursor, ctrl-0 backspace, etc
                    // ctrl-p would send quotes, ctrl-j (-), ctrl-k (+), ctrl-l (=), etc.
                    int number = 0;
                    for( int i = 0; i <= 9; ++i )
                        if(keys['0'+i]) ZXKey(ZX_SHIFT), ZXKey(ZX_0+i), number = 1;
                    if( !number ) { ZXKey(ZX_SYMB); KEYS(K); }
                }
                else 
                if( pad ) {
                    KEYS(K); // for gamepads
                }
            }
            // patch unicode 8220/8221(“”) as \"
            if( chr >= 8220 && chr <= 8221 ) chr = '\"';
            // regular chars
            if( chr ) for( const char *sym = "0123456789abcdefghijklmnopqrstuvwxyz " ifdef(win32, "\n", "\r"), *peek = strchr(sym, chr); peek; peek = 0, chr = 0) {
                ZXKey(ZX_0 + (peek - sym));
            }
            if( chr ) for( const char *sym = "ABCDEFGHIJKLMNOPQRSTUVWXYZ", *peek = strchr(sym, chr); peek; peek = 0, chr = 0) {
                ZXKey(ZX_SHIFT), ZXKey(ZX_A + (peek - sym));
            }
            if( chr ) for( const char *sym = "_!@#$%&'()a*?defg^i-+=.,;\"q<s>u/w$y:", *peek = strchr(sym, chr); peek && !isalpha(*peek); peek = 0, chr = 0) {
                ZXKey(ZX_SYMB), ZXKey(ZX_0 + (peek - sym));
            }
            if( chr ) for( const char *sym = "`bc\\e{}hijklmno\xa9qrst]vwx[z", *peek = strchr(sym, chr); peek && !isalpha(*peek); peek = 0, chr = 0) {
                WRITE8(0x5C41, 1); // MODE, E cursor
                ZXKey(ZX_SYMB), ZXKey(ZX_A + (peek - sym));
            }
        }
        else
#endif
        {
            KEYS(K);
            if(keys[TK_SPACE])       ZXKey(ZX_SPACE);
            if(keys[TK_BACKSPACE])   ZXKey(ZX_SHIFT), ZXKey(ZX_0);
            if(keys[TK_RETURN])      ZXKey(ZX_ENTER);
            if(keys[TK_SHIFT])       ZXKey(ZX_SHIFT);
            if(keys[TK_CONTROL])     ZXKey(ZX_SYMB);
        }
    }

    // z80_opdone() returns 0 indefinitely while z80 is in halt state, often seen during BASIC sessions.
    // hack: force a benign keypress when user wants to close window; so z80_opdone() returns 1 hopefully.
    if( 0 && window_closed() ) {
        static int flip = 0; if( flip ^= 1 ) ZXKey(ZX_SHIFT);
    }

    // prepare command keys

    #define KEYBINDING(N) \
    if( key_longpress( TK_F##N) ) cmdkey = 'F00' + N; /*vk_redefine(N);*/ \
    else \
    if( key_released(  TK_F##N) ) if(!/*dialog_*/num_options) if((cmdkey = ZX_FN[N]) == 0) cmdkey = 'F00' + N;

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

void draw_redefinables() {
    ui_dialog_option(0,"<ESC" PLAY_STR "\f","Game browser", 0,NULL );
    for( int i = 1; i <= 12; ++i ) {
        int cmd = vk_find(ZX_FN[i]);
        const char *icon = cmd >= 0 ? commands[cmd - 1] : "";
        const char *text = cmd >= 0 ? commands[cmd] + 5 : NULL;
        if( text && strchr(text, '\n') ) text = va("%.*s", (int)(strchr(text, '\n') - text), text);
        ui_dialog_option(1,va("\b\bF%d%s",i,icon),text, 'F00'+i,NULL );
    }
}




void help() {
    int total = numok+numwarn+numerr;

    ui_dialog_new(NULL);
    ui_dialog_option(0,"<Spectral " SPECTRAL " (Public Domain).\n",NULL, 0,NULL );
    ui_dialog_option(1,"<https://spectral.zxe.io\n\n",NULL, 'LINK',"https://spectral.zxe.io" );
    ui_dialog_option(0,va("<ZXDB Library %s: %d entries\n",ZXDB_VERSION + countof("version"),zxdb_count()), NULL, 0,NULL );
    ui_dialog_option(0,va("<Local Library: %d games found (%d%%%% ✓)\n\n",numgames, 100 - (numerr * 100 / (total + !total))), NULL, 0,NULL );
    ui_dialog_option(0,"\n",NULL, 0,NULL );
    ui_dialog_ok();
}

void titlebar(const char *filename) {
    filename = filename ? filename : "";
    const char *basename = strrchr(filename, '/'); basename += !!basename;
    const char *basename_= strrchr(filename,'\\'); basename_+= !!basename_; if(basename_ > basename) basename = basename_;
    const char *title = basename ? basename : filename;
    const char *models[] = { [1]="16",[3]="48",[8]="128",[9]="P128",[12]="+2",[13]="+2A",[18]="+3" };
    const char *titlebar = ZX_PLAYER ? __argv[0] : va("Spectral%s %s%s%s", DEV ? " DEV" : "", models[(ZX/16)|ZX_PENTAGON], title[0] ? " - " : "", title);
    window_title(titlebar);

    if( ZX_TITLE != title )
    (free(ZX_TITLE), ZX_TITLE = strdup(title));
}

void draw_ui() {

    enum { UI_X = 0, UI_Y = 0 };

    // draw_compatibility_stats(ui);

    // ui
    int UI_LINE1 = (ZX_CRT ? 2 : 0); // first visible line

    struct mouse m = mouse();
    if( m.cursor == 0 ) {
        m.x = _320/2, m.y = _240/2; // ignore mouse; already clipped & hidden (in-game)
    }

    // ui animation
    enum { _60 = 58+8-4 };
    int hovering_border = !browser && !do_overlay && (m.x > (_320 - _60) || m.x < _60 );
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
        float chr_x = REMAP(smooth,0,1,-6,0.5) * 11, chr_y = REMAP(smooth,0,1,-4,2.5+1-0.5) * 11 + 2;
        int right = chr_x+8*4-4;
//        int bottom = chr_y+8*31.0-1;
        int bottom = _240-11*4.5+chr_y; // +8*31.0-1;

        ui_at(ui,chr_x-2,chr_y-11*2.5-2-1-2);
        //ui_at(ui, chr_x, chr_y);
        if( ZX_BROWSER == 2 ) {
            // ZXDB builds
            if( ui_click(browser ? "Resume" : "Pause", browser ? PLAY_STR : PAUSE_STR) ) cmdkey = 'GAME'; // browser ^= 1, ui_dialog_new(NULL);
        } else {
            // NOZXDB builds
            if( !numgames )
            if( ui_click("- Scan games folder -", "%c\n", FOLDER_CHR) ) cmdkey = 'SCAN';
            if(  numgames )
            if( ui_click(browser ? "Resume" : "Pause", browser ? PLAY_STR : PAUSE_STR) ) cmdkey = 'GAME'; // browser ^= 1, ui_dialog_new(NULL);
        }

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

        int len;

        if( ZXDB.ids[0] ) {

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

#if 0
        int starred = 0;
        int flagged = 0, color = flagged == 0 ? 7 : flagged == 3 ? 4 : flagged == 1 ? 6 : 2;
        if( ui_click("-Toggle compatibility flags-\n\2fail\7, \6warn\7, \4good", va("%c%s", color, flagged == 0 || flagged == 3 ? "✓":"╳")) )
            ;
        if( ui_click("-Toggle bookmark-", "\f\f\f\f" LOVE2_STR "\f\f" LOVE1_STR  "\f\f" LOVE0_STR "\n") )
            ;
#endif

        // zxdb
        if( ui_click(va("- %s -", ZXDB.ids[0]), "ZXDB"));
        {
        char *link = va("- Visit game page -\nhttps://spectrumcomputing.co.uk/entry/%s", ZXDB.ids[0]);
        if( ui_click(link, "\f\f\x19\n")) visit(link + countof("- Visit game page-\n"));
        }
        if( ui_click(va("- %s -", ZXDB.ids[2]), "Title\n"));
        if( ZXDB.ids[3][0] )
        if( ui_click(va("- %s -", ZXDB.ids[3]), "Alias\n"));
        if( ui_click(va("- %s -", ZXDB.ids[1]), "Year\n"));
        if( ui_click(va("- %s -", ZXDB.ids[4]), "Brand\n"));

        if( ui_click(va("- %s -", ZXDB.ids[7] + strspn(ZXDB.ids[7],"0123456789")), "Genre\n"));
        if( ZXDB.ids[6][0] && ui_click(va("- %s -", ZXDB.ids[6]), "Score\n"));

        if( ZXDB.authors[0] ) {
            if( ZXDB.authors[1] ) {
                char text[(1+9)*64] = {0}, *ptr = text;
                for( int i = 0; i < 9/*countof(ZXDB.authors)*/; ++i ) {
                    if( i == 0 )
                        ptr += snprintf(ptr, 64, "- Credits -\n");
                    if( ZXDB.authors[i] )
                        ptr += snprintf(ptr, 64, "%s%s\n",roles[ZXDB.authors[i][0]],ZXDB.authors[i]+1);
                }
                if( ui_click(text, "Team\n"));
            } else {
                int i = 0;
                if( ui_click(va("- %s%s -",roles[ZXDB.authors[i][0]],ZXDB.authors[i]+1), "Author\n"));
            }
        }

        if( ZXDB.ids[8] )
        if( ui_click(ZXDB.ids[8], "Tags\n"));
//          if( ui_click("- AY Sound -", "Feat.\n"));
//          if( ui_click("- Multicolour (Rainbow Graphics) -", "Feat.\n"));

        if( ui_click(va("- %s -", strchr(ZXDB.ids[5], ',')+1), "Model\n"));
        if( ui_click("- Select media -", "Media\n")) cmdkey = 'LIST', cmdarg = va("#%s", ZXDB.ids[0]);

        if( zxdb_url(ZXDB, "inlay") && ui_click("- Toggle Inlay -", "Inlay\n")) { // @todo: include scanned instructions and tape scan
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "inlay"), &len); data; free(data), data = 0 ) {
                do_overlay ^= 1;
                tigrClear(overlay, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                if( do_overlay ) {
                    load_overlay(data,len);
                }
            }
        }
        if( zxdb_url(ZXDB, "screen") && ui_click("- Toggle Screen$ -", "Screen\n")) {
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "screen"), &len); data; free(data), data = 0 ) {
                //if( len == 6912 ) memcpy(VRAM, data, len);
                if( len == 6912 ) {
                    rgba *tx = thumbnail(data, len, 1, false);

                    do_overlay ^= 1;
                    tigrClear(overlay, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                    if( do_overlay ) {
                        load_overlay_rgba(tx,256,192);
                        free(tx);
                    }
                }
            }
        }
        if( zxdb_url(ZXDB, "ay") && ui_click("- Toggle Music Tracks -", "Tunes\n")) {
            // use loading screen as a background
            int scrlen; char *scrdata = zxdb_download(ZXDB,zxdb_url(ZXDB, "screen"), &scrlen);

            // load & play tune
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "ay"), &len); data; free(data), data = 0 ) {
                zxdb_unpack2(&data, &len);
                loadbin(data, len, false);
            }

            if( scrlen == 6912 ) memcpy(VRAM, scrdata, scrlen);
            free(scrdata);
        }
        static int timer = 0; timer = (timer + 1) % 100;
        if( zxdb_url(ZXDB, "mp3") && ui_click("- Toggle Bonus Track(s) -", va("Bonus%s\n", !play_findvoice('mp3') ? "" : (timer > 50) * 2 + "\f\f\f\f♬"/*♪"*/))) {
            cmdkey = 'MP3L'; // mp3 list
        }
        if( zxdb_url(ZXDB, "poke") && ui_click("- Enable Cheats -", "Cheats\n") ) {
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "poke"), &len); data; free(data), data = 0 ) {
                zxdb_unpack2(&data, &len);
                loadbin(data, len, false);
            }
        }
        if( zxdb_url(ZXDB, "map") && ui_click("- Toggle Game Map -", "Maps\n")) {
            const char *url = zxdb_url(ZXDB, "png|map"); // check colorful png maps first
            if(!url) url = zxdb_url(ZXDB, "map"); // else try regular jpg scanned maps
            for( char *data = zxdb_download(ZXDB,url,&len); data; free(data), data = 0 ) {
                do_overlay ^= 1;
                tigrClear(overlay, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                if( do_overlay ) {
                    load_overlay(data, len);
                }
            }
        }
        if( zxdb_url(ZXDB, "instructions") && ui_click("- Toggle Instructions -", "Help\n")) { // @todo: word wrap maybe. see Afterburner for a good test case
            for( char *data = zxdb_download(ZXDB,zxdb_url(ZXDB, "instructions"), &len); data; free(data), data = 0 ) {

                // unpack if needed (see: IndianaJonesAndTheLastCrusade)
                zxdb_unpack2(&data, &len);

                do_overlay ^= 1;
                tigrClear(ui, !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,OVERLAY_ALPHA));
                if( do_overlay ) {
                    const char *text = as_utf8(replace(data, "\t", " "));

                    int dims = (ui_monospaced = 1, ui_print(NULL, 4,4, ui_colors, text));
                    int w = dims & 0xFFFF;
                    int h = dims >> 16;
                    w = w < _320 ? _320 : w + 16-(w%16);
                    h = h < _240 ? _240 : h + 16-(h%16);

                    tigrFree(overlay);
                    overlay = tigrBitmap(w, h);

                    (ui_monospaced = 1, ui_print(overlay, 4,4, ui_colors, text));

                    tigrRenderInitMap();
                }
            }
        }

        // mags reviews
        // netplay lobby
        }
    }

    // lmb: default mode
    static int lmb_prev = 0, lmb_then = 0, lmb_now = 0;
    lmb_prev = lmb_then;
    lmb_then = lmb_now;
    lmb_now = m.lb;
    int lmb_up = lmb_then > lmb_now;

    // rmb: expert mode
    static int rmb_prev = 0, rmb_then = 0, rmb_now = 0;
    rmb_prev = rmb_then;
    rmb_then = rmb_now;
    rmb_now = m.rb;
    int rmb_held = rmb_now, rmb_up = rmb_prev;

    int shift = key_pressed( TK_SHIFT);

    // right panel: emulator options
    if( 1 )
    {
        int chr_x = REMAP(smooth,0,1,_320+11,_320-6*11) + 0, chr_y = REMAP(smooth,0,1,-4,2.5) * 11;
        int right = chr_x+8*4-4;
        int bottom = _240-11*(4+0.5*tape_inserted())+chr_y; // +8*31.0-1;

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

        ui_at(ui,chr_x - 8,chr_y-11*2-2-1);
        ui_y++;
        if( ui_click("- Frames per second -\nClick to toggle max speed", "\b%s\f%d", bat, (int)ZX_FPS) ) cmdkey = 'MAX';
        ui_y--;

        ui_x = chr_x + 3 * 8;
        if( ui_click("- Screenshot -\n(Right-click captures UI)", "%c", SNAP_CHR) ) cmdkey = rmb_up ? 'PIC_':'PIC'; // send screenshot command
        if( ui_click("- VideoREC -\n(Right-click records UI)", "\f\f" )) cmdkey = rmb_up ? 'REC_':'REC';

        ///if( ui_press("- Full Throttle -\n(hold)", "%c\b\b\b%c\b\b\b%c%d\n\n", PLAY_CHR,PLAY_CHR,PLAY_CHR,(int)fps) ) cmdkey = 'MAX';

        ui_at(ui,chr_x - 4/*ui_x - 8*/,ui_y + 11*3);

        ui_y-=1*0;
          if( ui_click("- Reset -", "\f🗲\f"/**/) ) cmdkey = 'WIPE'; //'MODE', cmdarg = va("%d",ZX+ZX_PENTAGON);
//        if( ui_click("- Clear Medias -", "\f\f") ) cmdkey = 'WIPE';
//        if( ui_click(rmb_held*23+"- Magic button (NMI) -\0- Magic button (NMI) -\nGenerates a Non-Maskable Interrupt", "") ) cmdkey = 'NMI';
        ui_y+=1*0;

        const int zxmodes[] = {[0]=16,[1]=48,[2]=128,[3]=200,[4]=210,[5]=300,[6]=128|1,[16]=0,[48]=1,[128]=2,[200]=3,[210]=4,[300]=5,[128|1]=6};
        static int zx_prev1, zx_prev2; do_once zx_prev1 = zx_prev2 = zxmodes[ZX|ZX_PENTAGON];
        if( zx_prev1 != zx_prev2 ) if( !lmb_now ) cmdkey = 'MODE', cmdarg = va("%d", zxmodes[zx_prev2 = zx_prev1]);

        const int z80modes[] = {[0]=50,[1]=60,[2]=100,[3]=120,[4]=150,[5]=200,[6]=400,[50]=0,[60]=1,[100]=2,[120]=3,[150]=4,[200]=5,[400]=6};
        static int z80_prev1, z80_prev2; do_once z80_prev1 = z80_prev2 = z80modes[ZX_FPSMUL];
        if( z80_prev1 != z80_prev2 ) if( !lmb_now ) cmdkey = 'CPU', cmdarg = va("%d", z80modes[z80_prev2 = z80_prev1]);

        const char *models[] = { [1]=" 16K",[3]=" 48K",[8]="128K",[9]="P128",[12]=" +2",[13]=" +2A",[18]=" +3" };
        if( ui_click(rmb_held*17+"- Toggle Model -\0- Toggle Model -\n16, 48, 128, +2, +2A, +3, Pentagon\n", "%s%s\n\n",models[(ZX/16)|ZX_PENTAGON],ZX_ALTROMS ? "!":"")) if(rmb_up) cmdkey = 'MODE'; else
        {
            int mode = ZX + ZX_PENTAGON;
            ui_dialog_new("- Toggle Model -");

            ui_dialog_combo(1,"<ZX Spectrum \00516K|ZX Spectrum \00548K|ZX Spectrum \005128K|ZX Spectrum \005+2|ZX Spectrum \005+2A|ZX Spectrum \005+3|ZX \005Pentagon",NULL,&zx_prev1,0,6);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_combo(1,"<CPU speed \00550%%|CPU speed \00560%%|CPU speed \005100%%|CPU speed \005120%%|CPU speed \005150%%|CPU speed \005200%%|CPU speed \005400%%",NULL,&z80_prev1,0,6);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_combo(1,"<Input latency \5x1|Input latency \5x2|Input latency \5x3",NULL,&ZX_RUNAHEAD,0,2);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_separator();
            ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5NMI\f\f","- Generate a Non-Maskable Interrupt (Magic Button) -",'NMI',0);
            ui_dialog_ok();
        }

#if 0
        int tableHz[] = {[50]=0,[60]=1,[100]=2,[120]=3,[150]=4,[200]=5,[400]=6};
        if( ui_click(rmb_held*21+"- Toggle Z80 speed -\0- Toggle Z80 speed -\n0:25Hz, 1:30Hz, 2:50Hz, 3:60Hz, 4:75Hz, 5:7MHz, 6:14MHz", "🗲\f%d", tableHz[(int)ZX_FPSMUL]) ) if(rmb_up) cmdkey = 'CPU'; else
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
        if( ui_click(rmb_held*19+"- Toggle ULAplus -\0- Toggle ULAplus -\n0:off, 1:on, 2:on+ultrawide", "%c\f%d\n", ZX_ULAPLUS ? 'U':'u'/*CHIP_CHR '+'*/, ZX_ULAPLUS) ) if(rmb_up) cmdkey = 'ULA'; else
        {
            ui_dialog_new("- Toggle ULAplus -");
        }
#endif

        static int zoom_prev; do_once zoom_prev = ZX_ZOOM;
        if( zoom_prev != ZX_ZOOM && !lmb_now ) cmdkey = 'ZOOM', cmdarg = va("%d", zoom_prev = ZX_ZOOM), ZX_ZOOM = -1;

        if( ui_click(rmb_held*19+"- Toggle TV mode -\0- Toggle TV mode -\n0:off, 1:crt, 2:rf, 3:rf+crt, 4:ext", "▒\f%d", ZX_SHADED ? 4 : (ZX_RF << 1 | ZX_CRT)) ) if(rmb_up) cmdkey = 'TV'; else
        {
            ui_dialog_new("- Toggle TV mode -");

            ui_dialog_slider(1,"<Blur",NULL,&ZX_BLUR,0,100);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_slider(1,"<Bloom",NULL,&ZX_BLOOM,0,100);
            ui_dialog_separator();
            ui_dialog_separator();

#if 0
            int w, h, max_zoom = 1; tigrGetDesktop(&w, &h);
            for( int i = 1; i <= 5; ++i ) if( app_wouldfit(i) ) max_zoom = i;
            ui_dialog_slider(1,"<Zoom",NULL,&ZX_ZOOM,1,max_zoom); //x1,x2,x3,x4,x8
            ui_dialog_separator();
            ui_dialog_separator();
#else
            int w, h, max_zoom = 1; tigrGetDesktop(&w, &h);
            for( int i = 1; i <= 5; ++i ) if( app_wouldfit(i) ) max_zoom = i;
            ui_dialog_combo(1,"<Zoom \5x1|Zoom \5x2|Zoom \5x3|Zoom \5x4|Zoom \5x8",NULL,&ZX_ZOOM,1,max_zoom);
            ui_dialog_separator();
            ui_dialog_separator();
#endif

            ui_dialog_combo(1,"<ULA \5Classic|ULA+ \5Enhanced|ULA+ \5Ultrawide",NULL,&ZX_ULAPLUS,0,2);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_checkbox2cmd(&ZX_FULLSCREEN,"<Fullscreen\n\n","- Enable Fullscreen mode -",'FULL',NULL);

            ui_dialog_checkbox2(&ZX_CRT,"<Internal shader (CRT)\n","- Enable CRT mode -",(void*)app_apply_shader);
            ui_dialog_checkbox2(&ZX_SHADED,"<External shader ","- Use external shader -",(void*)app_apply_shader);
            ui_dialog_option(1|4,FOLDER_STR"\n\n","- Browse external .fx shader -",'SHAD',NULL);

            ui_dialog_checkbox2(&ZX_RF,"<RF mode (extra CPU cost)\n\n","- Enable RF mode -",(void*)app_apply_shader);

            ui_dialog_option(1|4," Palettes ","- Change palette -",'PAL0',NULL);
            ui_dialog_option(1|4,"Preset ","-Reset to default configuration-",'SETG',NULL);
            ui_dialog_option(1,"OK\n",NULL, 0,NULL);
        }
        ui_x += 8;
        if( ui_click(rmb_held*19+"- Toggle AY core -\0- Toggle AY core -\n0:off, 1:fast, 2:accurate", /*𝄞♬*/"♪\f%d\n",ZX_AY) ) if(rmb_up) cmdkey = 'AY'; else
        {
            ui_dialog_new("- Toggle AY core -");

            // @todo: Mute beep
            // @todo: Mute floppy disk

            ui_dialog_combo(1,"<\5Mute|Fast \5AY|Accurate \5YM","-Select AY configuration-",&ZX_AY,0,2);
            ui_dialog_separator();
            ui_dialog_separator();

            //ui_dialog_checkbox2cmd(&ZX_STEREO,"<Stereo\n\n","- Toggle stereo sound -",0,NULL);
            ui_dialog_combo(1,"<\5Mono|Western Stereo|Eastern Stereo","-Select AY Stereo configuration-",&ZX_STEREO,0,2);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_checkbox2(&ZX_WAVES,"Display waveforms\n\n","- Display audio waveforms -",NULL);

            ui_dialog_separator();
            ui_dialog_option(1,"OK\n",NULL, 0,NULL);
        }

        const char joyicon[256] = {
            //[64]='X',   // Kempston C [port 95]
            [32]='B',     // Kempston B [port 55]
            [16]='F',     // (F)uller
            [8]='2',      // Sinclair/Interface (2)
            [4]='1',      // Sinclair (1)
            [2]='K',      // (K)empston
            [1]='C',      // (C)ursor
            [0]='0',      // (0)Off
            [16+2+1]='J', // (J)oysticks mask: cursor+kempston+fuller
        };
        if( ui_click(rmb_held*21+"- Toggle Joysticks -\0- Toggle Joysticks -\n0:off, 1:sinclair1, 2:sinclair/interface2\nC:ursor, K:empston, J:fuller+cursor+kempston", "%c\f%c", JOYSTICK_CHR, ZX_JOYSTICK > 0xFF ? (ZX_JOYSTICK>>8) + 'a' : joyicon[ZX_JOYSTICK&0xFF] ? joyicon[ZX_JOYSTICK&0xFF] : '*')) if(rmb_up) cmdkey = 'JOY'; else
        {
            cmdkey = 'JOY0';
        }
        ui_x += 8;
        if( ui_click(rmb_held*17+"- Toggle Mouse -\0- Toggle Mouse -\n0:off, 1:kempston", "\x9\f%d\n", ZX_MOUSE) ) if(rmb_up) cmdkey = 'MICE'; else
        {
            ui_dialog_new("- Toggle Mouse -");

            ui_dialog_combo(1,"<" MOUSE_STR " Mouse \5Off|"  MOUSE_STR " Mouse \5Kempston|" MOUSE_STR " \5Lightgun+Gunstick",NULL,&ZX_MOUSE,0,2);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_combo(1,"<" FIRE_STR " Autofire \5Off|" FIRE_STR " Autofire \5slow|" FIRE_STR " Autofire \5fast|" FIRE_STR " Autofire \5faster","-Select mouse autofire rate-",&ZX_MOUSE_AUTOFIRE,0,3);
            ui_dialog_separator();
            ui_dialog_separator();

            ui_dialog_separator();
            ui_dialog_option(1,"OK\n",NULL, 0,NULL);
        }

        if( ui_click(rmb_held*31+"- Toggle 48-BASIC input mode -\0- Toggle 48-BASIC input mode -\nK:token based, L:letter based", "~%c~\f%d", ZX_KLMODE ? 'L' : 'K', ZX_KLMODE) ) if(rmb_up) cmdkey = 'KL'; else
        {
            ui_dialog_new("- Keyboard bindings and options -");

            draw_redefinables();
            ui_dialog_separator();
            ui_dialog_separator();

            //ui_dialog_option(1,( ZX_KLMODE)+"\5Tokens\n","Use traditional input mode",'KL',"0");
            //ui_dialog_option(1,(!ZX_KLMODE)+"\5Letters\n","Use modern input mode",'KL',"1");
            ui_dialog_checkbox2cmd(&ZX_KLMODE,"<Type letters in 48-BASIC mode\n","Whether to type tokens or letters in 48-BASIC",'KL',NULL);

            //ui_dialog_option(1,(!!issue2)+"\5Classic keyboard (issue 2)\n",NULL,'KEYB',"3");
            //ui_dialog_option(1,( !issue2)+"\5Earlier keyboard (issue 3)\n",NULL,'KEYB',"2");
            ui_dialog_checkbox2cmd(&issue2,"<Earlier keyboard type (issue 2)\n\n","Improves compatibility with a few older games",'KEYB',NULL);

            ui_dialog_option(0,"\n",NULL, 0,NULL );
            ui_dialog_option(1,1/*(!ZX_AUTOLOCALE)*/+"\5Paste ","Paste clipboard contents",'CLIP',"1");
            ui_dialog_ok();
        }
        ui_x += 8;
        if( ui_click("- Toggle Lenslok -\nFocus on the center of the encoded image", "𜲌\f%d\n", ZX_LENSLOK)) cmdkey = 'LENS';

        if( ui_click("- Toggle chat/lobby -", "%s\f%d", ZX_LOBBY && ZXFlashFlag ? "":"", ZX_LOBBY) ) if(rmb_up) cmdkey = 'CHAT'; else
        {
            cmdkey = 'CHAT';
        }
        ui_x += 8;
        if( ui_click(rmb_held*24+"- Other options -\0- Translate game menu -\n0:off, 1:poke game menu into English", "T\f%d\n", ZX_AUTOLOCALE)) if(rmb_up) cmdkey = 'TENG'; else
        {
            ui_dialog_new("- Other options -");
            ui_dialog_option(1,1/*(!ZX_AUTOLOCALE)*/+"\5Translate game menu\n\n","-Poke game menu into English-",'TENG',"1");
            ui_dialog_option(1|4,"\6" FOLDER_STR " \7Build standalone player from game\n\n","- Select .z80 game -",'MAKE',NULL);
            ui_dialog_checkbox2(&ZX_PAUSE,"<Paused while minimized\n\n","-Pause emulation when losing windows focus-",NULL);
            ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5Cancel\n",NULL,'TENG',"0");
        }

        if( ui_click(rmb_held*20+"- Toggle FastLoad -\0- Toggle FastMedia -\n0:off, 1:faster media loading", "\f%d", ZX_FASTTAPE )) if(rmb_up) cmdkey = 'FAST'; else
        {
            ui_dialog_new("- Toggle FastLoad -");
            /*
            ui_dialog_option(1,(!ZX_FASTTAPE)+"\5Faster loading speed\n",NULL,'FAST',"1");
            ui_dialog_option(1,( ZX_FASTTAPE)+"\5Normal loading speed\n",NULL,'FAST',"0");
            ui_dialog_option(1,(!ZX_TURBOROM)+"\5Turbo ROM .tap loader\n",NULL,'TURB',"1");
            ui_dialog_option(1,( ZX_TURBOROM)+"\5Compatible ROM loader\n",NULL,'TURB',"0");
            */
            /*
            ui_dialog_checkbox2cmd(&ZX_FASTTAPE,"<Faster loading speed\n","- Speed up during media loading -",'FAST',NULL);
            ui_dialog_checkbox2cmd(&ZX_TURBOROM,"<Turbo ROM .tap loader\n","- Patch Turbo ROM and .tap loader -",'TURB',NULL);
            */
            ui_dialog_checkbox2(&ZX_FASTTAPE,"<Max loading speed while loading\n","- Use 100% CPU while loading medias -",NULL);
            ui_dialog_checkbox2(&ZX_FLASHLOAD,"<FlashLoad standard .tap files\n","- Load tape files instantly where possible -",NULL);
            ui_dialog_checkbox2(&ZX_TURBOROM,"<Use TurboROM on standard tape blocks\n","- Convert standard tape blocks to Turbo -",NULL);

            ui_dialog_separator();

            // @todo: eject media + show tape counter
            if(ZX_MEDIA)
            ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5"EJECT_STR"\fSelect\f\f",NULL,'LIST',ZX_MEDIA);
            else
            ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5"EJECT_STR"\fEject\f\f",NULL,'WIPE',0);

            ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5OK\n",NULL,0,0);
        }
        ui_x += 8;
        if( ui_click(rmb_held*20+"- Toggle console -", ZXFlashFlag ? ">▂\n" : ">▁\n")) // if(rmb_up) cmdkey = 'TURB'; else
        {
            ZX_CONSOLE ^= 1;
            loggers(ZX_CONSOLE);
#if DEV
            //if(ZX_CONSOLE) cmdkey = 'DEVT';
#endif
        }

        if( ui_click(rmb_held*14+"- Quicksave -\0- Quick savegame -\n0..9: Slot", DISK_STR "\f%d", 0) ) if(rmb_up) cmdkey = 'SAVE'; else
        {
            cmdkey = 'SAVE';
        }
        ui_x += 8;
        if( ui_click(rmb_held*14+"- Quickplay -\0- Quick loadgame -\n0..9: Slot", PLAY_STR "\f%d\n", 0) ) if(rmb_up) cmdkey = 'LOAD'; else
        {
            cmdkey = 'LOAD';
        }

        //if( ui_click("- Toggle TapePolarity -", "%c\f%d\n", mic_low ? '+':'-', !mic_low) ) cmdkey = 'POLR';

        ui_at(ui,chr_x - 8,bottom+1);
        if( ui_click(NULL, "i") ) cmdkey = 'HELP';

        // debug
        ui_at(ui,right,bottom);
        if( ui_click("- Debug -", "") ) cmdkey = 'DEV'; // send disassemble command
    }

    if( record_enabled() && ZXFlashFlag ) {
        ui_print(ui, _320-8*1-3, 0*11+4-2+2+1-2-1, ui_colors, "\2\7" );
    }

#if 1
    if( browser ) {
        ui_at(ui, 1*11-4+2-8+2, 0*11+4-2);
        if( ui_click(browser ? "Resume" : "Pause", browser ? PLAY_STR : PAUSE_STR) ) cmdkey = 'GAME'; // browser ^= 1, ui_dialog_new(NULL);
    }
#endif

    // tape counter & controls
    if(!browser && !do_overlay && tape_inserted() && tape_tellf() <= 1. && mic_on ) {
        // tape controls
        int tapex = _320/2-(8+1)*1.5;
        int tapey = _240*13/14;
        if( (ui_at(ui,tapex-8*2,tapey), ui_click(NULL, tape_playing() ? "■" : PLAY_STR)) )
            cmdkey = tape_playing() ? 'STOP' : 'PLAY';
            //if( ui_click(NULL, "\xf\b\b\b\xf") ) cmdkey = 'PREV';
            //if( ui_click(NULL, "%c\b\b\b%c", PLAY_CHR, PLAY_CHR) ) cmdkey = 'NEXT';
            //if( ui_click(NULL, "%d", autoplay)) ZX_AUTOPLAY ^= 1;
            //if( ui_click(NULL, "%d", autostop)) ZX_AUTOSTOP ^= 1;
        // tape counter
        {
            // animates count to zero once clicked
            static int tape_anim = 0, tape_target;
            if( tape_anim > 0 )
                tape_counter = --tape_anim > 0 ? tape_target * 0.5 + tape_counter * 0.5 : tape_target;

            // scale down so it advances a digit per second at 50hz approx. handle negatives too
            const double _1e3 = 2.8875*1e3;
            double counter = (voc_pos - tape_counter) / _1e3;
            if( counter < 0 ) counter += 1000;
            counter = fmodf(counter, 1000);

            // print each digit separately in two rows, with vertical scroll
            int click = 0;
            ui_at(ui,tapex,tapey);
            for( int place = 100; place > 0; place /= 10 ) {
                int digit = ((int)counter / place) % 10;
                float frac = ((int)(counter * 10)) % 10;
                if( place >=  10 ) if( (((int)counter / (place/ 10)) % 10) != 9 ) frac = 0;
                if( place >= 100 ) if( (((int)counter / (place/100)) % 10) != 9 ) frac = 0;
                int offset = (8-1) * (frac/9.);
#if 0
                ui_y += offset;
                if( ui_y -= 8, ui_monospaced = 1, ui_allow_links = 0, ui_button(NULL, va("%d\f", (1+digit) % 10)) ) click |= ui_click;
                if( ui_x -= 8, ui_y += 8, ui_monospaced = 1, ui_allow_links = 0, ui_button(NULL, va("%d\f", digit)) ) click |= ui_click;
                ui_y -= offset;
#else
                ui_y -= offset;
                if( ui_y += 8, ui_monospaced = 1, ui_allow_links = 0, ui_button(NULL, va("%d\f", (1+digit) % 10)) ) ; // click |= ui_click;
                if( ui_x -= 8, ui_y -= 8, ui_monospaced = 1, ui_allow_links = 0, ui_button(NULL, va("%d\f", digit)) ) click |= ui_click;
                ui_y += offset;
#endif
            }
            // clear upper and bottom rows
            int width = ui_x - tapex;
            TPixel *p = &ui->pix[ui_x-width+tapey*_320];
            TPixel *q = p + 8 * _320;
            for( int y = 0; y < 8; ++y)
                memset32(p + (y-8)*_320, 0, width),
                memset32(p + (y+8)*_320, 0, width);
            // draw shadow
            for( int x = 0; x < width; ++x )
                if( p[x+7*_320].rgba == ui_ff.rgba ) p[x+1+8*_320].rgba = ui_00.rgba;
            // draw hover link
            if( mouse().x >= tapex && mouse().x < ui_x )
            if( mouse().y >= tapey && mouse().y < (tapey+8) ) {
                mouse_cursor(2),
                memset32(q, ui_ff.rgba, width),
                memset32(q+1+1*_320, ui_00.rgba, width);
                // trigger reset animation. adjust target so it displays one third of 999 digits purposedly
                if( mouse().lb )
                    tape_anim = 50, tape_target = voc_pos + 0.666 * _1e3;
            }
        }
    }

    // bottom slider: tape browser. @todo: rewrite this into a RZX player/recorder too
    #define MOUSE_HOVERED_X() (m.x >= TOFF && m.x < (TOFF+_320))
    #define MOUSE_HOVERED_Y() (m.y >= (_240-6-2*(m.x<_320*5/6)) && m.y < (_240+11) ) // m.y > 0 && m.y < 11 
    #define MOUSE_ACTION(pct) tape_seekf(pct)
    #define BAR_Y()           (_240-REMAP(smoothY,0,1,-7,7)) // REMAP(smoothY,0,1,-10,UI_LINE1)
    #define BAR_PROGRESS()    tape_tellf()
    #define BAR_VISIBLE()     ( !tape_inserted() ? 0 : (MOUSE_HOVERED_Y() || BAR_PROGRESS() <= 1. && mic_on/*tape_playing()*/) ) // (m.y > -10 && m.y < _240/10) )
    #define BAR_FILLING(T,...) for(int pct = (int)((x)*1000.0/(T320+!T320)), *e = &pct; e; e = 0) for( unsigned color = ZXPalettes[0][tape_preview[pct]]; color; color = 0 ) if( (x&1) && tape_preview[pct] ) { T; } else { __VA_ARGS__; }

    // bottom slider: tape browser
    const int TOFF = 0/*_32/10*/, T320 = _320 - TOFF * 2;
    int visible = !browser && !do_overlay && BAR_VISIBLE();
    static float smoothY; do_once smoothY = visible;
    smoothY = smoothY * 0.75 + visible * 0.25;
    if( smoothY > 0.01 )
    {
        int y = BAR_Y();

        TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[TOFF + y * _320];

        if( ZX_CRT && (y > _240/2) ) // scanline correction to circumvent CRT edge distortion
            bar -= _320 * 2;

        unsigned mark = BAR_PROGRESS() * T320;
        if( y < (_240/2) ) {
            // bars & progress (top)
            if(y>= 0) for( int x = 0; x < T320; ++x ) bar[x] = white;
            if(y>=-2) for( int x = 0; x < T320; ++x ) bar[x+2*_320] = white;
            if(y>= 1) for( int x = 0; x<=mark; ++x )  bar[x+_320] = white;
            if(y>=-2) for( int x = 1; x<=mark; ++x )  bar[-1+2*_320] = black;
            if(y>=-1) for( int x = 0; x < T320; ++x ) BAR_FILLING(bar[x+1*_320] = ((TPixel){.rgba = color}) /*white*/);
            // triangle marker (top)
            if(y>=-4) bar[mark+4*_320] = white;
            if(y>=-5) for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+5*_320] = white;
            if(y>=-6) for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+6*_320] = white;
        } else {
            // triangle marker (bottom)
++y; bar += _320;
            if(y<=_239-0) for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+0*_320] = white;
            if(y<=_239-1) for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+1*_320] = white;
            if(y<=_239-2) bar[mark+2*_320] = white;
--y; bar -= _320;
            // bars & progress (bottom)
            TPixel zero = {0};
            int hovered = mouse().y > _240-5;
            for( int x = 0; x < T320; ++x ) {
                int pct = (int)((x)*1000.0/(T320+!T320));
                unsigned tape_color = tape_preview[pct];

#if 1
                if( y <= _239-6 ) bar[x+6*_320] = x < (mark+!tape_color) && !hovered ? white : !tape_color ? zero : (tape_color & 1 ? x & 1 : x & 5) ? white : zero;
#elif 0
                if( y <= _239-4 ) bar[x+4*_320] = white;
                if( y <= _239-6 ) bar[x+6*_320] = x == mark ? white : !tape_color ? zero : (tape_color & 1 ? x & 1 : x & 5) ? white : zero;
#elif 0
                if( y <= _239-2 ) bar[x+2*_320] = white;
                if( y <= _239-3 ) bar[x+3*_320] = x == mark ? white : zero;
                if( y <= _239-4 ) bar[x+4*_320] = x == mark ? white : !tape_color ? zero : (tape_color & 1 ? x & 1 : x & 5) ? white : zero;
                if( y <= _239-5 ) bar[x+5*_320] = x == mark ? white : zero;
                if( y <= _239-6 ) bar[x+6*_320] = white;
#elif 0
                if( y <= _239-4 ) bar[x+4*_320] = white;
                if( y <= _239-5 ) bar[x+5*_320] = x <= mark+1 ? white : zero;
                if( y <= _239-6 ) bar[x+6*_320] = x <= mark+1 ? white : !tape_color ? zero : (x & (tape_color & 1 ? 1 : 5)) ? white : zero;
#else
                if( y <= _239-4 ) bar[x+4*_320] = white;
                if( y <= _239-5 ) bar[x+5*_320] = x <= mark+1 ? white : !tape_color ? zero : (x & (tape_color & 1 ? 1 : 3)) ? zero : white;
                if( y <= _239-6 ) bar[x+6*_320] = white;
#endif
            }
        }

        // is mouse hovering
        if( MOUSE_HOVERED_Y() && MOUSE_HOVERED_X() ) {
            mouse_cursor(2);
            if( m.buttons ) {
                // MOUSE_ACTION((CLAMP(m.x,TOFF,TOFF+T320)-TOFF) / ((float)T320));
                MOUSE_ACTION((m.x-TOFF) / ((float)T320));
                // cmdkey = 'STOP';
            }
            static int prev = 0;
            if( prev && !m.buttons ) {
                cmdkey = 'PLAY';
            }
            prev = m.buttons;
        }



    }

    // bottom slider. @todo: rewrite this into a RZX player/recorder
    if( 0 )
    if( ZX_DEBUG )
    if( !browser && !do_overlay ) {
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
                ui_print(ui, (mark+5)/11.f,(_240-12.0)/11, ui_colors, text);
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

    if( !browser ) return 0;

    // game browser
    if( browser ) {
        // disable overlay
        if( do_overlay ) tigrClear(overlay, tigrRGBA(0,0,0,0));
        do_overlay = 0;

        // restore mouse interaction in case it is being clipped (see: kempston mouse)
        mouse_clip(0);
        mouse_cursor(1);
    }

    char *entry = version == 2 ? game_browser_v2() : game_browser_v1();
    if( entry || cmdkey == 'ZXDB' ) {
        //mouse_cursor(1); // @fixme: restore mouse cursor after clicking an item list
        ui_dialog_new(NULL); // cancel any internal dialog that remains active
    }
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
    // normalize argv[0] path
    if( strrchr(__argv[0], '/') ) __argv[0] = strdup(va("%s", strrchr(__argv[0], '/')+1));
    if( strrchr(__argv[0],'\\') ) __argv[0] = strdup(va("%s", strrchr(__argv[0],'\\')+1));

    // init external zxdb (high priority)
    zxdb_init("Spectral.db");

    // init embedded zxdb (medium priority)
    // also change app behavior to ZX_PLAYER if embedded games are detected.
    if( !ZX_PLAYER )
    {
        int embedlen; char *embed;
        for( unsigned id = 0; (embed = embedded(id, &embedlen)); ++id ) {
            if(!memcmp(embed + 0000, "\x1f\x8b\x08",3))
            if(!memcmp(embed + 0x0A, "Spectral.db",11)) {
                //tinyARC4(embed + 72, embed + 72, embedlen - 72, "FuckM$Defender");
                zxdb_initmem(embed, embedlen);
                continue;
            }
            if(!memcmp(embed + 0000, "Rar!",4))
            if(!memcmp(embed + 0x38, "Spectral.db",11)) {
                //tinyARC4(embed + 72, embed + 72, embedlen - 72, "FuckM$Defender");
                zxdb_initmem(embed, embedlen);
                continue;
            }
            if( embedlen ) ZX_PLAYER |= 1;
        }
    }

    // init embedded zxdb as resource (low priority)
    #if _WIN32
    if( !ZX_PLAYER )
    {
        HMODULE hModule = NULL;
        HRSRC resourceInfo = FindResourceA(hModule, "ZXDBData", "CUSTOMDATA");
        if (resourceInfo) {
            HGLOBAL resource = LoadResource(hModule, resourceInfo);
            if (resource) {
                DWORD resourceSize = SizeofResource(hModule, resourceInfo);
                zxdb_initmem(resource, resourceSize);
            }
        }
    }
    #endif

    // import config
    if(!ZX_PLAYER)
    load_config();

    // fixed settings on zxplayer builds
    if( ZX_PLAYER ) {
        ZX_RF = strstri(__argv[0], "-rf") ? 1 : 0;
        ZX_CRT = strstri(__argv[0], "-crt") ? 1 : 0;
        ZX_HORACE = 0;
        ZX_PALETTE = ZX_PLAYER_PALETTE; // vivid
    }

    // console
#if DEV
    // forced console
    loggers_(1);
    logo();
#else
    // logo can be printed on linux/macos builds (dev+release) and also on win32 builds if terminal window is open.
    loggers(ZX_CONSOLE);
#endif

    // prepare title based on argv[0]
    char *apptitle = va("%s", __argv[0]);
    while( strrchr(apptitle,'\\') ) apptitle = strrchr(apptitle,'\\') + 1;
    while( strrchr(apptitle, '/') ) apptitle = strrchr(apptitle, '/') + 1;
    if( strrchr(apptitle, '.') ) *strrchr(apptitle, '.') = '\0';
    apptitle = va("%s%s", apptitle, DEV ? " DEV" : "");

    // main app and bitmap layers
    if( !app_create(apptitle,ZX_FULLSCREEN,ZX_ZOOM) ) die("cannot create app window");
    canvas = tigrBitmap(_256, _192);

    // disable win key
    // disable nag window after holding shift for 8s (windows)
    // ensure this is called after win32 handles are created
    enable_os_keys(0);

    // must be as close to frame() as possible
    audio_init();

    // zx
    boot(ZX|ZX_PENTAGON/*128*/, 0);

    // load embedded games (if any)
    {
        int embedlen; const char *embed;
        for( unsigned id = 0; (embed = embedded(id, &embedlen)); ++id ) {
            // skip spectral.db.gz
            if(!memcmp(embed + 0000, "\x1f\x8b\x08",3))
            if(!memcmp(embed + 0x0A, "Spectral.db",11))
                continue;
            // skip spectral.db.rar
            if(!memcmp(embed + 0000, "Rar!",4))
            if(!memcmp(embed + 0x38, "Spectral.db",11))
                continue;
            loadbin(embed, embedlen, 1);
        }
    }

    // zxplayer does not load/save state
    if(!ZX_PLAYER)

    {
        int seekpos = 0;
        int shaded = ZX_SHADED;

        // restore media: almost same than reload(ZX);
        titlebar(ZX_TITLE);
        if( ZX_MEDIA ) {
            char *at = strrchr(ZX_MEDIA, '@'); if(at) *at = 0, seekpos = atoi(at+1);
            if( !load(ZX_MEDIA, ZX)) {} // @fixme: emit warning
        }

        // import state; do not clear medias we just loaded 3 lines ago
        for( FILE *state = fopen("Spectral.sav","rb"); state; fclose(state), state = 0) {
            import_state(state, 0|KEEP_MEDIA);
        }

        tape_seeki(seekpos);

        // refresh postfx + user shader too
        ZX_SHADED = shaded;
        app_apply_shader();
    }

    // main loop
    do {

    #if 0 && NEWCORE
    do {
    #endif

        // flush
        if( tigrClosed(app) )
            break;
        tigrUpdate(app);

        // force pause when blurred
        static int paused = 0; int running = (ZX_FLASHLOAD ? 0 : tape_playing()) || tigrFocused(app);
        paused = (paused + 1) * !running;
#if DEV
        ifdef(win32, paused *= !IsDebuggerPresent());
#endif
        if( ZX_PAUSE && paused ) {
            if( paused > 1 ) {
                ifdef(win32, Sleep(500), sys_sleep(500));
            } else {
                for( TPixel *ix = app->pix, *ixend = ix+_320*_240; ix < ixend; ++ix ) {
                    ix->rgba = ( ix->rgba & 0x00f8f8f8 ) >> 3 | 0xff000000;
                }
                ui_monospaced = 1;
                ui_print(app, (_320-12*8)/2, _240/2, ui_colors, "- P A U S E -" );
            }
            continue;
        }

#if 1
        // 4 parameters in our shader. we use parameters[0] to track time
        tigrSetPostFX(app, (ticks / (69888 * 50.)), -mouse_offsets()[0],-mouse_offsets()[1], 0);

        float blur = ZX_BLUR/100.;
        float bloom = ZX_BLOOM/100.; bloom = bloom * bloom + 1;
        tigrSetPostFX2(app, bloom,blur,0,0 );

        // update background color so it matches. this is especially visible during fullscreen
        glColor[0] = glColor[1] = glColor[2] = 0, glColor[3] = 1;
#if 0
        if( !ZX_CRT )
        glColor[0] = ((ZXPalette[ZXBorderColor] >> 0) & 255) / 255.0,
        glColor[1] = ((ZXPalette[ZXBorderColor] >> 8) & 255) / 255.0,
        glColor[2] = ((ZXPalette[ZXBorderColor] >>16) & 255) / 255.0;
#endif

#endif

        key_read();
        ui_frame_begin();
        input();
        if(do_overlay || cmdkey == 'PAD1') ZXKeyboardClear(); // do not submit keys to ZX while overlay is drawn on top

        // handle ESC/RMB (cancel buttons)
        int escape = key_trigger(TK_ESCAPE);
        int cancel = escape || mouse().rb;
        if( cancel ) {
            // cancel dialog
            if( num_options && mouse().cursor != 2 ) ui_dialog_new(NULL);
            // cancel overlay
            else if( do_overlay ) do_overlay = 0, tigrClear(overlay, tigrRGBA(0,0,0,0));
            // else toggle browser (ESC key only)
            else if( !mouse().rb ) cmdkey = 'GAME';
        }

        // detect disk activity: fdc.timeout can be 0 or 512 while idling. wd.Wait is 255 for every cmd, then gets decremented every tick.
        // not using fdc.motor because some games leave that set while playing (cybernoid2,smaily,rickdangerous2)
        int fdc_inuse3 = ZX == 300 && ((fdc.timeout & 0x1FF) || (PC(cpu) < 0x4000 && GET_MAPPED_ROMBANK() == GET_3DOS_ROMBANK()));
        int fdc_inuseP = ZX_PENTAGON && ((wd.Wait == 255) || ((PC(cpu) & 0xFF00) == 0x3D00 && GET_MAPPED_ROMBANK() == 1));
        static int disk_hz = 0; disk_hz = fdc_inuse3 || fdc_inuseP || play_findvoice('read') || play_findvoice('seek'); // disk_hz += 50 * fdc_inuse; disk_hz = CLAMP(disk_hz-1, 0, 50);

        int disk_likely_loading = disk_hz > 0;
        int tape_likely_loading = (PC(cpu) & 0xFF00) == 0x0500 ? 1 : tape_hz > 40;

        int media_accelerated = ZX_FASTCPU || key_held(TK_RSHIFT) ? 1
            : tape_inserted() && tape_peek() == 'o' ? 0 
            : ZX_FASTTAPE && tape_likely_loading && tape_playing() ? 1
            : ZX_FASTTAPE && disk_likely_loading ? 1
            : 0;

        if( browser ) media_accelerated = 0;

        // z80, ula, audio, etc
        // static int frame = 0; ++frame;
        int do_sim = browser ? 0 : 1;
        int do_drawmode = 1; // no render (<0), full frame (0), scanlines (1)
        int do_flashbit = media_accelerated ? 0 : 1;
        int do_runahead = media_accelerated ? 0 : ZX_RUNAHEAD;

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
        rom_patch_klmode(PC(cpu));
        //printf("%X\n",PC(cpu));

        static byte counter = 0; // flip flash every 16 frames @ 50hz
        if( !((++counter) & 15) ) if(do_flashbit) ZXFlashFlag ^= 1;


#if 0 // needed for outlines, attrib clash removal and/or to highlight vram changes
static char VRAM0[6912];
static unsigned *r0, *r1;
r0 = realloc(r0, _256 * _192 * 4);
r1 = realloc(r1, _256 * _192 * 4);
int cond = key_pressed('X'); static int counter_ = 0; cond |= counter_++ < 10;
if(cond)
memcpy(VRAM0, VRAM, 6912),
memcpy(r0, canvas->pix, _256*_192*4);
#endif


if( do_runahead == 0 ) {
        do_audio = 1;
        frame(do_drawmode, do_sim); //media_accelerated ? (frame%50?0:1) : 1 );
} else {
        // runahead:
        // - https://near.sh/articles/input/run-ahead 
        // - https://web.archive.org/web/20200322151707/https://byuu.net/input/run-ahead
        // https://www.youtube.com/watch?v=_qys9sdzJKI 
        // https://docs.libretro.com/guides/runahead/

        do_audio = 0;
        int do_video = -1;

        frame(do_video, do_sim);

        quicksave(10);

        if( do_runahead > 1 )
           frame(do_video, do_sim); // repeat for 2-frame runaheads, ...
        // frame(do_video, do_sim); // repeat for 3-frame runaheads, ...
        // frame(do_video, do_sim); // repeat for 4-frame runaheads, etc.

        do_audio = 1;
        do_video = do_drawmode;
        frame(do_video, do_sim); // final frame does video+audio

        quickload(10);
}


#if 0 // DEV_HIGHLIGHT_VRAM_CHANGES
    unsigned changed = memcmp(VRAM0, VRAM, 6912);
    if( changed ) {

        static byte *bit2;
        {
            bit2 = realloc(bit2, _256*_192);
            memset(bit2,0,_256*_192);

            // isolate moving objects: subtract frame1-frame0
            for( int y = 0; y < 192; ++y ) {
                byte *pixels0=VRAM0+SCANLINE(y);
                byte *pixels1=VRAM +SCANLINE(y);

                int offs = _32 + (y+_24) * _256;
                for( int x = 0; x < 32; ++x, offs += 8 ) {
                    int mask = pixels0[x] ^ pixels1[x];
                    bit2[offs+0] = (mask & 0x80) >> 7;
                    bit2[offs+1] = (mask & 0x40) >> 6;
                    bit2[offs+2] = (mask & 0x20) >> 5;
                    bit2[offs+3] = (mask & 0x10) >> 4;
                    bit2[offs+4] = (mask & 0x08) >> 3;
                    bit2[offs+5] = (mask & 0x04) >> 2;
                    bit2[offs+6] = (mask & 0x02) >> 1;
                    bit2[offs+7] = (mask & 0x01) >> 0;
                }
            }

#if 1
            // dilate > outline > floodfill > erode
            byte *dilated = s2o_dilate((byte*)bit2, _256, _192);
            byte *outlined = s2o_outline(dilated, _256, _192);

            if(0) // visualize
            for( int i = 0; i < _192 * _240; ++i)
                if( outlined[i] ) canvas->pix[i].rgba = ZXPalette[1];

            s2o_floodfill(outlined, _256, _192);
            byte *eroded = s2o_erode(outlined, _256, _192);

            free(dilated);
            free(outlined);
            // free(eroded);

            free(bit2);
            bit2 = eroded;
#else
            bit2 = fill_closed_outline_shapes(bit2, _256, _192);
            //byte *bit3 = s2o_safe_dilate_interior_white(bit2, _256, _192);
            byte *bit3 = s2o_morph_close_white(bit2, _256, _192);
            bit3 = fill_enclosed_black_regions(bit3, _256, _192);
            free(bit2);
            bit2 = bit3;
#endif

            if(key_held('V')) // disabled
                memset(bit2,1,_256*_192);

            int scale = 4; if(key_pressed('C')) scale = 1;
            if(0) // visualize
            for( int y = 0; y < _192/scale; ++y )
            for( int x = 0; x < _256/scale; ++x ) {
                int i = x*scale + (y*scale) * _256;
                canvas->pix[x+((_192-_192/scale)+y)*_256].rgba = bit2[i] ? ~0u : RGB(0,0,0);
            }
        }

#if 0
        for( int y = 0; y < 192; ++y ) {
            byte *attribs0=VRAM0+6144+((y&0xF8)<<2);
            byte *attribs1=VRAM +6144+((y&0xF8)<<2);

            int idx = _32+(y+_24)*_256;

            for( int x = 0; x < 32; ++x ) {
                if( attribs0[x] == attribs1[x] ) continue;
                int i = idx + x * 8;
                if( !bit2[  i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
                if( !bit2[++i] ) canvas->pix[i].rgba = r0[i];
            }
        }
#else
        int idx = _32 + _24 * _256, next = _32 * 2;
        for( int y = 0; y < 192; ++y, idx += next ) {
            for( int end = idx+256; idx < end; ++idx ) {
                if( !bit2[idx] )
                canvas->pix[idx].rgba = r0[idx];
            }
        }
#endif
    }
#endif

        // @todo: optimize me
        tigrClear(app, tigrRGB(0,0,0));
        tigrClear(ui, !browser && !do_overlay ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,128));

        // blit zx layer (canvas) to main app
        {
            // center
            int x = canvas->w < app->w ? (app->w - canvas->w) / 2 : -(canvas->w - app->w) / 2;
            int y = canvas->h < app->h ? (app->h - canvas->h) / 2 : -(canvas->h - app->h) / 2;
            // detect resize change
            static int prevx = 0, prevy = 0;
            if( x != prevx || y != prevy )
                app_resize();
            prevx = x, prevy = y;
            // blit
            tigrBlit(app, canvas, x,y, 0,0,_256,_192);
            // adjust mouse coords for next frame
            mouse_offsets()[0] = -x;
            mouse_offsets()[1] = -y;
            // ultrawide ula: for every paper scanline, repeat leftmost/rightmost pixels to infinity
            if( ZX_ULAPLUS >= 2 ) {
                if( x > 0 )
                for( int sc = 0; sc < (0+_192); ++sc)
                    if( (sc+y) >= 0 && (sc+y) < _240 )
                    memset32(&app->pix[0+(y+sc)*_320], *(rgba*)&canvas->pix[0+sc*_256], x),
                    memset32(&app->pix[_319-x+(y+sc)*_320], *(rgba*)&canvas->pix[_256-1+sc*_256], x);
                // truncate last line since it is unlikely to be a full width line in any zx model
                if( y > 0 )
                for( int sc = _192-1; sc < (0+_192); ++sc)
                    if( (sc+y) >= 0 && (sc+y) < _240 )
                    memset32(&app->pix[0+(y+sc)*_320], *(rgba*)&canvas->pix[0+sc*_256], _320);
            }
        }

        char* game = game_browser(ZX_BROWSER);
        if( game ) {
            browser = 0;

            bool insert_next_disk_or_tape = false;
            if( ZX_TITLE ) { // @fixme: ZXDB might use ZX_MEDIA and search for sequential #X diffs instead
                if( 0 != strcmp(game, ZX_TITLE) ) {
                    insert_next_disk_or_tape = are_sequential_urls(game, ZX_TITLE);
                }
            }

            int model = 0; // game[0] == '#' ? 0 : 48; // auto-detect if zxdb. local files probably a 48k game
            if( key_pressed( TK_SHIFT) ) // alt model
                model = ZX < 128 ? 128 : 48;

            load_should_clear = insert_next_disk_or_tape || strstri(game, ".pok") ? 0 : 1;
            if( load(game,model) ) {
                titlebar(game);

                if( ZX_MEDIA != game )
                (free(ZX_MEDIA), ZX_MEDIA = strdup(game));

                // clear window keys so the current key presses are not being sent to the 
                // next emulation frame. @fixme: use ZXKeyboardClear(); instead
                memset(tigrInternal(app)->keys, 0, sizeof(tigrInternal(app)->keys));
                memset(tigrInternal(app)->prev, 0, sizeof(tigrInternal(app)->prev));
            }
        }

        // measure time & frame lock (50.01 fps)
        int max_speed = browser ? 0 : media_accelerated || !ZX_FPSMUL || ZX_FASTCPU || key_held(TK_RSHIFT); // max speed if media_accelerated or no fps lock
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
            static uint64_t prev_dt = 0; do_once prev_dt = time_ns();
            uint64_t now_dt = time_ns();
            int64_t diff = now_dt - prev_dt;
            prev_dt = now_dt;

            float target = (ZX_FPSMUL/100.0) * (ZX < 128 ? 50.08:50.01);
            double target_ms = 1000 / (target+!target);
            double target_ns = target_ms * 1000000;
            if( diff < target_ns ) SDL_DelayPrecise( target_ns - diff ); // tigrDelayPrecise()

            dt = diff / 1e9; // dt = tigrTime();
#elif 1 // accurate (beware of CPU usage)
            ifdef(win32, timeBeginPeriod(1));

            float target = (ZX_FPSMUL/100.0) * (ZX < 128 ? 50.08:50.01);

            // be nice to os
            sys_sleep(ZX_FPSMUL > 120 ? 1 : 5);
            // complete with shortest sleeps (yields) until we hit target fps
            dt = tigrTime();
            for( float target_fps = 1.f/(target+!target); dt < target_fps; ) {
                sys_yield();
                dt += tigrTime();
            }

            ifdef(win32, timeEndPeriod(1));
#endif
        }

        // zxplayer quits loop early, so...
        // it wont be drawing UI
        // it wont be drawing game browser
        // it wont be processing cmdline/FN-keys commands
        // it wont be processing drag 'n drops
        if( ZX_PLAYER ) {
            continue;
        }

        // calc fps
        static int frames = 0; ++frames;
        static double time_now = 0; time_now += dt;
        if( time_now >= 1 ) { ZX_FPS = frames / time_now; time_now = frames = 0; /* printf("%5.2ffps\n", ZX_FPS); */ }

        // tape timer
        if(tape_playing()) timer += dt;

        static char dev_status[256] = "";
        for( dev_status[0] = 0; (DEV || ZX_DEBUG) && !browser && !dev_status[0]; ) {
            // renders per second
            static int rps_;
            static char screen_[6912];
            rps_ += !!memcmp(screen_, VRAM, 6912); memcpy(screen_, VRAM, 6912);
            static float taken_;
            if( (taken_ += dt) >= 1 ) taken_ -= (int)taken_, ZX_RPS = rps_, rps_ = 0;

            char *ptr = dev_status;
            ptr += sprintf(ptr, "%02d/", (int)ZX_RPS);
            ptr += sprintf(ptr, "%5.2ffps%s%3d\f\f\f\fmem%s%d%d%d%d ", ZX_FPS, do_runahead ? "!":"", ZX, rom_patches ? "!":"", GET_MAPPED_ROMBANK(), (page128&8?7:5), 2, page128&7);
            ptr += sprintf(ptr, "      "); // leave blank space for chat icon
            ptr += sprintf(ptr, "%02X%c%02X %04X ", page128, page128&32?'!':' ', page2a, PC(cpu));

            for( int i = 0; i < voices_max; ++i )
            ptr += sprintf(ptr, "%x", (int)voice[i].count > 15 ? 15 : voice[i].count );

            ptr += sprintf(ptr, " %c%c%d %dHz ", "  +-"[tape_inserted()*2+tape_level()], toupper(tape_peek()), mic_on, disk_hz ? disk_hz : tape_hz);
            ptr += sprintf(ptr, "\n%dm%02ds ", (unsigned)(timer) / 60, (unsigned)(timer) % 60);
        }

        if(!browser)
        mouse_cursor(ZX_GUNSTICK ? 4 : 1);

        if(ZX_PALETTE_PREVIEW)
            if(draw_palette(app, ZXPalettes[ZX_PALETTE] /*ZXPalette*/, 2+ZXPaletteNames[ZX_PALETTE % countof(ZXPaletteNames)]))
                palette_use(ZX_PALETTE);

        // rec before/after UI,
        int rec(Tigr*);
        int gui(const char *);
        gui(dev_status);
        rec(ZX_PRINTUI ? app : canvas);

        #define LOAD(ZX,file) if(file) do { \
                if( !load(file,ZX) ) { \
                    if( is_folder(file) ) cmdkey = 'SCAN', cmdarg = file; \
                    else alert(va("cannot open '%s' file\n", file)); \
                } else titlebar(file), (free(ZX_MEDIA), ZX_MEDIA = strdup(file)); \
            } while(0)

        // parse drag 'n drops. reload if needed
        for( char **list = tigrDropFiles(app,0,0); list; list = 0)
        for( int i = 0; list[i]; ++i ) {
        	while( strchr("\r\n", list[i][strlen(list[i])-1]) ) list[i][strlen(list[i])-1] = '\0';
            #if TESTS
            LOAD(48,list[i]);
            #else
            LOAD(00,list[i]);
            #endif
        }

        // parse cmdline. reload if needed
        do_once {
            int showhelp = flag("-h") || flag("--help") || flag("-v") || flag("--version");
            if( showhelp ) {
                FILE *fp = fopen("README.txt", "wt");
                if( fp ) {
                    fprintf(fp, "Spectral " SPECTRAL "\n");
                    fprintf(fp, "https://github.com/r-lyeh/Spectral\n\n");

                    fprintf(fp, "%s [options] [file]\n", __argv[0]);
                    for( int j = 1; j < countof(commands); j += 2 ) {
                        char cmd[128] = {0}, info[128] = {0}, values[128] = {0};
                        sscanf(commands[j], "%[^:] %[^\n]\n %[^\n]", cmd, info, values);
                        char header[16]; snprintf(header, 16, "%s%s", cmd + strspn(cmd," "), values[0] ? "=X" : "" );
                        if( values[0] )
                        fprintf(fp, "  --%-6s  %s (%s)\n", header, info+1, values);
                        else
                        fprintf(fp, "  --%-6s  %s\n", header, info+1);
                    }
                    fclose(fp);
                }
                char *txt = readfile("README.txt", 0);
                puts(txt);
                alert(txt);
                free(txt);
            }

            // files/folders first
            for( int i = 1; i < __argc; ++i ) {
                if( __argv[i][0] != '-' ) {
                    #if TESTS
                    LOAD(48,list[i]);
                    #else
                    LOAD(00,__argv[i]);
                    #endif
                }
            }
            // @fixme: more than 1 arg
            // @fixme: do not save args to ini
            // options next
            for( int i = 1; i < __argc; ++i ) {
                if( __argv[i][0] == '-' ) {
                    char cmd[128]; snprintf(cmd, 128, "%s", __argv[i]);
                    if( strchr(cmd, '=') ) *strchr(cmd, '=') = '\0';

                    cmdarg = flag(cmd);
                    if( !cmdarg ) continue;

                    char *p = cmd + strspn(cmd, "-");
                    int cmdlen = strlen(p);
                    if( cmdlen > 0 ) cmdkey = toupper(p[0]);
                    if( cmdlen > 1 ) cmdkey = toupper(p[1]) | (cmdkey << 8);
                    if( cmdlen > 2 ) cmdkey = toupper(p[2]) | (cmdkey << 8);
                    if( cmdlen > 3 ) cmdkey = toupper(p[3]) | (cmdkey << 8);
                    if( cmdlen > 4 ) continue;
                }
            }
        }

        // joffa ui
        do_once {
            time_t t = time(0);
            struct tm *tm = localtime(&t);
            int mm = tm->tm_mon+1, dd = tm->tm_mday;
            if( mm == 02 && dd == 01 ) ui_mirror_bak = 1;
        }

        // clear command
        int cmdkey_ = cmdkey; cmdkey = 0;
        const char *cmdarg_ = cmdarg; cmdarg = 0;

        // parse commands
        ZX_FULLSCREEN = !tigrWindowed(app); // update var in case user pressed ALT+ENTER, or in some other external way
        switch(cmdkey_) { default: 
            #if DEV
            if(cmdkey_) hexdump(&cmdkey_, sizeof(cmdkey_));
            if(cmdkey_) alert(va("command not found `%08x`", cmdkey_));
            #endif
            break; case 'SETG': ZX_BLUR = 50;
                                ZX_BLOOM = 0;
                                ZX_ULAPLUS = 2;
                                palette_use(ZX_PALETTE = 0);
                                ZX_PALETTE_PREVIEW = 0;
                                ZX_SHADED = 0;
                                ZX_RF = 1;
                                ZX_CRT = 1;
                                app_apply_shader();
                                if( app_create(window_title(NULL), ZX_FULLSCREEN, 2) ) ZX_ZOOM = 2;
                                if( app_create(window_title(NULL), 0, ZX_ZOOM) ) ZX_FULLSCREEN = 0;

            break; case 'GAME':  if( ZX_BROWSER == 1 ? numgames : 1 ) browser ^= 1, ui_dialog_new(NULL);
            break; case 'MAX':   ZX_FASTCPU ^= 1; // toggle fast-forward cpu

            break; case 'PLAY':  tape_play(1); ZX_AUTOSTOP = 0; ZX_AUTOPLAY = 0; // tape_play(!tape_playing()); /*if(!tape_inserted()) browser ^= 1; else tape_play(!tape_playing());*/ // open browser if start_tape is requested but no tape has been ever inserted
            break; case 'PREV':  tape_prev();
            break; case 'NEXT':  tape_next();
            break; case 'STOP':  tape_stop();  ZX_AUTOPLAY = 0; ZX_AUTOSTOP = 0;
            break; case 'EJEC':  tape_reset();
            break; case 'FAST':  ZX_FASTTAPE ^= 1;

            break; case  'RF':  ZX_RF  ^= 1; if(cmdarg_) ZX_RF  = (ZX_RF  * (cmdarg_[0] == '^')) ^ atoi(cmdarg_ + !isdigit(cmdarg_[0]));
            break; case 'CRT':  ZX_CRT ^= 1; if(cmdarg_) ZX_CRT = (ZX_CRT * (cmdarg_[0] == '^')) ^ atoi(cmdarg_ + !isdigit(cmdarg_[0]));
            break; case  'TV':  { static int mode = 0; do_once mode = ZX_SHADED ? 4 : ZX_RF << 1 | ZX_CRT; mode = (mode + 1) % (ZX_SHADER && ZX_SHADER[0] ? 5 : 4); if(cmdarg_) mode = atoi(cmdarg_); ZX_RF = !!(mode & 2); ZX_CRT = mode & 1; ZX_SHADED = mode == 4; app_apply_shader(); }
            break; case 'SHAD': app_browse_shader();

            break; case 'FULL': { int mode = cmdarg_ ? atoi(cmdarg_) : (ZX_FULLSCREEN ^ 1);
                                if( app_create(window_title(NULL), mode, ZX_ZOOM) ) ZX_FULLSCREEN = mode; }

            break; case 'ZOOM': { int mode = (ZX_ZOOM + !ZX_ZOOM + 1) % 6; if( !app_wouldfit(mode) ) mode = 1; if( cmdarg_ ) mode = atoi(cmdarg_);
                                if( app_create(window_title(NULL), ZX_FULLSCREEN, mode) ) ZX_ZOOM = mode + !mode; }

            break; case 'SAVE':   quicksave(0);
            break; case 'LOAD':   quickload(0);

            break; case 'PIC':  cmdkey = 'PIC2', ZX_PRINTUI = 0 ^ (!!key_pressed( TK_SHIFT)); // resend screenshot cmd
            break; case 'PIC_': cmdkey = 'PIC2', ZX_PRINTUI = 1 ^ (!!key_pressed( TK_SHIFT)); // resend screenshot cmd
            break; case 'PIC2':
            break; case 'REC':  cmdkey = 'REC2', ZX_PRINTUI = 0 ^ (!!key_pressed( TK_SHIFT)); // resend recording cmd
            break; case 'REC_': cmdkey = 'REC2', ZX_PRINTUI = 1 ^ (!!key_pressed( TK_SHIFT)); // resend recording cmd
            break; case 'REC2':

            break; case 'TURB':  ZX_TURBOROM ^= 1; if(tape_inserted()) boot(ZX, 0|KEEP_MEDIA), reload(ZX, 1); // toggle turborom and reload
            break; case 'BOOT':  reset(0|KEEP_MEDIA), reload(ZX, 1);
            break; case 'NMI':   if( pins & Z80_NMI ) pins &= ~Z80_NMI; else pins |= Z80_NMI; RZX_reset(); if(rom[3]==0x76) boot(ZX, 0); // also wipe media on DI+HALT case (our .scr viewer) // @todo: verify
            break; case 'WIPE':

            #if ZX_CUSTOM_ROMS
                // restore original roms if they were replaced
                rom48 = rom48_bak;
                rom128 = rom128_bak;
                romplus2 = romplus2_bak;
                romplus341 = romplus341_bak;
                rompentagon128 = rompentagon128_bak;
            #endif

                reset(0); ZXDB = zxdb_free(ZXDB); if(ZX_MEDIA) (free(ZX_MEDIA), ZX_MEDIA = 0); if(ZX_TITLE) (free(ZX_TITLE), ZX_TITLE = 0); titlebar(""); // clear media    KEEP_MEDIA/*|QUICK_RESET*/);
            
            break; case 'CLIP':  if( !app_clipboard ) app_clipboard = tigrGetClipboard(app);
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

                boot(ZX, 0|KEEP_MEDIA), reload(ZX, 1); // toggle model and reload last media

                // hack: force model if something went wrong. @fixme: investigate why
                if( ZX == 128 ) if( model & 1 ) if(!ZX_PENTAGON) ZX_PENTAGON = 1, rom_restore();

                titlebar(ZX_TITLE); // refresh titlebar to reflect new model
            }
            // cycle many settings
            break; case 'AY':    { const int table[] = { 1,2,0,0 }; ZX_AY = table[ZX_AY]; if(cmdarg_) ZX_AY=atoi(cmdarg_); }
            break; case 'WAVE':  ZX_WAVES ^= 1;                                   if(cmdarg_) ZX_WAVES=atoi(cmdarg_);
            break; case 'LENS':  ZX_LENSLOK ^= 1;                                 if(cmdarg_) ZX_LENSLOK=atoi(cmdarg_);
            break; case 'BLUR':  ZX_BLUR = (ZX_BLUR+10)%100;                      if(cmdarg_) ZX_BLUR=atoi(cmdarg_)%100;
            break; case 'BLUM':  ZX_BLOOM = (ZX_BLOOM+10)%100;                    if(cmdarg_) ZX_BLOOM=atoi(cmdarg_)%100;
            break; case 'MAKE':  { 

            const char *pathgame = cmdarg_ ? cmdarg_ : app_loadfile();

            if( pathgame ) {
                int gamelen;
                unsigned char *game = readfile(pathgame, &gamelen);
                if( !game ) ui_alert("", "\2Error\7: Cannot read input file");

                int exelen;
                unsigned char *exe = readfile(__argv[0], &exelen);
                if( !exe ) ui_alert("", "\2Error\7: Cannot read Spectral executable");

                if( game && gamelen && exe && exelen ) {
                    const char *pathexe = va("%s.%s", pathgame, __argv[0]);
 
                    char watermark[] = SPECTRAL_EMBED_WATERMARK;
                    watermark[0] -= 32;

                    exe = realloc(exe, exelen + 16 + gamelen); // may @leak
                    if( exe ) {
                        memcpy(exe+exelen, watermark, 16);
                        memcpy(exe+exelen+16, game, gamelen);
                    }

                    if( exe && writefile(pathexe, exe, exelen + 16 + gamelen) )
                        ui_alert("", "\5%s\7\n\n generated correctly.", basename(pathexe));
                    else
                        ui_alert("", "\2Error\7: Cannot create output executable.");

                }

                if(exe)  free(exe);
                if(game) free(game);
            }}

            break; case 'PALB':  { const char *file = app_loadfile(); if( file && strendi(file, ".pal") ) if( pal_load(file) ) palette_use(ZX_PALETTE = countof(ZXPalettes) - 1), cmdkey = 'PAL0'; }
            break; case 'PAL':   ZX_PALETTE = (ZX_PALETTE+1)%countof(ZXPalettes); if(cmdarg_) ZX_PALETTE=atoi(cmdarg_);    palette_use(ZX_PALETTE), ZX_BLOOM = atoi(ZXPaletteNames[ZX_PALETTE]);
            break; case 'PALP':  ZX_PALETTE_PREVIEW ^= 1;                         if(cmdarg_) ZX_PALETTE_PREVIEW=(ZX_PALETTE_PREVIEW * (cmdarg_[0] == '^')) ^ atoi(cmdarg_ + !isdigit(cmdarg_[0]));
            break; case 'FIRJ':  ZX_JOYSTICK_AUTOFIRE =(ZX_JOYSTICK_AUTOFIRE+1)%4;if(cmdarg_) ZX_JOYSTICK_AUTOFIRE=atoi(cmdarg_);
            break; case 'FIRM':  ZX_MOUSE_AUTOFIRE = (ZX_MOUSE_AUTOFIRE+1)%4;     if(cmdarg_) ZX_MOUSE_AUTOFIRE=atoi(cmdarg_);
            break; case 'MICE':  ZX_MOUSE = (ZX_MOUSE+1)%3;                       if(cmdarg_) ZX_MOUSE=atoi(cmdarg_);      if(ZX_GUNSTICK) ZX_JOYSTICK = 0; // cycle mouse/guns
            break; case 'ULA':   ZX_ULAPLUS = (ZX_ULAPLUS+1)%3;                   if(cmdarg_) ZX_ULAPLUS=atoi(cmdarg_);    // cycle ulaplus
            break; case 'RUN':   ZX_RUNAHEAD = (ZX_RUNAHEAD+1)%3;                 if(cmdarg_) ZX_RUNAHEAD=atoi(cmdarg_);   // cycle runahead mode
            break; case 'DEV':   ZX_DEBUG ^= 1;                                   if(cmdarg_) ZX_DEBUG=atoi(cmdarg_);
            break; case 'KL':    ZX_KLMODE ^= 1;                                  if(cmdarg_) ZX_KLMODE=atoi(cmdarg_);     ZX_KLMODE_PATCH_NEEDED = 1;
            break; case 'CHAT':  ZX_LOBBY ^= 1;                                   if(cmdarg_) ZX_LOBBY=atoi(cmdarg_);
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
                                        load(ZX_MEDIA,ZX);
                                    }
#endif
            break; case 'CPU':   { const float table[] = { [50]=60,[60]=100,[100]=120,[120]=150,[150]=200,[200]=400,[400]=50 };
                                 ZX_FPSMUL = table[(int)(ZX_FPSMUL)]; if(cmdarg_) ZX_FPSMUL=atoi(cmdarg_); }

            break; case 'HELP':  help();

            break; case 'SCAN':  for( const char *f = cmdarg_ && cmdarg_[0] ? cmdarg_ : app_selectfolder("Select games folder", ZX_FOLDER); f ; f = 0 ) {
                                    rescan( ZX_FOLDER ), /*browser = !!numgames,*/ ui_dialog_new(NULL);
                                }

            break; case 'DEVT': ZX_DEVTOOLS ^= 1;

            break; case 'LINK': visit(cmdarg_);

            break; case 'F01': vk_redefine(1);
            break; case 'F02': vk_redefine(2);
            break; case 'F03': vk_redefine(3);
            break; case 'F04': vk_redefine(4);
            break; case 'F05': vk_redefine(5);
            break; case 'F06': vk_redefine(6);
            break; case 'F07': vk_redefine(7);
            break; case 'F08': vk_redefine(8);
            break; case 'F09': vk_redefine(9);
            break; case 'F0:': vk_redefine(10);
            break; case 'F0;': vk_redefine(11);
            break; case 'F0<': vk_redefine(12);

            break; case 'LIST': { // cmdarg in "#ID" format. @todo: add LIST redefineable cmd
                int id = atoi(cmdarg_+1);
                zxdb z = zxdb_search(cmdarg_, 0);

                int count = 0;
                char *list[256] = {0};

                const char *url; int slot = -1;
                ui_dialog_new("- Select media -");
                do {
                    url = zxdb_url(z, va("%d",++slot));
                    if(!url) continue;
                    puts(url);

                    const char *base = strrchr(url, '/')+1;

                    // common game images here only
                    // exclude wav/mp3/ay/html/pdf/txt/pok/rzx from listing
                    int supported = 0;
                    if(!supported) supported |= !!strendi(base, ".sna.zip");
                    if(!supported) supported |= !!strendi(base, ".z80.zip");
                  //if(!supported) supported |= !!strendi(base, ".szx.zip"); // not yet fully supported (see: RaidAereoEm1934.szx.zip)
                  //if(!supported) supported |= !!strendi(base, ".rzx.zip"); // not yet fully supported
                    if(!supported) supported |= !!strendi(base, ".tap.zip");
                    if(!supported) supported |= !!strendi(base, ".tzx.zip");
                    if(!supported) supported |= !!strendi(base, ".pzx.zip"); // (see: Duel2.pzx.zip)
                    if(!supported) supported |= !!strendi(base, ".rom.zip"); // (see: MagTheMagician.rom.zip)
                    if(!supported) supported |= !!strendi(base, ".dsk.zip");
                    if(!supported) supported |= !!strendi(base, ".trd.zip");
                    if(!supported) supported |= !!strendi(base, ".scl.zip"); // (see: Seraphima.scl.zip)
                    if(!supported) supported |= !!strendi(base, ".fdi.zip"); // (see: BlackRaven.fdi.zip)
                    if(!supported) continue;

                    if( strstri(base, "(MasterTape)") ) continue; // wat (see: Nexor)
                    if( strstri(base, "(MasterDisk)") ) continue; // wat (see: BlinkysScarySchool)

#if 0
                    (see: Cabal.ay.zip)
                    (see: AirborneRanger.html.zip)
                    (see: EliteLegend_Info.pdf.zip)
                    
                    (see: JackTheNipper.rzx.zip)
                    (see: FiremanSam_2.slt.zip)
                    (see: DungDarach.wav.zip)
                    (see: AfterBurner.mp3.zip)

                    (see: TheRingOfTheInka.mdr.zip or WhereTimeStoodStill)
                    (see: TheRingOfTheInka.mdv.zip)
                    (see: TheRingOfTheInka.if1.zip)

                    (see: CivilServiceII.mgt.zip)
                    (see: FallingDown.d80.zip)
                    (see: PhantomF4II.d40.zip)

                    (see: ShootOutZX80.o.zip)
                    (see: NowotnikPuzzleThe_3.p.zip)
#endif

                    // Myth-HistoryInTheMaking(MCMSoftwareSA)(SideA).dsk.zip > <DSK|(MCMSoftwareSA)(SideA)|#id#slot
                    char pretty[256]; snprintf(pretty, 256, "%.*s", (int)(strlen(base) - 4), base); // remove zip/rar
                    char ext[8]; snprintf(ext, 8, "%s", strrchr(pretty, '.') + 1); // find ext
                    for( char *s = ext; *s; s++) *s = toupper(*s);
                    const char *tags = strlen(base) > 53/*(_320/(8+1))*/ && strchr(base, '(') ? strchr(base, '(') : base;
                    list[count++] = va("<%s|%.*s|#%d#%d", ext, (int)(strlen(tags)-8*(tags!=base)),tags, id, slot ); // compose ext|name|#id

                } while(url);

                // cancel
                if( count == 0 ) ui_dialog_new(NULL);
                // sort and queue
                if( count > 0 ) {
                    qsort(list, count, sizeof(char *), qsort_strcmp);

                    unsigned hits = -1;
                    unsigned type1 = *((unsigned *)list[0]);

                    for( int i = 0; i < count; ++i ) {
                        puts(list[i]);

                        unsigned type2 = *((unsigned *)list[i]);
                        int sameline = type1 == type2;
                        type1 = type2;

                        char *ext = list[i];
                        char *name = strchr(ext, '|'); *name++ = '\0';
                        char *zxid = strchr(name, '|'); *zxid++ = '\0';

                        const char *romans[] = {
                            ext, "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
                        };
                        hits = (hits + 1) * sameline;
                        if( !sameline ) ui_dialog_separator();
                        ui_dialog_option(1, romans[hits], name, 'ZXDB',zxid);

                    }

                    ui_dialog_separator(), ui_dialog_separator();

                    ui_dialog_option(1,1/*( ZX_AUTOLOCALE)*/+"\5"EJECT_STR"\fEject\f\f",NULL,'WIPE',0);

                    ui_dialog_cancel();
                }

                zxdb_free(z);
            }

            break; case 'ZXDB': {

                // @fixme: make a char *zxdb_v2_url(id); util func instead
                if( ZX == 300 ) {
                    char *prev = va("%s", ZX_MEDIA); if(strchr(prev,'@')) *strchr(prev,'@') = 0;
                    int insert_next_disk_or_tape = are_sequential_urls(zxdb_url2(prev), zxdb_url2(cmdarg_));

                    load_should_clear = insert_next_disk_or_tape ? 0 : 1;
                }

                // eject();
                if( load(cmdarg_, 0) ) {
                	if( ZX_MEDIA != cmdarg_ )
                    (free(ZX_MEDIA), ZX_MEDIA = strdup(cmdarg_));
                    // update titlebar
                    if( ZXDB.ids[0] )
                        titlebar( ZXDB.ids[2] );
                    // small addendum for tape files that require a separate rom download
                    /**/ if( !strcmp(cmdarg_,"#4424") ) cmdarg_ = "#4424#10"; // shadow of the unicorn
                    else cmdarg_ = 0;
                    // load roms
                    if( cmdarg_ ) {
                        int len; char *bank = zxdb_download2(cmdarg_, &len);
                        zxdb_unpack2(&bank, &len);
                        if( bank && len && len < 65536 ) memcpy(rom, bank, len);
                        if( bank ) free(bank);
                        ZX_KLMODE = 0, ZX_TURBOROM = 0; // avoid further patches
                        z80_reset(&cpu);
                    }
                    // if not rom, and only a #NUMBER, display a menu for media selection
                    if( !cmdarg_ && strcnt(ZX_MEDIA,'#') == 1 ) {
                        cmdkey = 'LIST', cmdarg = va("#%d", atoi(ZX_MEDIA+1));
                    }
                } else {
                    // show error dialog
                    ui_dialog_new("Cannot load media");
                    ui_dialog_ok();
                }
            }

            break; case 'PAL0':
                if( cmdarg_ &&  strcmp(cmdarg_, "^1") ) ZX_PALETTE = atoi(cmdarg_) % countof(ZXPalettes);
                if( cmdarg_ && !strcmp(cmdarg_, "^1") ) ZX_PALETTE_PREVIEW ^= 1;

                int ext = countof(ZXPaletteNames) - 1, is_ext_loaded = !!ZXPalettes[ext][1]; // !!is_file(".Spectral/Spectral.pal");

                ui_dialog_new("- Toggle Palette -");
                for( int i = 0; i < ext; ++i)
                ui_dialog_option(1|4,va((ZX_PALETTE!=  i)+"\005%s%s",2+ZXPaletteNames[i  ],strendi(ZXPaletteNames[i  ],"\n") ? "":" "), NULL,'PAL0',va("%d",i));
                ui_dialog_option(1|4,va((ZX_PALETTE!=ext)+"\005%s%s",2+ZXPaletteNames[ext],strendi(ZXPaletteNames[ext],"\n") ? "":" "), NULL,is_ext_loaded ? 'PAL0':'PALB',va("%d",ext));
                ui_dialog_option(1|4,FOLDER_STR"\n\n","- Browse external .pal palette -",'PALB',NULL);

                ui_dialog_checkbox2(&ZX_PALETTE_PREVIEW,"Editor  ","- Display Palette editor -",NULL);

                ui_dialog_option(1,"OK\n",0,'PAL',va("%d", ZX_PALETTE));

            case 'PAL1':
                cmdkey = 'PAL1';
                palette_use(ZX_PALETTE);

            break; case 'JOY0': {
                // 0: no, |1: cursor/protek/agf, |2: kempston, |4: sinclair1, |8: sinclair2, |16:fuller, |32:kempston2, >=256:... custom mappings

                const char *buttons[] = { "□\f\f", "□\b\b\b\b\b\b\b\b\5√\7\f\f\f" };

                #define UI_JOYSTICK(name,condition,toggle,set,hint) \
                ui_dialog_option(1|4,va("<%s", buttons[!!(condition)]),NULL,'JOY',toggle); \
                ui_dialog_option(1|4,va("%s\n", name),hint,'JOY',set);

                ui_dialog_new("- Toggle Joysticks -");
                UI_JOYSTICK("Fuller",     ZX_JOYSTICK&16,"^16","=16", "-Less common interface with unique key mapping-");
                UI_JOYSTICK("Kempston",   ZX_JOYSTICK& 2, "^2", "=2", "-Most popular interface with wide game support-");
                UI_JOYSTICK("Kempston B", ZX_JOYSTICK&32,"^32","=32", "-Additional Kempston interface in Port 55-");
                UI_JOYSTICK("Sinclair 1", ZX_JOYSTICK& 4, "^4", "=4", "-Left port on Interface 2, mapped to keys 1:5-");
                UI_JOYSTICK("Sinclair/Interface 2", ZX_JOYSTICK& 8,"^8","=8", "-Right port on Interface 2, mapped to keys 6:0-");
                UI_JOYSTICK("Cursor (Protek, AGF)",ZX_JOYSTICK& 1, "^1", "=1", "-Early interface with BASIC arrow keys mapping-");
                UI_JOYSTICK("No joystick\n",ZX_JOYSTICK==0,"=0","=0", "-Disable joysticks-");

                ui_dialog_combo(1,"<" FIRE_STR " Autofire \5Off|" FIRE_STR " Autofire \5slow|" FIRE_STR " Autofire \5fast|" FIRE_STR " Autofire \5faster","-Select joystick autofire rate-",&ZX_JOYSTICK_AUTOFIRE,0,3);
                ui_dialog_separator();
                ui_dialog_separator();

                ui_dialog_separator();
                ui_dialog_option(1|4," Bindings\f\f","-Remap joystick bindings-",'PAD0',NULL);
                ui_dialog_option(1|4,"Preset\f\f","-Reset to default configuration-",'JOY',"=19");
                ui_dialog_option(1,"OK\n",NULL, 0,NULL);
            }

            break; case 'JOY':
            {
                if( cmdarg_ ) { // toggles
                    unsigned m = atoi(cmdarg_+1);
                    if(cmdarg_[0]=='=') ZX_JOYSTICK = m;
                    if(cmdarg_[0]=='^') ZX_JOYSTICK ^= m; // (ZX_JOYSTICK & ~m) | ((ZX_JOYSTICK & m) ^ m); // '^' case
                    cmdkey = 'JOY0';
                } else { // cycle
                    // ZX_JOYSTICK = (ZX_JOYSTICK+1)%4; if(ZX_JOYSTICK==3) ZX_JOYSTICK=1|2|16;
                    int next[] = {[0]=4,[4]=8,[8]=1,[1]=2,[2]=1|2|16,[1|2|16]=0,[255]=0};
                    ZX_JOYSTICK = next[ZX_JOYSTICK];
                }
                if(ZX_JOYSTICK) ZX_MOUSE = 0; // prevent gunstick/lightgun conflict
            }

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

                ui_dialog_checkbox2(&ZX_HORACE, "Display screenmate\n", "- Keep Horace around -", NULL );

                ui_dialog_separator();
                ui_dialog_option(1|4,"Preset ",0,'PAD3',NULL);
                ui_dialog_option(1,"Cancel ",0,'PAD2',NULL);
                ui_dialog_option(1,"OK\n",0,0,NULL);

                cmdkey = 'PAD1';
            }

            break; case 'HOR2': // toggle horace (port2) @todo
            break; case 'HOR1': // toggle horace (port1)
            ZX_HORACE ^= 1; if( cmdarg_ ) ZX_HORACE = atoi(cmdarg_);

            break; case 'PAD1': // tick remap
            {
                cmdkey = 'PAD1'; // repeat

                // num_buttons
                enum { _12 = countof(ZX_PAD) };

                int vk = -1;
                // scan keyboard
                char *keys = tigrKeys(app);
                for( int i = 0; i < 256; ++i ) {
                    // if pressed...
                    if( !!keys[i] > !!keys[i+256] ) {
                        // if not already registered, select this vk
                        int registered = 0;
                        for( int j = 0; j < _12; ++j ) {
                            registered |= ZX_PAD[j] == i;
                        }
                        if( !registered ) {
                            vk = i;
                            break;
                        }
                    }
                }
                // scan gamepad
                if( vk < 0 ) {
                    unsigned pad = gamepad();
                    if(pad&1) vk = TK_LEFT;
                    if(pad&2) vk = TK_RIGHT;
                    if(pad&4) vk = TK_UP;
                    if(pad&8) vk = TK_DOWN;
                    if(pad&(8|16|32|64)) vk = TK_TAB;

                    if( vk >= 0 )
                    for( int j = 0; j < _12; ++j ) {
                        if( ZX_PAD[j] == vk ) {
                            vk = -1;
                            break;
                        }
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
                static const char *keyicons[256] = {0}, **keysallowed = keyicons;
                do_once {
                    static char ascii[256][2] = {0};
                    for( int i = 32; i < 128; ++i ) {
                        ascii[i][0] = i;
                        keyicons[i] = ascii[i];
                    }
                    keyicons[TK_SPACE] = "□";
                    keyicons[TK_LEFT] = "🮤";
                    keyicons[TK_RIGHT] = "🮥";
                    keyicons[TK_UP] = "🮧";
                    keyicons[TK_DOWN] = "🮦";
                    keyicons[TK_TAB] = ""; //"";
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

                // apply (1x8 chars + 2x4 chars)
                if( option >= 0 && vk >= 0 && (vk == TK_BACKSPACE || keysallowed[vk]) ) {

                    // add key to mappings. terminate if no more room.
                    int slot = 0; while(ZX_PAD[slot] && slot < _12) slot++;
                    if( slot < _12 ) ZX_PAD[slot] = vk;

                    // delete last mapping if requested
                    if( vk == TK_BACKSPACE ) {
                        if(slot < _12 ) ZX_PAD[slot] = 0;
                        if(slot > 0) ZX_PAD[slot-1] = 0;
                    }
                }

                if( key_longpress(TK_BACKSPACE) )
                    memset(ZX_PAD, 0, sizeof(ZX_PAD));

                if( option >= 0 )
                {
                    // redraw mappings
                    const char *cursor = "▁";
                    char buf[256], *ptr = buf;
                    for( int i = 0; i < _12; ++i ) {
                        if( ZX_PAD[i] ) {
                            if( i < 8 )
                            ptr += sprintf(ptr, " %s", keyicons[ZX_PAD[i]]);
                            else
                            ptr += sprintf(ptr, " %s.", keyicons[ZX_PAD[i]]);
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

            break; case 'PAD2': // cancel remap
                memcpy(ZX_PAD, ZX_PAD_, sizeof(ZX_PAD_));
                //ZX_HORACE = 0;

            break; case 'PAD3': // preset remap
                memcpy(ZX_PAD, ZX_PAD_PRESET_, sizeof(ZX_PAD_));
                cmdkey = 'PAD0';

            break; case 'MP3P': // mp3 play
                {
                    void *mp3data; int mp3len;
                    sscanf(cmdarg_, "%p %d", &mp3data, &mp3len);

                    drmp3_uint64 mp3_fc;
                    drmp3_config mp3_cfg = {0};
                    for( float *fbuf = drmp3_open_memory_and_read_pcm_frames_f32(mp3data, mp3len, &mp3_cfg, &mp3_fc, NULL); fbuf ; fbuf = 0 ) { // @fixme: @leak
                        int channels = mp3_cfg.channels;
                        int frequency = mp3_cfg.sampleRate;
                        int supported = channels <= 2 && (frequency == 44100 || frequency == 22050);

                        if( !supported ) alert(va("Unsupported MP3 audio stream, %dch %dHz", channels, frequency));
                        if( !supported ) { free(fbuf); continue; }

                        int length16 = mp3_fc;
                        void *data16 = fbuf;

                        // [x] 2ch 44 Everyones a Wally
                        // [x] 1ch 44 SideArms
                        // [x] 1ch 22 Outrun

                        if( channels == 1 ) {
                            if( frequency == 44100 ) { // downsample to 22khz
                                float *s = data16;
                                for( int i = 0; i < length16/2; ++i ) {
                                    s[i] = (s[i*2+0] + s[i*2+1]) / 2;
                                }
                            }
                            play_stream('mp3', data16, length16 / 2, -1);
                        }
                        if( channels == 2 ) { // downsample to mono
                            float *s = data16;
                            for( int i = 0; i < length16; ++i ) {
                                s[i] = (s[i*2+0] + s[i*2+1]) / 2;
                            }
                            if( frequency == 44100 ) { // downsample to 22khz
                                float *s = data16;
                                for( int i = 0; i < length16; ++i ) {
                                    s[i] = (s[i*2+0] + s[i*2+1]) / 2;
                                }
                            }
                            play_stream('mp3', NULL,0, 0); // stop voice
                            play_stream('mp3', data16, length16 / 2, -1);

#if 0
                            // emit a warning if not played in ideal conditions
                            if( tape_feeding() && ZX_AY > 0 ) {
                                ui_dialog_new("- Warning -");
                                ui_dialog_option(0,"Consider playing the audio track\nwhile the tape is stopped.\n",NULL,0,NULL);
                                ui_dialog_ok();
                            }
#endif
                        }
                    }
                }

            break; case 'MP3M': // mp3 mute
               play_stream('mp3', NULL,0, 0); // stop voice

            break; case 'MP3L': // mp3 list
                ui_dialog_new("-Select track-");

                for( int len = 0; len >= 0; len = -1)
                for( char *url = zxdb_url(ZXDB,"mp3"); url; url = 0)
                for( char *data = zxdb_download(ZXDB, url, &len); data; free(data), data = 0 ) {
                    // zxdb_unpack2(&data, &len);
                    if( len > 2 && data[0] == 'P' && data[1] == 'K' ) {
                        writefile(".Spectral/$$mp3.zip", data, len);

                        zip *z = zip_open(".Spectral/$$mp3.zip","rb");
                        if( z ) {
                            for( int i = 0; i < zip_count(z); ++i ) {
                                const char *fname = zip_name(z,i);
                                if(!strendi(fname, ".mp3")) continue;

                                int unlen = zip_size(z, i);
                                void *unzipped = zip_extract(z, i); // @leak

                                ui_dialog_option(1,va("Track %02d\n",i+1),fname,'MP3P',va("%p %d",unzipped,unlen));
                            }
                            zip_close(z);
                        }

                        unlink(".Spectral/$$mp3.zip");
                    } else {
                        ui_dialog_option(1,va("Track %02d\n",1),strrchr(url,'/')+1,'MP3P',va("%p %d",data,len));
                        data = 0; // @leak
                    }
                }

                ui_dialog_separator();
                ui_dialog_option(1,"Cancel\n",NULL,'MP3M',0);

        }

    // shutdown zxdb browser if user is closing window app. reasoning for this:
    // next z80_opdone() will never finish because we dont tick z80 when zxdb browser is enabled
    // browser *= !window_closed();

    #if 0 && NEWCORE
    // ensure there is no pending opcode before exiting main loop: spectral.sav would be corrupt otherwise.
    // also, do not loop indefinitely on invalid DI+HALT combo, which we use in our .SCR viewer.
    // update: moved logic that bypasses z80_opdone(&cpu) in HALT state. rewritten as a forced/benign ZX_Key(ZX_SHIFT) operation; see: input() function.
    // printf("%d %018llx %d %d %d\n", z80_opdone(&cpu), cpu.pins, cpu.step, IFF1(cpu), IFF2(cpu));
    } while( z80_opdone(&cpu) ? 0 : 1 ); // (cpu.pins & Z80_HALT) && (cpu.step == 1) ? 0 : 1 ); // (cpu.pins & Z80_HALT) && (cpu.step == 1 || cpu.step==_z80_special_optable[4] || cpu.step==_z80_special_optable[5]) ? 0 : 1 );
    #endif

    } while( 1 ); // !window_closed() );

    // restore os keys
    enable_os_keys(1);

    // zxplayer does not save state
    if(!ZX_PLAYER)

    {
        // export state
        // media_seek[0] = voc_pos / (double)(voc_len+!voc_len);
        for( FILE *state = fopen("Spectral.sav","wb"); state; fclose(state), state = 0) {
            if( !export_state(state) )
                alert("Error exporting state");
        }

        // export config
        ZX = ZX | ZX_PENTAGON;
        if(ZX_MEDIA && ZX_MEDIA[0]) ZX_MEDIA = strdup(va("%s@%llu", ZX_MEDIA, media_seek[0] = voc_pos)); // @leak
        save_config();
    }

    tigrFree(app);
    return 0;
}

void on_cmd(unsigned key, const char *arg) {
    cmdkey = key;
    cmdarg = arg;
}

int gui(const char *status) {
    if( DEV ) {
        float x = 0.5, y = 0.5; // 23.5;
        ui_print(ui,  x*11-2, y*11-1, ui_colors, status);
    }

    if( ZX_DEBUG ) {
        tigrClear(dbg, tigrRGBA(0,0,0,128));

        float x = 0.5, y = 2;

        if( !DEV )
        ui_print(dbg, x*11, y*11, ui_colors, status), y += 1.5;

        ui_print(dbg, x*11, y*11, ui_colors, regs(0)), y += 5;

        ui_print(dbg, x*11, y*11, ui_colors, dis(PC(cpu), 22)), y += 22;
    }

    // audio, stats & debug
    if( !browser && !do_overlay ) {
        if( ZX_WAVES && ZX_AY > 0 ) {
            int count = _320;
            int tail = audio_write - count;
            if( tail < 0 ) tail = 0;
            float *audioch[1+3+3] = { 
                audio_buffer1+tail, // BEEPER
                audio_buffer2+tail, audio_buffer3+tail, audio_buffer4+tail, // AY1
                audio_buffer5+tail, audio_buffer6+tail, audio_buffer7+tail, // AY2
            };
            draw_audio(ui, 1+3*(ZX > 48)+3*ZX_PENTAGON, audioch, count);
        }
        if( ZX_LENSLOK && !num_options ) {
            if( mouse().rb ) ZX_LENSLOK = 0;
            else lenslok(ui, canvas, mouse_offsets()[0], mouse_offsets()[1] );
        }
    }

    int ui_click_bak = ui_click;
    if( num_options ) ui_click = 0; // cancel further ui actions as long as dialog exists
    draw_ui();
    if( ui_click_bak ) ui_click = ui_click_bak;

    if( ZX_DEBUG )
    tigrBlitAlpha(app, dbg, 0,0, 0,0, _320,_240, 1.0f);

    // flush ui
    ui_frame_end();

    // draw ui on top
    tigrBlitAlpha(app, ui, 0,0, 0,0, _320,_240, 1.0f);

    // draw overlay on top
    if( do_overlay ) {
        struct mouse m = mouse();
        tigrRenderMap(app, overlay, m.x, m.y, m.buttons, -m.wheel);
    }

    // draw chat on top
    if( ZX_LOBBY ) {
        chat_draw(app);
    }

    // render dialog modal on top
    if( ui_dialog_render(dialog, mouse().wheel) ) {
        // composite result
        tigrBlitAlpha(app, dialog, 0,0, 0,0, _320,_240, 1.0f);
    }

    // companion screenmate, aka co-op for kids. used to be a test for gamepad bindings
    // @todo: coyote time
    // @todo: allow jumping right before touching the ground
    if( ZX_HORACE ) {
        int _230 = _240 - 2 * ZX_CRT; //-10;

        const char *smile = "";
        const char *stand = ""; // alternate 
        const char *walk[] = {"","","",""}; // 4-frame walk
        const char *jump[] = {"",""}; // ascending/descending frames
        const char *duck = "";

        static unsigned bgcolors[8];
        static float gx, gy, bt = 0, mv, dn;
        static float vy = 0; // vertical velocity
        static float is_jumping = 0; // jump state
        static float ground, mirror = 0;
        do_once gx = _320/4, gy = -10, ground = _230;
        mv = 0, dn = 0;

        static int dragx, dragy, drag;
        if( !drag && mouse().lb && fabsf(mouse().x - gx) < 8 && fabsf(mouse().y - gy + 8) < 8 ) {
            drag = 1;
            dragx = mouse().x - gx;
            dragy = mouse().y - gy;
        }
        if( drag && mouse().lb ) {
            mouse_cursor(2); // hand

            float nx = mouse().x - dragx;
            if( nx != gx ) mirror = nx < gx;
            gx = mouse().x - dragx;
            gy = mouse().y - dragy;
            dn = 1, is_jumping = vy = 0;
        } else {
            drag = 0;
        }

        // Other inputs
        if( TK_DOWN ) if( tigrKeyHeld(app, TK_DOWN) ) dn = 1;
        if( TK_UP ) bt = tigrKeyHeld(app, TK_UP);

        // Horizontal movement (using ZX_PAD[0] for left, ZX_PAD[1] for right)
        if(!dn) {
        if( tigrKeyHeld(app, TK_LEFT) ) gx -= 2, mv = 1, mirror = 1;
        if( tigrKeyHeld(app, TK_RIGHT) ) gx += 2, mv = 1, mirror = 0;
        }

        // Jump logic (using ZX_PAD[2] for jump)
        if( tigrKeyDown(app, TK_TAB) && !is_jumping && gy >= ground ) {
            memset32(bgcolors, 0, countof(bgcolors));

            if( dn ) {
                // duck: consider all colors on ground as transparent and fall down.
                for( int x = -4; x < 4; ++x) {
                    bgcolors[x+4] = app->pix[(int)((gx+x)+gy*_320)].rgba;
                }
            }
            else {
                vy = -6.0f; // Initial upward velocity (tuned for quick jump)
                is_jumping = 1;
            }

        }

        // Vertical movement
        if( is_jumping ) {
            vy += 0.250f; // Apply gravity (tuned for snappy feel)
            gy += vy; // Update y position
            // Hit the ground
            if( gy >= ground ) {
                gy = ground;
                vy = 0;
                is_jumping = 0;
            }
        }

        // disable platforms while in browser mode
        if( browser ) ground = _230, is_jumping = gy != ground;
        else

        // check ground: descending or standing
        if( is_jumping ? vy >= 0 : 1 ) {

            // 3 states: loading, gaming or typing (in basic)
            extern int tape_hz;
            int typing = PC(cpu) < 0x4000 && is_basic_mode();
            int loading = tape_playing() && tape_hz > 200; // (PC(cpu) & 0xFF00) == 0x0500 ? 1 : tape_playing() && !typing;
            int gaming = !loading && !typing;

            static int typing_hz; // fast increase, slow decay
            if( typing ) ++typing_hz; else typing_hz *= 0.95;

            int ATTR_P = READ8(0x5C8D); // FLASH:1|BRIGHT:1|PAPER:3|INK:3
            int BRIGHT = !!(ATTR_P & 32);
            int PAPER = (ATTR_P >> 3) & 7;
            int PAPER_PAL = PAPER + 8 * BRIGHT;

            unsigned hit = 0;
            for( int y = gy+0; y < _230 && !hit; ++y ) {
                ground = y;
                if(y>=0)
                for( int x = -4; x < 4; ++x ) {
                    unsigned *fg = &app->pix[(int)(gx + x + y * _320)].rgba;

                    unsigned r1 = *fg;
                    unsigned b1 = (r1 >> 16) & 255;
                    unsigned g1 = (r1 >>  8) & 255;
                             r1 = (r1 >>  0) & 255;

                    // Compare foregound color against background color
                    #define fgcheck(bg) if(bg) { \
                        unsigned r2 = bg; \
                        unsigned b2 = (r2 >> 16) & 255; \
                        unsigned g2 = (r2 >>  8) & 255; \
                                 r2 = (r2 >>  0) & 255; \
                        unsigned diff = (r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2); \
                        if( diff <= (40*40) ) continue; \
                    }

                    fgcheck(ZXPalette[ZXBorderColor]);

                    if( typing_hz )
                    fgcheck(ZXPalette[PAPER_PAL]);

                    fgcheck(bgcolors[0]);
                    fgcheck(bgcolors[1]);
                    fgcheck(bgcolors[2]);
                    fgcheck(bgcolors[3]);
                    fgcheck(bgcolors[4]);
                    fgcheck(bgcolors[5]);
                    fgcheck(bgcolors[6]);
                    fgcheck(bgcolors[7]);

                    hit = *fg; break;
                }
            }

            // disable platforms on borders while loading from tape
            if( loading ) {
                // still, allow horace to climb up UI interface
                if( hit != ui_ff.rgba ) {
                    if( gy >= _240-_24 ) ground = _230;
                    if( gy < _24 ) ground = _24;
                    if( (gx < _32 || gx >= (_320-_32)) ) ground = _230;
                }
            }

            if( gy < ground ) is_jumping = 1;
            else
                memset32(bgcolors, 0, countof(bgcolors)); // clear palette

            //tigrFill(app, gx-4,gy, 8,ground-gy, tigrRGB(255,0,255) );
        }


        // Keep sprite in bounds
        gx = fmod(gx + _320, _320);
        gy = tigrClamp(gy, 0, _240);

        // Select sprite
        const char *sprite = dn ? duck :
            is_jumping ? jump[vy >= 0] :
            mv ? walk[(int)fmod(gx/6, 4)] : bt ? stand : smile;

        ui_mirror = mirror;
        ui_print(app, gx - 4, gy - 8, ui_colors, sprite);
        //tigrPlot(app, gx, gy, tigrRGB(255,0,0));

        // display keypresses if needed (binding screen)
        if( 1 ) { // cmdkey == 'PAD0' || cmdkey == 'PAD1' ) {

            static const char *keyicons[256] = {0};
            do_once {
                static char ascii[256][2] = {0};
                for( int i = 32; i < 128; ++i ) {
                    ascii[i][0] = i;
                    keyicons[i] = ascii[i];
                }
                keyicons[TK_SPACE] = "□";
                keyicons[TK_LEFT] = "⭠";//🮤";
                keyicons[TK_RIGHT] = "⭢";//🮥";
                keyicons[TK_UP] = "⭡";//🮧";
                keyicons[TK_DOWN] = "⭣";//🮦";
                keyicons[TK_TAB] = ""; //"";
            }

            const char *keypress = 0;

            for( int i = 0; i < countof(ZX_PAD); ++i ) {
                if( ZX_PAD[i] && tigrKeyHeld(app, ZX_PAD[i]) ) {
                    keypress = keyicons[ZX_PAD[i]];
                }
            }
            
            if( keypress )
                ui_print(app, gx + 4, gy - 16, ui_colors, keypress);
        }
    }

    return 1;
}

int rec(Tigr *canvas) {
    // screenshots
    if( cmdkey == 'PIC2' ) {
        screenshot( window_title(NULL), canvas );
        play('cam', 1);
    }
    // videos
    if( cmdkey == 'REC2' ) {
        /**/ if( record_enabled() ) record_stop(), play('tape', 0);
        else switch( record_start(va("%s-%04x.mp4", window_title(NULL), file_counter(SLOT_MP4)), window_width(), window_height(), 50) ) {
                default: alert("Cannot record video");
                break; case 2: play('tape', 1);
                break; case 1: alert("FFMPEG" ifdef(win32,".exe","") " not " ifdef(win32,"found","installed") ".\nUsing suboptimal MPEG1 encoder.\n"); play('tape', 1);
        }
    }
    // video
    if( record_enabled() ) {
        record_frame( canvas->pix, canvas->w, canvas->h );
    }
    return 1;
}

void loggers(int m) {

    loggers_(m);

#ifdef _WIN32

    loggers_(0);

    HWND hConsole = GetConsoleWindow();
    if (hConsole) {
        ShowWindow(hConsole, SW_HIDE);
    }
    FreeConsole();

    if( m )
    {
        AllocConsole();

        enable_ansi();

        // Force UTF-8 codepage (Windows 10 build 1903+)
        SetConsoleCP(65001/*CP_UTF8*/);       // for ReadConsoleA / input
        SetConsoleOutputCP(65001/*CP_UTF8*/); // for WriteConsoleA / printf

        // Re-attach the streams
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$",  "r", stdin);

        // Strongly recommended with UTF-8: Unbuffered so output is immediate
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        loggers_(1);
    }

#endif

    if( m ) {
        void logo();
        logo();
        puts("open console");
    }
}
