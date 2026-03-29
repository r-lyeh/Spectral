// DISCiPLE interface (Miles Gordon Technology / Rockfort, 1986)
// - 8 KiB ROM + 8 KiB RAM, paged over 0x0000..0x3FFF. WD1770 FDC. Two drives. Kempston port. Parallel printer.
// - ports are decoded on A0..A7 only, and are always decoded (paged in or not).
// - reference: fuse/peripherals/disk/disciple.c + fuse/z80/z80_ops.c
//
// memory (paged in):
//   memswap=0: 0x0000 ROM | 0x2000 RAM      (state after reset, and after IN 0x7B)
//   memswap=1: 0x0000 RAM | 0x2000 ROM      (after OUT 0x7B)
//
// automatic page-in: opcode fetch (M1) at 0x0001, 0x0008, 0x0066 (NMI), 0x028E (48 ROM KEY-SCAN)
//
// ports:
//   0x1B r:status   w:command   (WD1770)
//   0x5B r/w:track
//   0x9B r/w:sector
//   0xDB r/w:data
//   0x1F r:kempston joystick + bit6 printer busy (0=busy/not attached)
//        w:control: b0 drive (1=drive 1, 0=drive 2), b1 side, b2 density, b3 rom bank (16K roms only),
//                   b4 inhibit, b5 exp select, b6 printer strobe, b7 network
//   0x3B w:network (not emulated; left unhandled so ULAplus ports 0xBF3B/0xFF3B still work)
//   0x7B r:memswap=0  w:memswap=1
//   0xBB r:page in    w:page out
//   0xFB w:printer data (not emulated)

#include "res/roms/disciple" // const unsigned char romdisciple[8192]; (bin2c disciple.rom disciple.c romdisciple)

typedef char disciple_rom_must_be_8k[ sizeof(romdisciple) == 0x2000 ? 1 : -1 ];

int  ZX_DISCIPLE = 0;       // 0: off, 1: on (auto-enabled when a .mgt disk is guessed)

byte disciple_enabled;      // ZX_DISCIPLE && compatible model. cached at reset (hot path flag)
byte disciple_active;       // 1 if paged in
byte disciple_memswap;      // 0: ROM@0000 RAM@2000, 1: RAM@0000 ROM@2000
byte disciple_control;      // last OUT 0x1F
byte disciple_printer;      // last OUT 0xFB
byte disciple_page[0x4000]; // live 16K view. halves are physically swapped on memswap changes

void disciple_map() {
    // reads come from the 16K view; writes are filtered by disciple_write() so ROM half stays read-only
    MEMr[0] = disciple_active ? disciple_page : ROM_BANK(GET_MAPPED_ROMBANK());
    MEMw[0] = DUMMY_BANK(0);
}

void disciple_swap(int memswap) {
    memswap = !!memswap;
    if( disciple_memswap == memswap ) return;
    byte tmp[0x2000];
    memcpy(tmp, disciple_page, 0x2000);
    memcpy(disciple_page, disciple_page + 0x2000, 0x2000);
    memcpy(disciple_page + 0x2000, tmp, 0x2000);
    disciple_memswap = memswap;
}

void disciple_pagein()  { disciple_active = 1; disciple_map(); }
void disciple_pageout() { disciple_active = 0; disciple_map(); }

void disciple_reset(int hard) {
    // 16K/48K/128/+2 only. blocked on +2A/+3 (edge connector) and pentagon (trdos).
    disciple_enabled = ZX_DISCIPLE && ZX <= 200 && !ZX_PENTAGON;
    disciple_active = 0;
    if( !disciple_enabled ) return;

    // 128/+2: the DISCiPLE boot rom ends with `LD HL,0x11CB / JP 0x004F` -> `PUSH HL / OUT (0xBB),A / RET`,
    // which pages itself out and jumps into NEW at 0x11CB of the 48K ROM. ROM0 (128 editor) would crash there,
    // so select ROM1 (48 BASIC) at reset. paging stays unlocked, so 128K software can still bank memory.
    if( ZX >= 128 ) port_0x7ffd(0x10);

    disciple_swap(0);
    memcpy(disciple_page, romdisciple, 0x2000);
    if( hard ) memset(disciple_page + 0x2000, 0, 0x2000);

    disciple_control = 0;
    disciple_printer = 0;
    wd.Drive = 0;
    wd.Side = 0;

    disciple_pagein(); // fuse: interface is paged in after reset
}

static inline
void disciple_trap(word pc) { // call on M1 opcode fetches
    if( !disciple_active )
    if( pc == 0x0001 || pc == 0x0008 || pc == 0x0066 || pc == 0x028E ) disciple_pagein();
}

static inline
void disciple_write(word addr, byte v) { // addr < 0x4000 && disciple_active
    if( (addr >> 13) == !disciple_memswap ) disciple_page[addr] = v; // RAM half only
}

static
byte wd1770_command(byte v) {
    // WD1770 has no side-compare flag. In type II commands bit1 is write-precomp and bit3 is motor-on
    // disable, so the WD1793 core must not interpret them as side compare (C_SIDECOMP/C_SIDE).
    if( (v & 0xC0) == 0x80 ) v &= ~C_SIDECOMP;
    return v;
}

int disciple_out(word port, byte v) { // returns 1 if handled
    switch( port & 0xFF ) {
        default: return 0;
        case 0x1B: Write1793(&wd, WD1793_COMMAND, wd1770_command(v)); return 1;
        case 0x5B: Write1793(&wd, WD1793_TRACK, v);  return 1;
        case 0x9B: Write1793(&wd, WD1793_SECTOR, v); return 1;
        case 0xDB: Write1793(&wd, WD1793_DATA, v);   return 1;
        case 0x1F:
            disciple_control = v;
            wd.Drive = (v & 0x01) ? 0 : 1; // bit0 set: drive 1 (fdd[0]). clear: drive 2 (fdd[1])
            wd.Side  = (v & 0x02) ? 1 : 0;
            return 1;
        case 0x7B: disciple_swap(1); disciple_map(); return 1;
        case 0xBB: disciple_pageout(); return 1;
        case 0xFB: disciple_printer = v; return 1;
    }
}

int disciple_in(word port, byte *v) { // returns 1 if handled
    switch( port & 0xFF ) {
        default: return 0;
        case 0x1B: *v = Read1793(&wd, WD1793_STATUS); return 1;
        case 0x5B: *v = Read1793(&wd, WD1793_TRACK);  return 1;
        case 0x9B: *v = Read1793(&wd, WD1793_SECTOR); return 1;
        case 0xDB: *v = Read1793(&wd, WD1793_DATA);   return 1;
        case 0x1F: *v = kempston & 0xBF; return 1; // kempston + bit6=0 (no printer). same as fuse's 0xBF & kempston
        case 0x7B: disciple_swap(0); disciple_map(); *v = 0; return 1;
        case 0xBB: disciple_pagein(); *v = 0; return 1;
    }
}
