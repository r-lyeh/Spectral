// # build (windows)
// cl run.c /O2 /MT /DNDEBUG /GL /GF /arch:AVX2
//
// # build (linux)
// sudo apt-get install mesa-common-dev libx11-dev gcc libgl1-mesa-dev
// gcc run.c -Wno-unused-result -Wno-format -lm -ldl -lX11 -lpthread -lGL -O3
//
// # usage
// - esc: game browser
// - f1: speed boost (hold)
// - f2: start/stop tape
// - f3/f4: rewind/advance tape
// - f5: restart
// - f6: toggle input latency (runahead)
// - f7: toggle tape polarity
// - f8: toggle fast tape
// - f9: tv rf mode
// - f11/f12: quick save/load 
// - alt+enter: fullscreen
// - tab+cursors: joysticks
//
// ref: http://www.zxdesign.info/vidparam.shtml
// https://worldofspectrum.org/faq/reference/48kreference.htm
// https://faqwiki.zxnet.co.uk/wiki/ULAplus
// https://foro.speccy.org/viewtopic.php?t=2319
// test(48): RANDOMIZE USR 46578
// test(48): RANDOMIZE USR 5050
// test(2A): RANDOMIZE USR 20000

//
// done
// cpu, ula, mem, rom, 48/128, key, joy, ula+, tap, ay, beep, sna/128, fps, tzx, if2, zip, rf, menu, kms, z80, scr,
// key2/3, +2a/+3, fdc, dsk,

// fix
// tzx(flow,gdb)
// z80 uncr, kms windowed, window focus @ MMB+tiger
// crt blur/eyefish
// fdc not being completely reset sometimes (load mercs.dsk, then mercs.dsk again)

// @todo:
// disable mic_on if...
// ay write
// out beeper
// input keyboard != space

// fixme: edos tapes: streecreedfootball(edos).tzx,
//      beyondtheicepalace(edos).tzx,elvenwarrior(edos).tzx, uses additional non-standard padding bytes in headers (19->29), making the EAR routine to loop pretty badly
// [x] LOAD "" CODE if 1st block is code
// [x] glue consecutive tzx/taps in zips (side A) -> side 1 etc)
// [ ] glue consecutive tzx/taps in disk
// [ ] glue: do not glue two consecutive tapes if size && hash match (both sides are same)
// [ ] glue: if tape1 does not start with a basic block, swap tapes
// [ ] scan programs.bas for "128" string on them. reposition marker on demand. (barbarian 2; dragon ninja)
// [ ] stop tape if next block is PROGRAM (side 2)
// [ ] stop tape if pause && key/joy pressed
// TS   / if in(fe) slows down -> pause tape for real (until in(fe) boosts up)
// prev \ stop tape -> slow down --> test joeblade2,atlantis,vegasolaris,amc
// score128: 128 in filename + memcmp("19XY") X <= 8 Y < 5 + sizeof(tape) + memfind("in ayffe") + side b > 48k + program name "128" + filename128  -> load as 128, anything else: load as usr0
//      if single bank > 49k (navy seals), if size(tap)>128k multiload (outrun)
// auto rewind

// try
// https://github.com/anotherlin/z80emu

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

#include "3rd.h"
#include "emu.h"
#include "sys.h"
#include "zx.h"

// @fixme: RUNAHEAD: +3 are not compatible. AYUMI not compatible (see TargetRenegade128K).
// https://near.sh/articles/input/run-ahead https://www.youtube.com/watch?v=_qys9sdzJKI // https://docs.libretro.com/guides/runahead/

int RUNAHEAD = 1;

enum { _320 = 320, _319 = _320-1, _32 = (_320-256)/2 };
enum { _240 = 240, _239 = _240-1, _24 = (_240-192)/2 };

int file_is_supported(const char *filename) {
    // @todo: trd,scl
    const char *ext = strrchr(filename ? filename : "", '.');
    if( ext && strstr(".sna.z80.tap.tzx.rom.dsk.scr.zip", ext) ) return 1;
    if( ext && strstr(".SNA.Z80.TAP.TZX.ROM.DSK.SCR.ZIP", ext) ) return 1;
    return 0;
}

void regs() {
    extern byte page128;
    printf("af:%04x,bc:%04x,de:%04x,hl:%04x,ix:%04x\n", cpu.af,cpu.bc,cpu.de,cpu.hl,cpu.ix);
    printf("af'%04x,bc'%04x,de'%04x,hl'%04x,iy:%04x\n", cpu.af2,cpu.bc2,cpu.de2,cpu.hl2,cpu.iy);
    printf("pc:%04x,sp:%04x,ir:%02x%02x,im:%d,if2:%d,ei:??\n", cpu.pc,cpu.sp,cpu.i,cpu.r,cpu.im,cpu.iff2/*,cpu.ei_pending*/);
    printf("page128(%02x)\n", page128);
}



static int flick_frame = 0;
static int flick_hz = 0;

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

*texture = ZXPalette[ZXBorderColor];
int shift0 = ZX_TV ? (rand()<(RAND_MAX/256)) : 0; // flick_frame * -(!!((y+0)&0x18))
texture += shift0;

        for(int x = 0; x < 32; ++x) {
            byte attr = *attribs;
            byte pixel = *pixels, fg, bg;

            if (ulaplus_enabled) {
                fg = ((attr & 0xc0) >> 2) | ((attr & 0x07));
                bg = ((attr & 0xc0) >> 2) | ((attr & 0x38) >> 3) | 8;
            } else {
                pixel ^= (attr & 0x80) && ZXFlashFlag ? 0xff : 0x00;
                fg = (attr & 0x07) | ((attr & 0x40) >> 3);
                bg = (attr & 0x78) >> 3;
            }

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

if(ZX_TV) {
                    continue;
                    // saturate aberrations (very slow)
                    pix0 = texture[i];
                    rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    if( s0 * 1.5 > 255 ) s0 = 255; else s0 *= 1.5;
                    if( v0 * 1.01 > 255 ) v0 = 255; else v0 *= 1.01;
                    texture[i] = as_rgb(h0,s0,v0);
}
                }
            // interesting tv effects (j): 13, 19, 23, 27, 29, 33

                if(x%2)
                for(int i = 0; i < 8; ++i) {
                    unsigned pix0 = texture[i-0];
                    byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    texture[i] = as_rgb(h0,s0,v0*0.99);
                }

                for(int i = 0; i < 8; ++i) { ++j; j%=33;
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
    // hsv slow
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
            void *data = file_is_supported(zip_name(z,i)) ? zip_extract(z,i) : 0;
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



static char *last_load = 0;
int load(const char *file, int must_clear, int do_rompatch) {
    if( !file ) return 0;
    last_load = (free(last_load), strdup(file));

    if( must_clear ) {
        // hard reset
    //  pins = z80_reset(&cpu);
        // soft reset
        mic_reset();
rom_patch(do_rompatch);
        page128 = 0;
        port_0x7ffd(0);
    }

    char *ptr = 0; size_t size = 0;
    void *zip_read(const char *filename, size_t *size);
    if( strstr(file, ".zip") ) {
        ptr =  zip_read(file, &size);
        if(!(ptr && size))
            return 0;
    }

    if(!ptr)
    for( FILE *fp = fopen(file,"rb"); fp; fclose(fp), fp = 0) {
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        ptr = malloc(size);
        fread(ptr, 1, size, fp);
    }

    if(!(ptr && size))
        return 0;

#ifndef NDEBUG
    // forensics
    for( FILE *fp = fopen("last.tzx","wb"); fp; fclose(fp), fp=0) {
        fwrite(ptr, 1, size, fp);
    }
    for( FILE *fp = fopen("last.tap","wb"); fp; fclose(fp), fp=0) {
        fwrite(ptr, 1, size, fp);
    }
    for( FILE *fp = fopen("last.sna","wb"); fp; fclose(fp), fp=0) {
        fwrite(ptr, 1, size, fp);
    }
    for( FILE *fp = fopen("last.z80","wb"); fp; fclose(fp), fp=0) {
        fwrite(ptr, 1, size, fp);
    }
    for( FILE *fp = fopen("last.dsk","wb"); fp; fclose(fp), fp=0) {
        fwrite(ptr, 1, size, fp);
    }
#endif

    // dsk first
    if(!memcmp(ptr, "MV - CPC", 8)) return dsk_load(ptr), pins = z80_prefetch(&cpu, cpu.pc), 1;
    if(!memcmp(ptr, "EXTENDED", 8)) return dsk_load(ptr), pins = z80_prefetch(&cpu, cpu.pc), 1;

    // make usr0 (128) pretty O:)
    #define show_loader_screen(snap,len) \
        do { if( (snap) && (len) && sna_load((snap),(len))) { if(ZX>=128) memcpy(VRAM, ld128scr, 6912); ZXBorderColor = 7; page128 = ZX<128?32:16; } } while(0)

    // tapes first
    if(tzx_load(ptr, (int)size)) {
        int is_bin = mic_data[1] == 3, len = is_bin ? (ZX < 128 ? ld48bin_length : ld128bin_length) : (ZX < 128 ? ld48bas_length : ld128bas_length);
        const byte *bin = is_bin ? (ZX < 128 ? ld48bin : ld128bin) : (ZX < 128 ? ld48bas : ld128bas);
        if(must_clear) show_loader_screen(bin, len);
rom_patch(mic_queue_has_turbo ? 0 : do_rompatch);
pins = z80_prefetch(&cpu, cpu.pc);
        return 1;
    }
    if(tap_load(ptr,(int)size)) {
        int is_bin = mic_data[1] == 3, len = is_bin ? (ZX < 128 ? ld48bin_length : ld128bin_length) : (ZX < 128 ? ld48bas_length : ld128bas_length);
        const byte *bin = is_bin ? (ZX < 128 ? ld48bin : ld128bin) : (ZX < 128 ? ld48bas : ld128bas);
        if(must_clear) show_loader_screen(bin, len);
rom_patch(do_rompatch); // beware: wont work for wizball.tap (custom loader)
pins = z80_prefetch(&cpu, cpu.pc);
        return 1;
    }

    // headerless fixed-size formats now, sorted by ascending file size.
    if( scr_load(ptr, size) ) {
pins = z80_prefetch(&cpu, cpu.pc);
        return 1;
    }
    if( rom_load(ptr, size) ) {
pins = z80_prefetch(&cpu, cpu.pc);
        return 1;
    }
    if( sna_load(ptr, size) ) {
pins = z80_prefetch(&cpu, cpu.pc);
        return regs(), 1;
    }

    // headerless variable-size formats now
    if( z80_load(ptr, size) ) {
pins = z80_prefetch(&cpu, cpu.pc);
        return regs(), 1;
    }

    puts("unknown file format");
    return 0;
}



window *app, *ui;
int do_disasm = 0;
void input() {
    // keyboard
    ZXKeyUpdate();

    // kempston/i2l+cursor/protek/agf+fuller
    // @todo: interface ii port1(67890 <>v^F) port2(12345)
    kempston=0; fuller=0xff;
    if(window_pressed(app, TK_TAB))   { kempston|=16; fuller&=0xFF-128; ZXKey(ZX_0); /*ZXKey(ZX_5); ZXKey(ZX_0);*/ }
    if(window_pressed(app, TK_UP))    { kempston|=8;  fuller&=0xFF-1;   ZXKey(ZX_7); /*ZXKey(ZX_4); ZXKey(ZX_9);*/ }
    if(window_pressed(app, TK_DOWN))  { kempston|=4;  fuller&=0xFF-2;   ZXKey(ZX_6); /*ZXKey(ZX_3); ZXKey(ZX_8);*/ }
    if(window_pressed(app, TK_RIGHT)) { kempston|=1;  fuller&=0xFF-8;   ZXKey(ZX_8); /*ZXKey(ZX_2); ZXKey(ZX_7);*/ }
    if(window_pressed(app, TK_LEFT))  { kempston|=2;  fuller&=0xFF-4;   ZXKey(ZX_5); /*ZXKey(ZX_1); ZXKey(ZX_6);*/ }

    #if 0
        //OPQAM/SP,OP1QM/SP,KLAZM/SP,ZXPL0/SP,QABNM/SP,QZIPM/SP,1ZIPM/SP,670OM/SP
        byte mapped_joystick = 0;
        byte mapped_joysticks[][6] = {
        { SPECKEY_O,SPECKEY_P,SPECKEY_Q,SPECKEY_A,SPECKEY_M,SPECKEY_SPACE },
        { SPECKEY_O,SPECKEY_P,SPECKEY_1,SPECKEY_Q,SPECKEY_M,SPECKEY_SPACE },
        { SPECKEY_K,SPECKEY_L,SPECKEY_A,SPECKEY_Z,SPECKEY_M,SPECKEY_SPACE },
        { SPECKEY_Z,SPECKEY_X,SPECKEY_P,SPECKEY_L,SPECKEY_0,SPECKEY_SPACE },
        { SPECKEY_B,SPECKEY_N,SPECKEY_Q,SPECKEY_A,SPECKEY_M,SPECKEY_SPACE },
        { SPECKEY_I,SPECKEY_P,SPECKEY_Q,SPECKEY_Z,SPECKEY_M,SPECKEY_SPACE },
        { SPECKEY_I,SPECKEY_P,SPECKEY_1,SPECKEY_Z,SPECKEY_M,SPECKEY_SPACE },
        { SPECKEY_6,SPECKEY_7,SPECKEY_0,SPECKEY_O,SPECKEY_M,SPECKEY_SPACE },
        { (byte)-1, (byte)-1, (byte)-1, (byte)-1, (byte)-1, (byte)-1 }
        } ;
    #endif

    #define KEYS(X) \
        X(0)X(1)X(2)X(3)X(4)X(5)X(6)X(7)X(8)X(9)\
        X(A)X(B)X(C)X(D)X(E)X(F)X(G)X(H)X(I)X(J)\
        X(K)X(L)X(M)X(N)X(O)X(P)X(Q)X(R)X(S)X(T)\
        X(U)X(V)X(W)X(X)X(Y)X(Z)
    #define X(x) if(window_pressed(app, 0[#x])) ZXKey(ZX_##x);
    KEYS(X);
    if(window_pressed(app, TK_SPACE))      {ZXKey(ZX_SPACE); /*if(mic_on) mic_on = 0, tap_prev();*/ }
    if(window_pressed(app, TK_BACKSPACE))  {ZXKey(ZX_SHIFT); ZXKey(ZX_0);}
    if(window_pressed(app, TK_RETURN))      ZXKey(ZX_ENTER);
    if(window_pressed(app, TK_SHIFT))       ZXKey(ZX_SHIFT);
    if(window_pressed(app, TK_CONTROL))     ZXKey(ZX_SYMB);
    if(window_pressed(app, TK_ALT))         ZXKey(ZX_CTRL);

    if(window_pressed(app, TK_F1))        boost_on = 1; else boost_on = 0;
    if(window_trigger(app, TK_F2))        mic_on ^= 1;
    if(window_trigger(app, TK_F3))        tap_prev();
    if(window_trigger(app, TK_F4))        tap_next();
    if(window_trigger(app, TK_F5))        { clear(window_pressed(app,TK_SHIFT) ? 48 : 128); load(last_load, 1, window_pressed(app,TK_CONTROL) ? 0:1); }
    if(window_trigger(app, TK_F6))        RUNAHEAD ^= 1;
    if(window_trigger(app, TK_F7))        mic_invert_polarity ^= 1;
    if(window_trigger(app, TK_F8))        ZX_FAST ^= 1;
    if(window_trigger(app, TK_F9))        ZX_TV ^= 1;
    if(window_trigger(app, TK_F11))       checkpoint_save(0);
    if(window_trigger(app, TK_F12))       checkpoint_load(0);
}
void frame(int drawmode) { // no render (<0), full screen (0), scanlines (1)
// NO RENDER
if( drawmode < 0 ) {
    zx_vsync = 1;
    sim(ZX_TS);
    return;
}

// FRAME RENDER
if( drawmode == 0 ) {
    zx_vsync = 1;
    sim(ZX_TS);
        for( int y = 0; y < 24+192+24; ++y ) draw(app, y);
    return;
}

// SCANLINE RENDER
if( drawmode == 1 ) {
    if( ZX < 128 ) {
        // 48K
        // After an interrupt occurs, 64 line times (14336 T states; see below for exact timings) pass before the first byte of the screen (16384)
        // is displayed. At least the last 48 of these are actual border-lines; the others may be either border or vertical retrace.
        // Then the 192 screen+border lines are displayed, followed by 56 border lines again. Note that this means that a frame is
        // (64+192+56)*224=69888 T states long,
        int TS = 224; // 224 TS/scanline, 312 scanlines (first one is 24 ts + 50 ts(int) + 150 ts)
        for( int y = 0; y <  64; ++y ) { zx_vsync = y==16; sim(TS); if(y>(64-_24-1)) draw(app, y-(64-_24)); }
        for( int y = 0; y < 192; ++y ) { sim(TS);           draw(app, _24+y); }
        for( int y = 0; y <  56; ++y ) { sim(TS); if(y<_24) draw(app, _24+192+y); }
    } else {
        // 128K
        // There are 63 scanlines before the television picture, as opposed to 64.
        // To modify the border at the position of the first byte of the screen (see the 48K ZX Spectrum section for details), the OUT must finish after 14365, 14366, 14367 or 14368 T states have passed since interrupt. As with the 48K machine, on some machines all timings (including contended memory timings) are one T state later.
        // Note that this means that there are 70908 T states per frame, and the '50 Hz' interrupt occurs at 50.01 Hz, as compared with 50.08 Hz on the 48K machine. The ULA bug which causes snow when I is set to point to contended memory still occurs, and also appears to crash the machine shortly after I is set to point to contended memory.
        // (63+192+56)*228=70908 T states long,
        int TS = 228; // 228 TS/scanline, 311 scanlines
        for( int y = 1; y <  64; ++y ) { zx_vsync = y==1; sim(TS); if(y>(64-_24-1)) draw(app, y-(64-_24)); }
        for( int y = 0; y < 192; ++y ) { sim(TS);           draw(app, _24+y); }
        for( int y = 0; y <  56; ++y ) { sim(TS); if(y<_24) draw(app, _24+192+y); }
    }
}

#if 0
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
        zx_vsync = (tv == vs);
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


if(ZX_TV) {
    // barrel, requires higher res for subpixel accuracy
    static uint64_t barrel[_320*_240], *barrel_init = 0;
    static float t = 0; t+=1/60.f;
    if(!barrel_init)
    for(int x = 0; x < _320; ++x)
    for(float y = 0; y < _240; y += 0.1f) {
        double u = x / (_320-0.9f);
        double v = y / (_240-0.9f);
        double bendx = 7.2, bendy = 4.5;

        // [-1,+1] coords
        u = (u - 0.5) * 2.0;
        v = (v - 0.5) * 2.0;
        // zoom
        u /= 1.000;
        v *= 1.025;
        // bend
        u *= 1.0 + (v/bendx) * (v/bendx);
        v *= 1.0 - (u/bendy) * (u/bendy);
        // [0,1] coords
        u = (u / 2.0) + 0.5;
        v = (v / 2.0) + 0.5;
        // clamp & store
        float nx = (u<0?0:u>1?1:u) * (_319);
        float ny = (v<0?0:v>1?1:v) * (_239);
        barrel[x + (int)(ny) * _320] = nx + (int)y * _320;
//        barrel[x + (int)y * _320] = (unsigned)nx << 16 + (unsigned)ny;
    }
    if(0)
    if(!barrel_init)
    for(int y = 0; y < _240; ++y)
    for(int x = 0; x < _320; ++x) {
        if(x<1||x>=_319||y<1||y>=_239) { barrel[x+y*_320] = x+y*_320; continue; }
        unsigned nxl = (barrel[x-1+y*_320] >> 16);
        unsigned nyl = (barrel[x-1+y*_320] & 0xFFFF);
        unsigned nxc = (barrel[x+0+y*_320] >> 16);
        unsigned nyc = (barrel[x+0+y*_320] & 0xFFFF);
        unsigned nxr = (barrel[x+1+y*_320] >> 16);
        unsigned nyr = (barrel[x+1+y*_320] & 0xFFFF);
        unsigned nxt = (barrel[x+y*_320-_320] >> 16);
        unsigned nyt = (barrel[x+y*_320-_320] & 0xFFFF);
        unsigned nxb = (barrel[x+y*_320+_320] >> 16);
        unsigned nyb = (barrel[x+y*_320+_320] & 0xFFFF);
        barrel[x+nyc*_320] = ((nxl+nxr+nxt+nxb) / 4) + (int)y*_320;
    }
    barrel_init = barrel;

    static window *copy = 0;
    if(!copy) copy = tigrBitmap(_320, _240);
    static window *blend = 0;
    if(!blend) blend = tigrBitmap(_320, _240);

    // scanlines
    scanlines(app);

#if 0
    // barrel
    for( int p = 0; p < _320*_240; ++p ) copy->pix[p] = app->pix[barrel[p]];
    // ghosting
    tigrBlitAlpha(blend, copy, 0,0, 0,0, _320,_240, 0.5f); //0.15f);
    tigrBlit(app, blend, 0,0, 0,0, _320,_240);
#else
    // ghosting
    tigrBlitAlpha(blend, app, 0,0, 0,0, _320,_240, 0.5f); //0.15f);
    tigrBlit(app, blend, 0,0, 0,0, _320,_240);
#endif

    // aberrations
    flick_hz ^= 1; // &1; // (rand()<(RAND_MAX/255));
    flick_frame = cpu.r; // &1; // (rand()<(RAND_MAX/255));
    blur(app);

return;

    for( int y = 1; y < _239; ++y )
    for( int x = 1; x < _319; ++x ) {
        TPixel pl = app->pix[x-1+y*_320];
        TPixel pr = app->pix[x+1+y*_320];
        TPixel pt = app->pix[x+0+y*_320-_320];
        TPixel pb = app->pix[x+0+y*_320+_320];
        app->pix[x+y*_320] = (TPixel){
        (pl.r+pr.r+pt.r+pb.r)/4,
        (pl.g+pr.g+pt.g+pb.g)/4,
        (pl.b+pr.b+pt.b+pb.b)/4,
        255};
    }

}

}


void db_set(const char *key, int value) {
//    printf("db[%s]<=%d\n", key, value);
    char fname[1024+1];
    snprintf(fname, 1024, "%s.db", key);
    for(FILE *fp = fopen(fname, "wb"); fp; fclose(fp), fp=0) {
        fprintf(fp, "%d", value);
    }
}
int db_get(const char *key) {
    char fname[1024+1];
    snprintf(fname, 1024, "%s.db", key);
    int value = 0;
    for(FILE *fp = fopen(fname, "rb"); fp; fclose(fp), fp=0) {
        fscanf(fp, "%d", &value);
    }
//  printf("db[%s]=>%d\n", key, value);
    return value;
}
char **games = 0;
int *dbgames = 0;
int numgames = 0;
int numok=0,numwarn=0,numerr=0; // stats
void rescan() {
    // clean up
    while( numgames ) free(games[--numgames]);
    free(games);

    // refresh stats
    {
        numok=0,numwarn=0,numerr=0;

        for( dir *d = dir_open("./games/", "r"); d; dir_close(d), d = NULL ) {
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
                if( file_is_supported(fname) ) {
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
}

int active = 0, selected = 0, scroll = 0;
int game_browser() { // returns true if loaded
    if( window_trigger(app, TK_ESCAPE) )         active ^= 1;
    if( window_trigger(app, TK_F2) && !mic_last_type[0] ) active ^= 1; // open browser if start_tape is requested but no tape has been ever inserted

    tigrClear(ui, !active ? tigrRGBA(0,0,0,0) : tigrRGBA(0,0,0,128));

    if( !active ) return 0;

//  tigrBlitTint(app, app, 0,0, 0,0, _320,_240, tigrRGB(128,128,128));

    enum { ENTRIES = (_240/11)-4 };
    static char *buffer = 0; if(!buffer) { buffer = malloc(65536); /*rescan();*/ }
    if (!numgames) return 0;
    if( scroll < 0 ) scroll = 0;
    for( int i = scroll; i < numgames && i < scroll+ENTRIES; ++i ) {
        const char starred = dbgames[i] >> 8 ? (char)(dbgames[i] >> 8) : ' ';
        sprintf(buffer, "%c %3d.%s%s\n", starred, i+1, i == selected ? " > ":" ", 1+strrchr(games[i], DIR_SEP) );
        window_printxycol(ui, buffer, 1, 4+(i-scroll-1),
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
        window_printxycol(ui, tmp2, 1,2, tigrRGB(0,192,255));
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
        int color = dbgames[selected] & 0xFF;;
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
                int la = strlen(games[selected]);
                int lb = strlen(last_load);
                if( la == lb ) {
                    int diff = 0;
                    for( int i = 0; i < la; ++i ) {
                        diff += games[selected][i] - last_load[i];
                    }
                    insert_next_disk_or_tape = diff == 1;
                }
            }
        }
        bool must_clear = insert_next_disk_or_tape ? 0 : 1;

        if(must_clear)
        clear(window_pressed(app, TK_SHIFT) ? 48 : strstri(games[selected], ".dsk") ? 300 : 128);

        if( load(games[selected], must_clear, window_pressed(app,TK_CONTROL) ? 0:1) ) {
            window_title(app, va("Spectral %s%d - %s", ZX > 128 ? "+":"", ZX > 128 ? ZX/100:ZX, 1+strrchr(games[selected], DIR_SEP)));
        }

        return 1;
    }

    return 0;
}

int main() {
    // install icon hooks for any upcoming window or modal creation
    window_override_icons();
    warning("v0.1");

    // relocate cwd to exe folder
    char path[MAX_PATH]={0};
    GetModuleFileName(0,path,MAX_PATH);
    *strrchr(path, '\\') = '\0';
    SetCurrentDirectoryA(path);

    // app
    app = window_open(_32+256+_32, _24+192+_24, "Spectral");

    ui = tigrBitmap(_320, _240);

    // scan files
    rescan();

    // zx
    clear(ZX);

    // must be as close to frame() as possible
    audio_init();

    do {
        int accelerated = ZX_FAST ? boost_on || (mic_on && mic_has_tape) : 0;
        if( accelerated && active ) accelerated = 0;

        // z80, ula, audio, etc
        // static int frame = 0; ++frame;
        int do_drawmode = 1; // no render (<0), full screen (0), scanlines (1)
        int do_flashbit = accelerated ? 0 : 1;
        int do_runahead = accelerated || ZX >= 300 ? 0 : RUNAHEAD;

#if AYUMI
        do_runahead = 0; // not compatible (see: TargetRenegade128K)
#endif

        input();

        static byte counter = 0; // flip flash every 16 frames @ 50hz
        if( !((++counter) & 15) ) if(do_flashbit) ZXFlashFlag ^= 1;

if( do_runahead == 0 ) {
        do_audio = 1;
        frame(do_drawmode); //accelerated ? (frame%50?0:1) : 1 );
} else {
        do_audio = 0;
        frame(-1);

        checkpoint_save(10);

        do_audio = 1;
        frame(do_drawmode);

        checkpoint_load(10);
}

        // game browser
        int game_loaded = game_browser();

        if( do_runahead == 1 && !game_loaded /*&& !accelerated*/ ) {
//            checkpoint_load(10);
        }

        // measure time & frame lock (50.01 fps)
        float dt;
        if(accelerated) {
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
#else // accurate (beware of CPU usage)
            // be nice to os
            sys_sleep(5);
            // complete with shortest sleeps (yields) until we hit target fps
            dt = tigrTime();
            for( float target_fps = 1.f/(ZX < 128 ? 50.08:50.01); dt < target_fps; ) {
                sys_yield();
                dt += tigrTime();
            }
#endif
        }

        // calc fps
        static float fps = 0;
        static int frames = 0; ++frames;
        static double time_now = 0; time_now += dt;
        if( time_now >= 1 ) { fps = frames / time_now; time_now = frames = 0; }

        // app timer
        static double accum = 0;
        if(mic_on && mic_has_tape) accum += dt;

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
            snprintf(compat, 64, "OK:%04.1f%% ", (total-numerr) * 100.f / (total+!total));
            window_printxy(ui, compat, 0,(_240-12.0)/11);
            }

            // stats & debug
            static char info[64] = "";
            char *ptr = info;
            ptr += sprintf(ptr, "%dm%02ds ", (unsigned)(accum) / 60, (unsigned)(accum) % 60);
            ptr += sprintf(ptr, "%5.2ffps%s %d mem%d%d%d%d%s ", fps, do_runahead ? "!":"", ZX, !!(page128&16), (page128&8?7:5), 2, page128&7, page128&32?"!":"");
            ptr += 1/*mic_on*/ ? sprintf(ptr, "%03d/%s(%c) ", mic_last_block, mic_last_type, mic_invert_polarity?'-':'+') : 0;
            ptr += sprintf(ptr, "x%.4f ", AZIMUTH);
//          ptr += sprintf(ptr, "%03d %05x", autotape_counter, autotape_indicators);

#ifdef NDEBUG
            if( window_pressed(app, TK_SHIFT) || active )
#endif
	            window_print(ui, info);

            // tape progress
            float pct = (voc_pos/4) / (float)(voclen+!voclen);
            if( mic_on && (pct < 1.00f) ) {
                TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + 0 * _320];
                // bars & progress
                unsigned mark = pct * _320;
                for( int x = 0; x < _320; ++x ) bar[x] = bar[x+2*_320] = white;
                for( int x = 0; x<=mark; ++x ) bar[x+_320] = white; bar[_320-1+_320] = white;
                for( int x = 0; x < _320; ++x ) if(mic_preview[x]) bar[x+1*_320] = white;
                // triangle marker (top)
                bar[mark+4*_320] = white;
                for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+5*_320] = white;
                for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+6*_320] = white;
                // mouse seeking
                int mx, my, mbuttons;
                tigrMouse(app, &mx, &my, &mbuttons);
                if( mbuttons && my > 0 && my <= 15 ) {
                    float target = (mx / (float)_320) * (float)(voclen+!voclen);
                    voc_pos = (voc_pos/4) * 0.95f + target * 0.05f ; // animate seeking
                    voc_pos *= 4;
                }
            }

            #define CLAMP(v, minv, maxv) ((v) < (minv) ? (minv) : (v) > (maxv) ? (maxv) : (v))
            #define REMAP(var, src_min, src_max, dst_min, dst_max) \
                (dst_min + ((CLAMP(var, src_min, src_max) - src_min) / (float)(src_max - src_min)) * (dst_max - dst_min))

            // azimuth slider
#ifdef NDEBUG
            if(0)
#endif
            if( !active ) {
                TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + (_240-7) * _320];
                unsigned mark = REMAP(AZIMUTH, 1.0,1.2, 0,1) * _320;
                // triangle marker (bottom)
                for(int i = -2; i <= +2; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+0*_320] = white;
                for(int i = -1; i <= +1; ++i) if((mark+i)>=0 && (mark+i)<_320) bar[mark+i+1*_320] = white;
                bar[mark+2*_320] = white;
                bar += _320 * 4;
                // bars & progress
                for( int x = 0; x < _320; ++x ) bar[x] = bar[x+2*_320] = white;
                for( int x = 0; x<=mark; ++x ) bar[x+_320] = white; bar[_320-1+_320] = white;
                // mouse seeking
                int mx, my, mbuttons;
                tigrMouse(app, &mx, &my, &mbuttons);
                if( (mbuttons/*&4*/) && my >= (_240-10) && my < _240 ) {
                    float target = REMAP(mx, 0,_320, 0.98,1.2);
                    AZIMUTH = AZIMUTH * 0.95f + target * 0.05f ; // animate seeking
                }
            }
        }

        // draw ui on top
        tigrBlitAlpha(app, ui, 0,0, 0,0, _320,_240, 1.0f);

        // flush
        window_update(app);

    } while( window_alive(app) );

    window_close(app);
}
