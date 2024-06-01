#define SPECTRAL "v0.7 wip+"

#define README \
"Spectral can be configured with a mouse.\n" \
"Here are some keyboard shortcuts, though:\n" \
"- ESC: Game browser\n" \
"- F1: CPU throttle (hold)\n" \
"- F2: Start/stop tape\n" \
"- F3/F4: Rewind/advance tape\n" \
"- F5: Reload game\n" \
"- F6: Toggle input latency (Run-a-head)\n" \
"- F7: Toggle keyboard issue 2/3\n" \
"- F8: Toggle tape speed\n" \
"- F9: Toggle TV/RF (4 modes)\n" \
"- F9+SHIFT: Toggle AY core (2 modes)\n" \
"- F11/F12: Quick save/load\n" \
"- ALT+ENTER: Fullscreen\n" \
"- TAB+CURSORS: Joysticks\n"

// # build (windows)
// cl run.c /O2 /MT /DNDEBUG /GL /GF /arch:AVX2
//
// # build (linux)
// sudo apt-get install mesa-common-dev libx11-dev gcc libgl1-mesa-dev
// gcc run.c -Wno-unused-result -Wno-format -lm -ldl -lX11 -lpthread -lGL -O3
//
// ref: http://www.zxdesign.info/vidparam.shtml
// https://worldofspectrum.org/faq/reference/48kreference.htm
// https://faqwiki.zxnet.co.uk/wiki/ULAplus
// https://foro.speccy.org/viewtopic.php?t=2319

//
// done
// cpu, ula, mem, rom, 48/128, key, joy, ula+, tap, ay, beep, sna/128, fps, tzx, if2, zip, rf, menu, kms, z80, scr,
// key2/3, +2a/+3, fdc, dsk, autotape, gui, KL modes, load "" code, +3 fdc sounds, +3 speedlock,
// pentagon, trdos,
// [x] glue sequential tzx/taps in zips (side A) -> side 1 etc)
// [x] sequential tzx/taps/dsks do not reset model

// @todo:
// [ ] ui_inputbox()
// [ ] widescreen fake borders
// [ ] animated states
// [ ] auto-saves, then F11 to rewind. use bottom bar
// [ ] scan folder if dropped or supplied via cmdline
// [ ] live coding disasm (like bonzomatic)
// [ ] convert side-b/mp3s into voc/pulses
// [ ] db interface [optionally: y/n/why]
//     [hearts] NUM. Title(F2 to rename)   [ghost-load*][ghost-run*][ghost-play*][ghost-snd*] [*:white,red,yellow,green]
//     on hover: show animated state if exists. show loading screen otherwise.
// [ ] es2en auto-translations:
//     - auto-translation IZQUIERD. DERECH. ABAJ. ARRIB. SALTA. DISPAR. FUEG. PAUS. ABORT.
//     - INSTRUCCIONE. TECLAD.
//
// todo (tapes)
// [ ] remove prompt() calls. create ui_inputbox() instead
// [ ] overlay ETA
// [ ] auto rewind
// [ ] auto-rewind at end of tape if multiload found (auto-stop detected)
// [ ] auto-insert next tape at end of tape (merge both during tzx_load! argv[1] argv[2])
// [ ] when first stop-the-tape block is reached, trim everything to the left, so first next block will be located at 0% progress bar
// [ ] trap rom loading, edge detection

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

// [ ] glue consecutive tzx/taps in disk
// [ ] glue: do not glue two consecutive tapes if size && hash match (both sides are same)
// [ ] glue: if tape1 does not start with a basic block, swap tapes
// [ ] prefer programs in tape with "128" string on them (barbarian 2; dragon ninja)
//     - if not found, and a "48" string is found, switch model to 48k automatically
// score128: 128 in filename + memcmp("19XY") X <= 8 Y < 5 + sizeof(tape) + memfind("in ayffe") + side b > 48k + program name "128" + filename128  -> load as 128, anything else: load as usr0
//      if single bank > 49k (navy seals), if size(tap)>128k multiload (outrun)
// test autotape with: test joeblade2,atlantis,vegasolaris,amc

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

enum { _320 = 352, _319 = _320-1, _321 = _320+1, _32 = (_320-256)/2 };
enum { _240 = 288, _239 = _240-1, _241 = _240+1, _24 = (_240-192)/2 };

#include "3rd.h"
#include "emu.h"
#include "sys.h"
#include "zx.h"

#define CLAMP(v, minv, maxv) ((v) < (minv) ? (minv) : (v) > (maxv) ? (maxv) : (v))
#define REMAP(var, src_min, src_max, dst_min, dst_max) \
    (dst_min + ((CLAMP(var, src_min, src_max) - src_min) / (float)(src_max - src_min)) * (dst_max - dst_min))

static float dt; 
static double timer;
static int flick_frame;
static int flick_hz;

void draw(window *win, int y /*0..311 tv scanline*/) {
    int width = _32+256+_32;
    rgba *texture = &((rgba*)win->pix)[0 + y * width];

    // int third = y / 64; int y64 = y % 64;
    // int bit3swap = (y64 & 0x38) >> 3 | (y64 & 0x07) << 3;
    // int scanline = (bit3swap + third * 64) << 5;
    #define SCANLINE(y) \
        ((((((y)%64) & 0x38) >> 3 | (((y)%64) & 0x07) << 3) + ((y)/64) * 64) << 5)

    // border left
    for(int x=0;x<_32;++x) *texture++=ZXPalette[ZXBorderColor];
    // screen
    if( !(y >= (0+_24) && y < (192+_24) )) {
    for(int x=0;x<256;++x) *texture++=ZXPalette[ZXBorderColor];
    } else {
        y -= _24;
        byte *pixels=VRAM+SCANLINE(y);
        byte *attribs=VRAM+6144+((y&0xF8)<<2);

// RF: misalignment
*texture = ZXPalette[ZXBorderColor];
enum { BAD = 8, POOR = 32, DECENT = 256 };
int shift0 = !ZX_RF ? 0 : (rand()<(RAND_MAX/(ZX_FASTCPU?2:POOR))); // flick_frame * -(!!((y+0)&0x18))
texture += shift0;

        for(int x = 0; x < 32; ++x) {
            byte attr = *attribs;
            byte pixel = *pixels, fg, bg;

            // @fixme: make section branchless

            if (ulaplus_enabled) {
                fg = ((attr & 0xc0) >> 2) | ((attr & 0x07));
                bg = ((attr & 0xc0) >> 2) | ((attr & 0x38) >> 3) | 8;
            } else {
                pixel ^= (attr & 0x80) && ZXFlashFlag ? 0xff : 0x00;
                fg = (attr & 0x07) | ((attr & 0x40) >> 3);
                bg = (attr & 0x78) >> 3;
            }

            // @fixme: make section branchless

            texture[0]=ZXPalette[pixel & 0x80 ? fg : bg];
            texture[1]=ZXPalette[pixel & 0x40 ? fg : bg];
            texture[2]=ZXPalette[pixel & 0x20 ? fg : bg];
            texture[3]=ZXPalette[pixel & 0x10 ? fg : bg];
            texture[4]=ZXPalette[pixel & 0x08 ? fg : bg];
            texture[5]=ZXPalette[pixel & 0x04 ? fg : bg];
            texture[6]=ZXPalette[pixel & 0x02 ? fg : bg];
            texture[7]=ZXPalette[pixel & 0x01 ? fg : bg];

            texture += 8;

            pixels++;
            attribs++;
        }

texture -= shift0;

    }
    // border right
    for(int x=0;x<_32;++x) *texture++=ZXPalette[ZXBorderColor];

#ifdef DEBUG_SCANLINE
    memset(texture, 0xFF, width * 4);
    //    sys_sleep(1000/60.);
#endif
}

void blur(window *win) {
    int height = _24+192+_24;
    int width = _32+256+_32;

    // screen
    static int j = 0;
    static byte jj = 0;

    for( int y = _24; y < _240-_24; ++y ) {
        rgba *texture = &((rgba*)win->pix)[_32 + y * width];

        for(int x=0;x<_32;x++) {

        int shift = (++jj)&1;
        texture += shift;

                // RF: hue shift
                for(int i = 8-1; i >= 0; --i) {
                    unsigned pix0 = texture[i-0];
                    unsigned pix1 = texture[i-1];
                    unsigned pix2 = texture[i-2];
                    unsigned r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    unsigned r1,g1,b1; rgb_split(pix1,r1,g1,b1);
                    unsigned r2,g2,b2; rgb_split(pix2,r2,g2,b2);
                    if(i&1)
                    texture[i] = rgb((r0+r0+r0+r0)/4,(g0+g0+g0+g0)/4,(b2+b2+b2)/3); // yellow left
                    else
                    texture[i] = rgb((r0+r0+r0+r1)/4,(g0+g0+g0+g1)/4,(b1+b1+b1+b2)/4); // blue left
                    //else
                    //texture[i] = rgb((r0+r1+r1+r1)/4,(g0+g1+g1+g1)/4,(b0+b1+b1)/3);

                    continue;
if(ZX_RF) {
                    // saturate aberrations (very slow)
                    pix0 = texture[i];
                    rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    if( s0 * 1.5 > 255 ) s0 = 255; else s0 *= 1.5;
                    if( v0 * 1.01 > 255 ) v0 = 255; else v0 *= 1.01;
                    texture[i] = as_rgb(h0,s0,v0);
}
                }

                // RF: jailbars
                if(x%2)
                for(int i = 0; i < 8; ++i) {
                    unsigned pix0 = texture[i-0];
                    byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    texture[i] = as_rgb(h0,s0,v0*0.99);
                }

                // RF: interferences
                // interesting tv effects (j): 13, 19, 23, 27, 29, 33
                // note: since CRT shader was introduced this RF effect became less
                // aparent (because of the bilinear smoothing). and that's why we're
                // using 13 now, since the screen stripes it creates are way more visible.
                // used to be 33 all the time before.
                // @fixme: apply this effect to the upper and bottom border as well
                for(int i = 0; i < 8; ++i) { ++j; j%=13; // was 33 before
                    unsigned pix0 = texture[i-0];
                    byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    if(j<1) texture[i] = as_rgb(h0,s0,v0*0.95);
                    else
                    if(j<2) texture[i] = as_rgb(h0,s0,v0*0.95);
                    else
                            texture[i] = rgb(r0,g0,b0);
                }

        texture -= shift;

            texture += 8;
        }
    }
}

void scanlines(window *win) {
    // hsv: slow. may be better in a shader
    int height = _24+192+_24;
    int width = _32+256+_32;
    for(int y = 0; y < _240; y+=2) {
        rgba *texture = &((rgba*)win->pix)[0 + y * width];
        for(int x = 0; x < width; ++x) {
            unsigned pix0 = texture[x];
            byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
            byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
            texture[x] = as_rgb(h0*0.99,s0,v0*0.98);
        }
    }
}

const char *shader = 
#if 0
"/* HSV from/to RGB conversion functions by Inigo Quilez. https://www.shadertoy.com/view/lsS3Wc (MIT licensed)*/\n"
"const float eps = 0.0000001;\n"
"vec3 hsv2rgb( vec3 c ) {\n"
"    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );\n"
"    return c.z * mix( vec3(1.0), rgb, c.y);\n"
"}\n"
"vec3 rgb2hsv( vec3 c) {\n"
"    vec4 k = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);\n"
"    vec4 p = mix(vec4(c.zy, k.wz), vec4(c.yz, k.xy), (c.z<c.y) ? 1.0 : 0.0);\n"
"    vec4 q = mix(vec4(p.xyw, c.x), vec4(c.x, p.yzx), (p.x<c.x) ? 1.0 : 0.0);\n"
"    float d = q.x - min(q.w, q.y);\n"
"    return vec3(abs(q.z + (q.w - q.y) / (6.0*d+eps)), d / (q.x+eps), q.x);\n"
"}\n"

"/* YUV conversions by icalvin102 https://www.shadertoy.com/view/3lycWz */\n"
"vec3 rgb2yuv(vec3 rgb){\n"
"    float y = 0.299*rgb.r + 0.587*rgb.g + 0.114*rgb.b;\n"
"    return vec3(y, 0.493*(rgb.b-y), 0.877*(rgb.r-y));\n"
"}\n"
"vec3 yuv2rgb(vec3 yuv){\n"
"    float y = yuv.x;\n"
"    float u = yuv.y;\n"
"    float v = yuv.z;   \n"
"    return vec3(\n"
"        y + 1.0/0.877*v,\n"
"        y - 0.39393*u - 0.58081*v,\n"
"        y + 1.0/0.493*u\n"
"    );\n"
"}\n"
#endif
    "/* based on code by lalaoopybee https://www.shadertoy.com/view/DlfSz8 */\n"
    "#define CURVATURE 8.2\n"
    "#define BLUR .01\n"
    "#define CA_AMT 1.0024\n"
    "void fxShader(out vec4 fragColor, in vec2 uv){\n"
    "    vec2 fragCoord=uv*vec2(352.0*3.0,288.0*3.0);\n"
#if 1
    "    /* curvature */\n"
    "    vec2 crtUV=uv*2.-1.;\n"
    "    vec2 offset=crtUV.yx/CURVATURE;\n"
    "    crtUV+=crtUV*offset*offset;\n"
    "    crtUV=crtUV*.5+.5;\n"

    "    /* edge blur */\n"
    "    vec2 edge=smoothstep(0., BLUR, crtUV)*(1.-smoothstep(1.-BLUR, 1., crtUV));\n"
#else
    "    vec2 crtUV = uv;\n"
    "    vec2 edge = vec2(1.,1.);\n"
#endif
#if 1
    "    /* chromatic aberration */\n"
    "    fragColor.rgb=vec3(\n"
    "        texture(image, (crtUV-.5)*CA_AMT+.5).r,\n"
    "        texture(image, crtUV).g,\n"
    "        texture(image, (crtUV-.5)/CA_AMT+.5).b\n"
    "    )*edge.x*edge.y;\n"
#else
    "    fragColor = texture(image, crtUV) * edge.x * edge.y;\n"
#endif
#if 0
    "    /* diodes */\n"
    "    if(mod(fragCoord.y, 2.)<1.) fragColor.rgb*=.95;\n"
    "    else if(mod(fragCoord.x, 3.)<1.) fragColor.rgb*=.95;\n"
    "    else fragColor*=1.05;\n"
#endif
    "}\n";

int load_shader(const char *filename) {
    char *data = readfile(filename, NULL);
    if(data) if(strstr(data, " fxShader")) return shader = strdup(data), 1; // @leak
    return 0;
}

#include <time.h>
int screenshot(const char *filename) {
    static byte counter = 0xFF; counter = (counter + 1) % 50;

    time_t timer = time(NULL);
    struct tm* tm_info = localtime(&timer);

    char stamp[32];
    strftime(stamp, 32, "%Y%m%d %H%M%S", tm_info);

    extern window* app;
    int ok1 = writefile(va("%s %s %04x.scr", filename, stamp, counter), VRAM, 6912);
    int ok2 = tigrSaveImage(va("%s %s %04x.png", filename, stamp, counter), app);

    return ok1 && ok2;
}


// command keys: sent either physically (user) or virtually (ui)
int cmdkey;


static byte* merge = 0;
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
        qsort(alpha_tapes, count_tapes, sizeof(char *), qsort_strcmp);

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
            if(strstr(zip_name(z,i),".tap") || strstr(zip_name(z,i),".tzx")) {
            continue;
            }
            break;
        }

        zip_close(z);
        return merge;
    } else {
        printf("error: bad %s zip\n", filename);
    }
    return 0;
}




void crt(int enable) {
    extern window *app;
    if( enable )
    tigrSetPostShader(app, shader, strlen(shader));
    else
    tigrSetPostShader(app, tigr_default_fx_gl_fs, strlen(tigr_default_fx_gl_fs));
}




window *app, *ui, *dbg;
int do_disasm;
float fps;

void input() {
    // keyboard
    ZXKeyUpdate();

    // joysticks
    int up   = window_pressed(app, TK_UP),   down = window_pressed(app, TK_DOWN);
    int left = window_pressed(app, TK_LEFT), right = window_pressed(app, TK_RIGHT);
    int fire = window_pressed(app, TK_TAB);
    ZXJoysticks(up,down,left,right,fire);

    // keyboard
    #define KEYS(k) \
        k(0)k(1)k(2)k(3)k(4)k(5)k(6)k(7)k(8)k(9)\
        k(A)k(B)k(C)k(D)k(E)k(F)k(G)k(H)k(I)k(J)\
        k(K)k(L)k(M)k(N)k(O)k(P)k(Q)k(R)k(S)k(T)\
        k(U)k(V)k(W)k(X)k(Y)k(Z)
    #define K(x) if(window_pressed(app, 0[#x])) ZXKey(ZX_##x);
    KEYS(K);
    if(window_pressed(app, TK_SPACE))      {ZXKey(ZX_SPACE); /*if(mic_on) mic_on = 0, tap_prev();*/ }
    if(window_pressed(app, TK_BACKSPACE))  {ZXKey(ZX_SHIFT); ZXKey(ZX_0);}
    if(window_pressed(app, TK_RETURN))      ZXKey(ZX_ENTER);
    if(window_pressed(app, TK_SHIFT))       ZXKey(ZX_SHIFT);
    if(window_pressed(app, TK_CONTROL))     ZXKey(ZX_SYMB);
    if(window_pressed(app, TK_ALT))         ZXKey(ZX_CTRL);

    // prepare command keys
    if( window_trigger(app, TK_ESCAPE) ) cmdkey = 'ESC';
    if( window_pressed(app, TK_F1) )     cmdkey = 'F1';
    if( window_trigger(app, TK_F2) )     cmdkey = 'F2';
    if( window_trigger(app, TK_F3) )     cmdkey = 'prev';
    if( window_trigger(app, TK_F4) )     cmdkey = 'next';
    if( window_trigger(app, TK_F5) )     cmdkey = 'F5';
    if( window_trigger(app, TK_F6) )     cmdkey = 'RUN';
    if( window_trigger(app, TK_F7) )     cmdkey = 'ISSU';
    if( window_trigger(app, TK_F8) )     cmdkey = 'F8';
    if( window_trigger(app, TK_F9) )     cmdkey = window_pressed(app, TK_SHIFT) ? 'AY' : 'F9';
    if( window_trigger(app, TK_F11) )    cmdkey = 'F11';
    if( window_trigger(app, TK_F12) )    cmdkey = 'F12';

    static int prev = 0;
    int key = !!(GetAsyncKeyState(VK_SNAPSHOT) & 0x8000);
    if( key ^ prev ) cmdkey = 'PIC1';
    prev = key;
}


void frame(int drawmode, int do_sim) { // no render (<0), whole frame (0), scanlines (1)

// notify new frame
if(do_sim) sim_frame();

// NO RENDER
if( drawmode < 0 ) {
    if(do_sim) sim(ZX_TS);
    return;
}

// FRAME RENDER
if( drawmode == 0 ) {
    if(do_sim) sim(ZX_TS);
    for( int y = 0; y < 24+192+24; ++y ) draw(app, y);
    return;
}

// most models start in late_timings, and convert into early_timings as they heat
bool early_timings = ZX != 200; // however, +2 is always late_timings

// SCANLINE RENDER
if( ZX_PENTAGON ) {
    // Pentagon: see https://worldofspectrum.net/rusfaq/index.html
    // 320 scanlines = 16 vsync + 64  upper + 192 paper + 48 bottom
    // each scanline = 32 hsync + 36 border + 128 paper + 28 border = 224 TS/scanline
    // total = (16+64+192+48) * 224 = 320 * (32+36+128+28) = 320 * 224 = 71680 TS
    const int TS = 224;
    for( int y = 0; y <  16; ++y ) { if(do_sim) zx_int = !y, sim(TS); }
    for( int y = 0; y <  64; ++y ) { if(do_sim) sim(TS); if(y>(64-_24-1)) draw(app, y-(64-_24)); }
    for( int y = 0; y < 192; ++y ) { if(do_sim) sim(TS);                  draw(app, _24+y); }
    for( int y = 0; y <  48; ++y ) { if(do_sim) sim(TS); if(y<_24)        draw(app, _24+192+y); }
}
else if( ZX < 128 ) {
    // 48K: see https://wiki.speccy.org/cursos/ensamblador/interrupciones http://www.zxdesign.info/interrupts.shtml
    // 312 scanlines = 16 vsync + 48  upper + 192 paper + 56 bottom
    // each scanline = 48 hsync + 24 border + 128 paper + 24 border = 224 TS/scanline
    // total = (16+48+192+56) * 224 = 312 * (48+24+128+24) = 312 * 224 = 69888 TS
    const int TS = 224;
    if( do_sim ) zx_int = 0, sim(24-early_timings), zx_int = 1, sim(TS-24-early_timings);
    for( int y = 1; y <  64; ++y ) { if(do_sim) sim(TS); if(y>(64-_24-1)) draw(app, y-(64-_24)); }
    for( int y = 0; y < 192; ++y ) { if(do_sim) sim(TS);                  draw(app, _24+y); }
    for( int y = 0; y <  56; ++y ) { if(do_sim) sim(TS); if(y<_24)        draw(app, _24+192+y); }
} else {
    // 128K:https://wiki.speccy.org/cursos/ensamblador/interrupciones https://zx-pk.ru/threads/7720-higgins-spectrum-emulator/page4.html
    // 311 scanlines = 15 vsync + 48  upper + 192 paper + 56 bottom
    // each scanline = 48 hsync + 26 border + 128 paper + 26 border = 228 TS/scanline
    // total = (63+192+56) * 228 = 311 * (48+26+128+26) = 311 * 228 = 70908 TS
    const int TS = 228;
    if( do_sim ) zx_int = 0, sim(26-early_timings), zx_int = 1, sim(TS-26-early_timings);
    for( int y = 2; y <  64; ++y ) { if(do_sim) sim(TS); if(y>(64-_24-1)) draw(app, y-(64-_24)); }
    for( int y = 0; y < 192; ++y ) { if(do_sim) sim(TS);                  draw(app, _24+y); }
    for( int y = 0; y <  56; ++y ) { if(do_sim) sim(TS); if(y<_24)        draw(app, _24+192+y); }
}

#if 0
// @todo
// PIXEL RENDER
if( drawmode == 2 ) {
    TS = 0;

    // 128K
    // There are 63 scanlines before the television picture, as opposed to 64.
    // To modify the border at the position of the first byte of the screen (see the 48K ZX Spectrum section for details), the OUT must finish after 14365, 14366, 14367 or 14368 T states have passed since interrupt. As with the 48K machine, on some machines all timings (including contended memory timings) are one T state later.
    // Note that this means that there are 70908 T states per frame, and the '50 Hz' interrupt occurs at 50.01 Hz, as compared with 50.08 Hz on the 48K machine. The ULA bug which causes snow when I is set to point to contended memory still occurs, and also appears to crash the machine shortly after I is set to point to contended memory.
    // 228 TS/scanline, 311 scanlines

    // A single ZX Spectrum display row takes 224 T-States, including the horizontal flyback. For every T-State 2 pixels are written to the display, so 128 T-States pass for the 256 pixels in a display row. The ZX Spectrum is clocked at 3.5 MHz, so if 2 pixels are written in a single CPU clock cycle, the pixel clock of our display must be 7 MHz. A single line thus takes 448 pixel clock cycles.
    // The left and right border areas can be shown to be 48 pixels wide, which gives a visible row of 352 pixels (and the border equivalent) in total. That would take 352 / 2 = 176 T-States to display, and we know that a display row takes 224 T-States, so the lost 96 T-States must be used during the horizontal flyback of the electron beam to the start of the new row.

    static int vs = 0;
    if(window_trigger(app,'T')) { vs+=1; printf("%d\n", vs); }
    if(window_trigger(app,'G')) { vs-=1; printf("%d\n", vs); }

    for( int tv = 0; tv < 311; ++tv ) {
        int y = (tv - 24);
        zx_int = (tv == vs);
        if(y < 0 || y >= 240) sim(228);
        else {
            // 16px left extra
            sim(2*4);
            // 32px left
            sim(4*4);
            draw(app, y, 0, 4);
            // 256px main
            int excess = 0;
            for( int x = 0; x < 32; ++x ) {
                sim(4 - excess);
                excess = 0;
                excess += !!vram_accesses * contended[TS/4];
                vram_accesses = 0;
                draw(app, y, 32+x*8, 1);
            }
            // 32px right
            sim(4*4);
            draw(app, y, 32+256, 4);
            // 16px right extra
            sim(2*4);
            // retrace
            sim(52);
        }
    }
}
#endif

#if 1
    // detect ZX_RF flip-flop
    static int ZX_RF_old = 1;
    int refresh = ZX_RF ^ ZX_RF_old;
    ZX_RF_old = ZX_RF;

    if(ZX_RF) {
        static window *blend = 0;
        if(!blend) blend = window_bitmap(_320, _240);

        // reset bitmap contents when re-enabling ZX_RF
        if( refresh ) tigrBlit(blend, app, 0,0, 0,0, _320,_240);

        // ghosting
        tigrBlitAlpha(blend, app, 0,0, 0,0, _320,_240, 0.5f); //0.15f);
        tigrBlit(app, blend, 0,0, 0,0, _320,_240);

        // scanlines
        scanlines(app);

        // aberrations
        flick_hz ^= 1; // &1; // (rand()<(RAND_MAX/255));
        flick_frame = cpu.r; // &1; // (rand()<(RAND_MAX/255));
        blur(app);
    }
#endif

}


char **games;
int *dbgames;
int numgames;
int numok,numwarn,numerr; // stats
void rescan() {
    // clean up
    while( numgames ) free(games[--numgames]);
    free(games);

    // refresh stats
    {
        numok=0,numwarn=0,numerr=0;

        const char *folder = "./games/";
#if TESTS
        folder = "./src/tests/";
#endif
        for( dir *d = dir_open(folder, "r"); d; dir_close(d), d = NULL ) {
            for( unsigned count = 0, end = dir_count(d); count < end; ++count ) {
                if( !dir_file(d, count) ) continue;

                const char *fname = dir_name(d, count);
                if( strendi(fname, ".db") ) {
                    for(FILE *fp2 = fopen(fname, "rb"); fp2; fclose(fp2), fp2=0) {
                        int ch;
                        fscanf(fp2, "%d", &ch); ch &= 0xFF;
                        numok += ch == 1;
                        numerr += ch == 2;
                        numwarn += ch == 3;
                    }
                }
            }
            for( unsigned count = 0, end = dir_count(d); count < end; ++count ) {
                if( !dir_file(d, count) ) continue;

                const char *fname = dir_name(d, count);
                if( file_is_supported(fname,ALL_FILES) ) {
                    // append
                    ++numgames;
                    games = realloc(games, numgames * sizeof(char*) );
                    games[numgames-1] = strdup(fname);
                    //
                    dbgames = realloc(dbgames, numgames * sizeof(char*) );
                    dbgames[numgames-1] = db_get(fname);
                }
            }
        }
    }

    printf("%d games\n", numgames);
}

int active, selected, scroll;
int game_browser() { // returns true if loaded

    // scan files
    do_once
    {
        uint64_t then = time_ns();
        rescan();
        printf("%5.2fs rescan\n", (time_ns() - then)/1e9);
    }

    if( !numgames ) return 0;

    if( !active ) return 0;

    // restore mouse interaction in case it is being clipped (see: kempston mouse)
    mouse_clip(0);
    mouse_cursor(1);

//  tigrBlitTint(app, app, 0,0, 0,0, _320,_240, tigrRGB(128,128,128));

    enum { ENTRIES = (_240/11)-4 };
    static char *buffer = 0; if(!buffer) { buffer = malloc(65536); /*rescan();*/ }
    if (!numgames) return 0;
    if( scroll < 0 ) scroll = 0;
    for( int i = scroll; i < numgames && i < scroll+ENTRIES; ++i ) {
        const char starred = dbgames[i] >> 8 ? (char)(dbgames[i] >> 8) : ' ';
        sprintf(buffer, "%c %3d.%s%s\n", starred, i+1, i == selected ? " > ":" ", 1+strrchr(games[i], DIR_SEP) );
        window_printxycol(ui, buffer, 1, 3+(i-scroll-1),
            (dbgames[i] & 0x7F) == 0 ? tigrRGB(255,255,255) : // untested
            (dbgames[i] & 0x7F) == 1 ? tigrRGB(64,255,64) :   // ok
            (dbgames[i] & 0x7F) == 2 ? tigrRGB(255,64,64) : tigrRGB(255,192,64) ); // bug:warn
    }

    int up = 0, pg = 0;

    static int UPcnt = 0; UPcnt *= !!window_pressed(app, TK_UP);
    if( window_pressed(app, TK_UP) && (UPcnt++ == 0 || UPcnt > 32) ) {
        up = -1;
    } UPcnt *= !!window_pressed(app, TK_UP);
    static int DNcnt = 0; DNcnt *= !!window_pressed(app, TK_DOWN);
    if( window_pressed(app, TK_DOWN) && (DNcnt++ == 0 || DNcnt > 32) ) {
        up = +1;
    } DNcnt *= !!window_pressed(app, TK_DOWN);
    static int PGUPcnt = 0; PGUPcnt *= !!window_pressed(app, TK_PAGEUP);
    if( window_pressed(app, TK_PAGEUP) && (PGUPcnt++ == 0 || PGUPcnt > 32) ) {
        pg = -1;
    } PGUPcnt *= !!window_pressed(app, TK_PAGEUP);
    static int PGDNcnt = 0; PGDNcnt *= !!window_pressed(app, TK_PAGEDN);
    if( window_pressed(app, TK_PAGEDN) && (PGDNcnt++ == 0 || PGDNcnt > 32) ) {
        pg = +1;
    } PGDNcnt *= !!window_pressed(app, TK_PAGEDN);

    // issue browser
    if( window_trigger(app, TK_LEFT)  ) { for(--up; (selected+up) >= 0 && (dbgames[selected+up]&0xFF) <= 1; --up ) ; }
    if( window_trigger(app, TK_RIGHT) ) { for(++up; (selected+up) < numgames && (dbgames[selected+up]&0xFF) <= 1; ++up ) ; }

    for(;up < 0;++up) {
        --selected;
        if( selected < scroll ) --scroll;
    }
    for(;up > 0;--up) {
        ++selected;
        if( selected >= (scroll+ENTRIES) ) ++scroll;
    }
    for(;pg < 0;++pg) {
        if( selected != scroll ) selected = scroll;
        else scroll -= ENTRIES, selected -= ENTRIES;
    }
    for(;pg > 0;--pg) {
        if( selected != scroll+ENTRIES-1 ) selected = scroll+ENTRIES-1;
        else scroll += ENTRIES, selected += ENTRIES;
    }

    scroll = scroll < 0 ? 0 : scroll >= numgames - ENTRIES ? numgames-ENTRIES-1 : scroll;
    selected = selected < scroll ? scroll : selected >= (scroll + ENTRIES + 1) ? scroll + ENTRIES : selected;
    selected = selected < 0 ? 0 : selected >= numgames ? numgames-1 : selected;


        static int chars[16] = {0}, chars_count = -1;
        #define RESET_INPUTBOX() do { memset(chars, 0, sizeof(int)*16); chars_count = -1; } while(0)
        int any = 0;
        // Grab any chars and add them to our buffer.
        for(;;) {
            int c = tigrReadChar(app);
            if (c == 0) break;
            if( window_pressed(app,TK_CONTROL)) break;
            if( c == 8 ) { RESET_INPUTBOX(); break; } // memset(chars, 0, sizeof(int)*16); chars_count = -1; break; }
            if( c == '\t' && chars_count > 0 ) { any = 1; break; }
            if( c <= 32 ) continue;
            else any = 1;
            chars[ chars_count = min(chars_count+1, 15) ] = c;
        }
        // Print out the character buffer too.
        char tmp[1+16*6], *p = tmp;
        for (int n=0;n<16;n++)
            p = tigrEncodeUTF8(p, chars[n]);
        *p = 0;
        char tmp2[16+16*6] = "Find:"; strcat(tmp2, tmp);
        window_printxycol(ui, tmp2, 3,1, tigrRGB(0,192,255));
        if( any ) {
            static char lowercase[1024];
            for(int i = 0; tmp[i]; ++i) tmp[i] |= 32;
            int found = 0;
            if(!found)
            for( int i = scroll+1; i < numgames; ++i ) {
                if (i < 0) continue;
                for(int j = 0; games[i][j]; ++j) lowercase[j+1] = 0, lowercase[j] = games[i][j] | 32;
                if( strstr(lowercase, tmp) ) {
                    scroll = selected = i;
                    found = 1;
                    break;
                }
            }
            if(!found)
            for( int i = 0; i < scroll; ++i ) {
                for(int j = 0; games[i][j]; ++j) lowercase[j+1] = 0, lowercase[j] = games[i][j] | 32;
                if( strstr(lowercase, tmp) ) {
                    scroll = selected = i;
                    found = 1;
                    break;
                }
            }
        }

    if( window_pressed(app, TK_CONTROL) || window_trigger(app, TK_SPACE) ) {
        int update = 0;
        int starred = dbgames[selected] >> 8;
        int color = dbgames[selected] & 0xFF;
        if( window_trigger(app, TK_SPACE) ) color = (color+1) % 4, update = 1;
        if( window_trigger(app, 'D') ) starred = starred != 'D' ? 'D' : 0, update = 1; // disk error
        if( window_trigger(app, 'T') ) starred = starred != 'T' ? 'T' : 0, update = 1; // tape error
        if( window_trigger(app, 'I') ) starred = starred != 'I' ? 'I' : 0, update = 1; // i/o ports error
        if( window_trigger(app, 'R') ) starred = starred != 'R' ? 'R' : 0, update = 1; // rom/bios error
        if( window_trigger(app, 'E') ) starred = starred != 'E' ? 'E' : 0, update = 1; // emulation error
        if( window_trigger(app, 'Z') ) starred = starred != 'Z' ? 'Z' : 0, update = 1; // zip error
        if( window_trigger(app, 'S') ) starred = starred != 'S' ? 'S' : 0, update = 1; // star
        if( window_trigger(app, '3') ) starred = starred != '3' ? '3' : 0, update = 1; // +3 only error
        if( window_trigger(app, '4') ) starred = starred != '4' ? '4' : 0, update = 1; // 48K only error
        if( window_trigger(app, '1') ) starred = starred != '1' ? '1' : 0, update = 1; // 128K only error
        if( window_trigger(app, '0') ) starred = starred != '0' ? '0' : 0, update = 1; // USR0 only error
        if( window_trigger(app, 'A') ) starred = starred != 'A' ? 'A' : 0, update = 1; // ay/audio error
        if( window_trigger(app, 'V') ) starred = starred != 'V' ? 'V' : 0, update = 1; // video/vram error
        if( window_trigger(app, 'H') ) starred = starred != 'H' ? 'H' : 0, update = 1; // hardware error
        if( window_trigger(app, 'M') ) starred = starred != 'M' ? 'M' : 0, update = 1; // mem/multiload error
        if(update) {
            dbgames[selected] = color + (starred << 8);
            db_set(games[selected], dbgames[selected]);
        }
    }

    if( window_trigger(app, TK_RETURN) ) {
        active = 0;
        RESET_INPUTBOX();

        bool insert_next_disk_or_tape = false;
        if( last_load ) {
            if( 0 != strcmp(games[selected], last_load) ) {
                const char *a1 = games[selected], *a2 = last_load;

                // basenames and their lengths
                const char *b1 = strrchr(a1, '/') ? strrchr(a1, '/')+1 : a1; int l1 = strlen(b1);
                const char *b2 = strrchr(a2, '/') ? strrchr(a2, '/')+1 : a2; int l2 = strlen(b2);
                // printf("%s(%d) %s(%d)\n", b1,l1, b2,l2);

                // multi-load tapes and disks are well named (eg, Mutants - Side 1.tzx). 
                // following oneliner hack prevents some small filenames to be catched in the 
                // diff trap below. eg, 1942.tzx / 1943.tzx; they do not belong to each other
                // albeit their diff is exactly `1`.
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

        int model = strstri(games[selected], ".dsk") ? 300 : window_pressed(app, TK_SHIFT) ? 48 : 128;
        int must_clear = insert_next_disk_or_tape || strstr(games[selected], ".pok") || strstr(games[selected], ".POK") ? 0 : 1;
        int must_turbo = window_pressed(app,TK_CONTROL) || ZX_TURBOROM ? 1 : 0;
        int use_preloader = must_clear ? 1 : 0;

        if( must_clear ) boot(model, 0);
        if( must_turbo ) rom_patch_turbo();

        if( loadfile(games[selected],use_preloader) ) {
            void titlebar(const char *);
            titlebar(games[selected]);

            // clear window keys so the current key presses are not being sent to the 
            // next emulation frame. @fixme: use ZXKeyUpdate(); instead
            memset(tigrInternal(app)->keys, 0, sizeof(tigrInternal(app)->keys));
            memset(tigrInternal(app)->prev, 0, sizeof(tigrInternal(app)->prev));
        }

        return 1;
    }

    return 0;
}



void help() {
    int total = numok+numwarn+numerr;
    char *help = va(
        "Spectral " SPECTRAL " (Public Domain).\n"
        "https://github.com/r-lyeh/Spectral\n\n"
        "Library: %d games found (%d%%)\n\n"
        README "\n", numgames, 100 - (numerr * 100 / (total + !total)));
    (warning)("Spectral " SPECTRAL, help);
}

void titlebar(const char *filename) {
    const char *models[] = { [1]="16",[3]="48",[8]="128",[12]="+2",[13]="+2A",[18]="+3" };
    const char *title = filename ? 1+strrchr(filename, DIR_SEP) : "";
    window_title(app, va("Spectral%s %s%s%s", DEV ? " DEV" : "", models[ZX/16], title[0] ? " - " : "", title));
}


void draw_ui() {

    // ui
    {
        // compatibility stats
        int total = numok+numwarn+numerr;
        if(total && active) {
        TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + _239 * _320];
        int num1 = (numok * (float)_319) / total;
        int num2 = (numwarn * (float)_319) / total;
        int num3 = (numerr * (float)_319) / total; if((num1+num2+num3)<_319) num1 += _319 - (num1+num2+num3);
        for( int x = 0; x <= num1; ++x ) bar[x-320]=bar[x] = tigrRGB(64,255,64);
        for( int x = 0; x <= num2; ++x ) bar[x+num1-320]=bar[x+num1] = tigrRGB(255,192,64);
        for( int x = 0; x <= num3; ++x ) bar[x+num1+num2-320]=bar[x+num1+num2] = tigrRGB(255,64,64);
        static char compat[64];
        snprintf(compat, 64, "  OK:%04.1f%%     ENTER:128, +SHIFT:48, +CTRL:Try turbo", (total-numerr) * 100.f / (total+!total));
        window_printxy(ui, compat, 0,(_240-12.0)/11);
        }

        // ui
        int UI_LINE1 = (ZX_CRT ? 2 : 0); // first visible line

        struct mouse m = mouse();
        if( m.cursor == 0 ) {
            m.x = _320/2, m.y = _240/2; // ignore mouse; already clipped & hidden (in-game)
        } else {
            mouse_cursor(1);
        }

        // ui animation
        int hovering_border = !active && (m.x > _320 * 5/6 || m.x < _320 * 1/6);
        static float smooth; do_once smooth = hovering_border;
        smooth = smooth * 0.75 + hovering_border * 0.25;
        // left panel: game options
        if( 0 ) // smooth > 0.1 )
        {
            {
                // draw black panel
                TPixel transp = { 0,0,0, 192 * smooth };
                tigrFillRect(ui, 0, -1, smooth * (_320*1/6), _240+2, transp);
            }

            // left panel

            float chr_x = REMAP(smooth,0,1,-6,0.5) * 11, chr_y = REMAP(smooth,0,1,-3,2.5) * 11;

            ui_at(ui,chr_x,chr_y);

            // stars, user-score
            const char *stars[] = {
                "\2\x10\f\x10\f\x10\n", // 0 0 0
                "\2\x11\f\x10\f\x10\n", // 0 0 1
                "\2\x12\f\x10\f\x10\n", // 0 1 0
                "\2\x12\f\x11\f\x10\n", // 0 1 1
                "\2\x12\f\x12\f\x10\n", // 1 0 0
                "\2\x12\f\x12\f\x11\n", // 1 0 1
                "\2\x12\f\x12\f\x12\n", // 1 1 0
                "\2\x12\f\x12\f\x12\n", // 1 1 1
            };
            static int score = 3;
            if( ui_click("-Stars-", stars[score]) )
                score = (score + 1) % 7;

            // issues 
            static int issues[4] = {0};
            static const char *err_warn_ok = "\7\4\2\6";
            if( ui_click("-Toggle Loading issue-", va("%c%s\f", err_warn_ok[issues[0]], issues[0] ? "":"" )) ) {
                issues[0] = (issues[0] + 1) % 4;
            }
            if( ui_click("-Toggle Game issue-", va("%c%s\f", err_warn_ok[issues[1]], issues[1] ? "":"" )) ) {
                issues[1] = (issues[1] + 1) % 4;
            }
            if( ui_click("-Toggle Audio issue-", va("%c%s\f", err_warn_ok[issues[2]], issues[2] ? "":"" )) ) {
                issues[2] = (issues[2] + 1) % 4;
            }
            if( ui_click("-Toggle Other issue-", va("%c%s\f", err_warn_ok[issues[3]], issues[3] ? "":"" )) ) {
                issues[3] = (issues[3] + 1) % 4;
            }
            int count = 0; for(int i = 0; i < 4; ++i) count += issues[i] > 1;
            if( ui_click("-Total Issues-", va("%d\n", count)) ) {
            }

            // sample sketch
            if( ui_click("- 2018 -", "Year\n"));
            if( ui_click("- RetroSouls (Russia) -", "Brand\n"));

            if( ui_click("- Arcade Game: Maze -", "Genre\n"));
            if( ui_click("- AY Sound -", "Feat.\n"));
            if( ui_click("- Multicolour (Rainbow Graphics) -", "Feat.\n"));

            if( ui_click("- Denis Grachev (Russia) -", "Author\n"));
            if( ui_click("- Oleg Nikitin (Russia) -", "Author\n"));
            if( ui_click("- Ivan Seleznev -", "Author\n"));
 
            // zxdb
            if( ui_click("- 34458 -", "ZXDB\n"));

            // pokes
            if( ui_click("- Cheats -", "Cheats\n"));

            // manual
            // inlays
            // tape scans

            // mp3s
            // mags reviews

            // netplay lobby
            // #tags
        }
        // right panel: emulator options
        if( 1 )
        {
            int chr_x = REMAP(smooth,0,1,33,28) * 11 + 0, chr_y = REMAP(smooth,0,1,-4,2) * 11;

            {
                // draw black panel
                TPixel transp = { 0,0,0, 192 * smooth };
                tigrFillRect(ui, REMAP(smooth,0,1,_320,_320*5/6), -1, _320*1/2, _240+2, transp);
            }

            ui_at(ui,chr_x - 8,chr_y-11);
            if( ui_click("-Take Picture-", "%c", SNAP_CHR) ) cmdkey = 'PIC1'; // send screenshot command

            ui_at(ui,chr_x,chr_y-11);
            if( ui_press("-Full Throttle-", "%c\b\b\b%c\b\b\b%c%d\n\n", PLAY_CHR,PLAY_CHR,PLAY_CHR,(int)fps) ) cmdkey = 'F1';

            const char *models[] = { [1]="16",[3]="48",[8]="128",[12]="+2",[13]="+2A",[18]="+3" };
            if( ui_click("-Reload Model-", "\f%s%s",models[ZX/16],ZX_ALTROMS ? "!":"") ) cmdkey = 'SWAP';
            //if( ui_click("-NMI-", "") ) cmdkey = 'NMI';
            if( ui_click("-Reset-", "\n") ) cmdkey = 'BOMB';

            if( ui_click("-Toggle TV mode-", "▒\f%d\n", 1+(ZX_CRT << 1 | ZX_RF)) ) cmdkey = 'F9';
            if( ui_click("-Toggle AY core-", "♬\f%d\n",ZX_AY) ) cmdkey = 'AY';
            if( ui_click("-Toggle ULAplus-", "%c\f%d\n", ZX_ULAPLUS ? 'U':'u'/*CHIP_CHR '+'*/, ZX_ULAPLUS) ) cmdkey = 'PLUS';

            if( ui_click("-Toggle Joysticks-", "%c\f%d\n", JOYSTICK_CHR, ZX_JOYSTICK)) cmdkey = 'JOY';
            if( ui_click("-Toggle Mouse-", "\x9\f%d\n", ZX_MOUSE) ) cmdkey = 'MICE';
            if( ui_click("-Toggle Lightgun-", "\xB\f%d\n", ZX_GUNSTICK) ) cmdkey = 'GUNS';

            if( ui_click("-Toggle RunAHead-", !ZX_RUNAHEAD ? "🯆\f0\n" : "🯇\f1\n") ) cmdkey = 'RUN';
            if( ui_click("-Toggle TurboROM-", !ZX_TURBOROM ? "\f0\n" : "\f1\n")) cmdkey = 'TROM';
            if( ui_click("-Toggle AutoTape-", "%c\f%d\n", PLAY_CHR,ZX_AUTOPLAY) ) cmdkey = 'AUTO';

            if( ui_click("-Translate game menus-", "T\f%d\n", ZX_AUTOLOCALE)) cmdkey = 'ALOC';

            // if( ui_click("-Toggle Speed-", "\f%d\n", (int)(ZX_FPS*50.)) ) cmdkey = 'FPS';
            if( ui_click("-Toggle 50/60 Hz-", "\f%d\n", ZX_FPS > 1) ) cmdkey = 'FPS';

            if( ui_click("-Toggle letter mode-", "~%c~\f%d\n", ZX_KLMODE ? 'L' : 'K', ZX_KLMODE) ) cmdkey = 'KL';

            //if( ui_click("-Toggle TapePolarity-", "%c\f%d\n", mic_low ? '+':'-', !mic_low) ) cmdkey = 'POLR';
            // if( ZX < 128 )
            if( ui_click("-Toggle Keyboard Issue 2/3-", "i\f%d\n", issue2 ? 2 : 3)) cmdkey = 'ISSU';

            if( ZX == 128 )
            if( ui_click("-Toggle Pentagon-", "%c\f%d\n", ZX_PENTAGON ? 'P':'p',/*beta128+*/ZX_PENTAGON)) {
                ZX_PENTAGON ^= 1;
                rom_restore();
                if(PC(cpu) < 0x4000 && !tape_playing()) reset(ZX);
            }

            int right = chr_x+8*4-4;
            int bottom = chr_y+8*31.0-1;

            ui_at(ui,chr_x - 8,bottom+1);
            if( ui_click(NULL, "") ) cmdkey = 'HELP';

            ui_at(ui,right,bottom);
            if( ui_click("-Debug-", "") ) cmdkey = 'DEV'; // send disassemble command
        }

        // manual tape handling
        ui_at(ui,1*11, 1*11);
        if( ZX_AUTOPLAY ) {
            if( ui_click(NULL, "%c", !active ? PLAY_CHR : PAUSE_CHR) ) active ^= 1;
        } else {
            if( ui_click(NULL, "%c", !tape_playing() ? PLAY_CHR : PAUSE_CHR) ) tape_play(!tape_playing());
            if( ui_click(NULL, "\xf\b\b\b\xf") ) cmdkey = 'prev';
            if( ui_click(NULL, "%c\b\b\b%c", PLAY_CHR, PLAY_CHR) ) cmdkey = 'next';
            if( ui_click(NULL, "■") ) cmdkey = 'stop';
            ui_x += 2;
            if( ui_click(NULL, "\xe") ) active ^= 1;
            ui_x += 1;
        }

        // tape progress
        float pct = tape_tellf();
#if DEV
        if( mic_on )
#else
        if( ZX_AUTOPLAY ? tape_inserted() : 1 )
#endif
        if( pct <= 1.00f )
        {
            TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + UI_LINE1 * _320];

            // bars & progress
            unsigned mark = pct * _320;
            for( int x = 0; x < _320; ++x ) bar[x] = bar[x+2*_320] = white;
            for( int x = 0; x<=mark; ++x ) bar[x+_320] = white; bar[_320-1+_320] = white;
            for( int x = 0; x < _320; ++x ) if(tape_preview[x]) bar[x+1*_320] = white;
            // triangle marker (top)
            bar[mark+4*_320] = white;
            for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+5*_320] = white;
            for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+6*_320] = white;
            // mouse seeking
            if( m.y > 0 && m.y < 11 ) {
                mouse_cursor(2);
                if( m.buttons ) {
                    m.x = m.x < 0 ? 0 : m.x > _320 ? _320 : m.x;
                    tape_seekf(m.x / (float)_320);
                }
            }
        }

        // bottom slider. @todo: rewrite this into a RZX player/recorder
        if( ZX_DEBUG )
        if( !active ) {
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
}

int main() {
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

    {
        // relocate cwd to exe folder (relative paths wont work from this point)
        char path[MAX_PATH]={0};
        GetModuleFileName(0,path,MAX_PATH);
        *strrchr(path, '\\') = '\0';
        SetCurrentDirectoryA(path);
    }

    // app
    app = window_open(_32+256+_32, _24+192+_24, va("Spectral%s", DEV ? " DEV" : "") );
    ui = window_bitmap(_320, _240);
    dbg = window_bitmap(_320, _240);

    // postfx
    crt(ZX_CRT);

    // must be as close to frame() as possible
    audio_init();

    // zx
    boot(128, 0);

    // import state
    for( FILE *state = fopen("Spectral.sav","rb"); state; fclose(state), state = 0) {
        if( import_state(state) )
            pins = z80_prefetch(&cpu, cpu.pc),
            titlebar(0);
    }

    // main loop
    do {
        ui_frame();
        input();

        int tape_accelerated = tape_inserted() && tape_peek() == 'o' ? 0 
            : tape_playing() && (ZX_FASTTAPE || ZX_FASTCPU);
        if( tape_accelerated && active ) tape_accelerated = 0;

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

        // screenshots: after drawn frame, before UI
        if( cmdkey == 'PIC2' )
        {
            char buffer[128];
            GetWindowTextA((HWND)((app)->handle), buffer, 128);
            screenshot( buffer );
            play('cam', 1);
        }

        static char status[128] = "";
        char *ptr = status;
        ptr += sprintf(ptr, "%dm%02ds ", (unsigned)(timer) / 60, (unsigned)(timer) % 60);
        ptr += sprintf(ptr, "%5.2ffps%s %d mem%s%d%d%d%d ", fps, do_runahead ? "!":"", ZX, rom_patches ? "!":"", GET_MAPPED_ROMBANK(), (page128&8?7:5), 2, page128&7);
        ptr += sprintf(ptr, "%02X%c%02X %04X ", page128, page128&32?'!':' ', page2a, PC(cpu));
        ptr += sprintf(ptr, "%c%c %4dHz ", "  +-"[tape_inserted()*2+tape_level()], toupper(tape_peek()), tape_hz);

        tigrClear(ui, !active ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,128));

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
            
        // game browser
        int game_loaded = game_browser();

        // measure time & frame lock (50.01 fps)
        int max_speed = tape_accelerated || !ZX_FPS || ZX_FASTCPU; // max speed if tape_accelerated or no fps lock
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
#else // accurate (beware of CPU usage)
            float target = ZX_FPS * (ZX < 128 ? 50.08:50.01);

            // be nice to os
            sys_sleep(ZX_FPS > 1.2 ? 1 : 5);
            // complete with shortest sleeps (yields) until we hit target fps
            dt = tigrTime();
            for( float target_fps = 1.f/(target+!target); dt < target_fps; ) {
                sys_yield();
                dt += tigrTime();
            }
#endif
        }

        // calc fps
        static int frames = 0; ++frames;
        static double time_now = 0; time_now += dt;
        if( time_now >= 1 ) { fps = frames / time_now; time_now = frames = 0; }

        // tape timer
        if(tape_playing()) timer += dt;

        // stats & debug
        draw_ui();

        if( ZX_DEBUG )
        tigrBlitAlpha(app, dbg, 0,0, 0,0, _320,_240, 1.0f);

        // draw ui on top
        tigrBlitAlpha(app, ui, 0,0, 0,0, _320,_240, 1.0f);

        // flush
        window_update(app);


        #define LOAD(ZX,TURBO,file) if(file) do { \
                boot(ZX, 0); if(TURBO || window_pressed(app,TK_CONTROL)) rom_patch_turbo(); \
                if( !loadfile(file,1) ) { \
                    if( !load_shader( file ) ) { \
                        warning(va("cannot open '%s' file\n", file)); \
                    } \
                } \
            } while(0)

        // parse drag 'n drops. reload if needed
        for( char **list = tigrDropFiles(app); list; list = 0)
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
        int cmd = cmdkey;
        cmdkey = 0;

        // parse commands
        ZX_FASTCPU = 0;
        switch(cmd) { default:
            break; case 'ESC':   active ^= 1;
            break; case  'F1':   ZX_FASTCPU = 1; // fast-forward cpu
            break; case  'F2':   if(!tape_inserted()) active ^= 1; else tape_play(!tape_playing()); // open browser if start_tape is requested but no tape has been ever inserted
            break; case 'prev':  tape_prev();
            break; case 'next':  tape_next();
            break; case 'stop':  tape_stop();
            break; case  'F8':   ZX_FASTTAPE ^= 1;
            // cycle tv modes
            break; case  'F9':   { static int mode = 0; do_once mode = ZX_CRT << 1 | ZX_RF; mode = (mode + 1) & 3; ZX_RF = mode & 1; crt( ZX_CRT = !!(mode & 2) ); }
            break; case 'F11':   quicksave(0);
            break; case 'F12':   quickload(0);

            // cycle AY cores
            break; case  'AY':   { const int table[] = { 1,2,0,0 }; ZX_AY = table[ZX_AY]; }

            break; case 'PIC1':   cmdkey = 'PIC2'; // resend screenshot cmd

            break; case 'TROM':  ZX_TURBOROM ^= 1; boot(ZX, 0); loadfile(last_load,1); // toggle turborom and reload
            break; case 'F5':    reset(0); loadfile(last_load, 1);
            break; case 'NMI':   if( pins & Z80_NMI ) pins &= ~Z80_NMI; else pins |= Z80_NMI; // @todo: verify
            break; case 'BOMB':  reset(KEEP_MEDIA/*|QUICK_RESET*/); // if(last_load) free(last_load), last_load = 0;
            break; case 'SWAP':  // toggle model and reload
            {
                static int modes[] = { 128, 48, 200, 210, 300, 16 };
                static byte mode = 0;
                while(modes[mode] != ZX)
                mode = (mode + 1) % 6;
                mode = (mode + 1) % 6;
                ZX = modes[ mode ];
                boot(ZX, 0); loadfile(last_load,1); // toggle rom
                titlebar(last_load); // refresh titlebar
            }
            break; case 'JOY':   ZX_JOYSTICK--; ZX_JOYSTICK += (ZX_JOYSTICK<0)*4; if(ZX_JOYSTICK) ZX_GUNSTICK = 0; // cycle Cursor/Kempston/Fuller,Sinclair1,Sinclair2
            break; case 'GUNS':  ZX_GUNSTICK ^= 1;   if(ZX_GUNSTICK) ZX_MOUSE = 0, ZX_JOYSTICK = 0; // cycle guns
            break; case 'MICE':  ZX_MOUSE ^= 1;      if(ZX_MOUSE) ZX_GUNSTICK = 0;                  // cycle kempston mouse(s)
            break; case 'PLUS':  ZX_ULAPLUS ^= 1;    // cycle ulaplus
            break; case 'AUTO':  ZX_AUTOPLAY ^= 1;   // cycle autoplay command
            break; case 'RUN':   ZX_RUNAHEAD ^= 1;   // cycle runahead mode
            break; case 'DEV':   ZX_DEBUG ^= 1;
            break; case 'KL':    ZX_KLMODE ^= 1, ZX_KLMODE_PATCH_NEEDED = 1;

            // break; case 'POLR':  mic_low ^= 64;
            break; case 'ISSU':  issue2 ^=1; reset(KEEP_MEDIA); loadfile(last_load,1); // toggle rom

            //break; case 'FPS':   { const float table[] = { [0]=1,[10]=1.2,[12]=2,[20]=4,[40]=0 }; ZX_FPS = table[(int)(ZX_FPS*10)]; }
            break; case 'FPS':   { const float table[] = { [10]=1.2,[12]=1 }; ZX_FPS = table[(int)(ZX_FPS*10)]; }

            break; case 'ALOC':  ZX_AUTOLOCALE ^= 1; if(ZX_AUTOLOCALE) translate(mem, 0x4000*16, 'en');
            break; case 'HELP':  help();
        }

    } while( window_alive(app) );

    // export state
#if NEWCORE
    // do
    while( !z80_opdone(&cpu) ) pins = z80_tick(&cpu, pins);
    // while(IFF1(cpu));
#endif
    if(medias) media[0].pos = voc_pos / (double)(voc_len+!voc_len);
    for( FILE *state = fopen("Spectral.sav","wb"); state; fclose(state), state = 0) {
        if( !export_state(state) )
            warning("Error exporting state");
    }

    window_close(app);
    return 0;
}
