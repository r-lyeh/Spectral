// @todo: hexdump mem contents (HL),(BC),etc on the right panel
// @todo: bonzomatic
// @todo: rom symbols and vars 05CA(LD-8-BITS) 5C6A(FLAGS2) etc.
// @todo: memwatch, exit subroutine, callstack
// @todo: breakpoint, break on next/specific port access/read/write,
// @todo: break on data condition, poke finder

unsigned dasm_pc;
char dasm_str[128] = {0};
uint8_t z80dasm_read(void *user_data) {
    uint8_t data = READ8(dasm_pc);
    return ++dasm_pc, data;
}
uint8_t z80dasm_write(char c, void *user_data) {
    char buf[2] = { c/*tolower(c)*/, 0};
    strncat(dasm_str, buf, 128);
    return 0;
}
char* z80dasm(unsigned pc) {
    int bytes = (*dasm_str = 0, z80dasm_op(dasm_pc = pc, z80dasm_read, z80dasm_write, NULL) - pc);

    char hexdump[128], *ptr = hexdump;
    for(int x=0;x<bytes;++x)
    ptr += sprintf(ptr, "%s%02X", " "+(!x), READ8(pc+x));

    char bank[8];
    sprintf(bank, "%d%d%d%d", !!(page128&16), (page128&8?7:5), 2, page128&7);

    return va("%d%s %04X: %-13s  %s", bytes, bank, pc, dasm_str, hexdump);
}

char* dis(unsigned pc, unsigned lines) {
    static char buf[80*25] = {0};
    char *ptr = buf;

    for( unsigned y = 0; y < lines; ++y ) {
        char *line = z80dasm(pc);
        pc += line[0] - '0';
        ptr += sprintf(ptr, "%s\n", line+1);
    }

    return buf;
}
char *regs(const char *title) {
    static char buf[80*25] = {0};
    char *ptr = buf;

    unsigned F = AF(cpu);
    if( title )
    ptr += sprintf(ptr, "\n--- %s ---\n", title);
    ptr += sprintf(ptr, "af:%04x,af'%04x,bc:%04x,bc':%04x,pc:%04x,SZYHXPNC\n", AF(cpu), AF2(cpu), BC(cpu), BC2(cpu), PC(cpu));
    ptr += sprintf(ptr, "de:%04x,de'%04x,hl:%04x,hl':%04x,sp:%04x,", DE(cpu), DE2(cpu), HL(cpu), HL2(cpu), SP(cpu));
    ptr += sprintf(ptr, "%d%d%d%d%d%d%d%d\n", !!(F & 0x80), !!(F & 0x40), !!(F & 0x20), !!(F & 0x10), !!(F & 0x8), !!(F & 0x4), !!(F & 0x2), !!(F & 0x1));
    ptr += sprintf(ptr, "iff%04x,im:%04x,ir:%02x%02x,ix :%04x,iy:%04x\n", IFF1(cpu) << 8 | IFF2(cpu), IM(cpu), I(cpu),R(cpu), IX(cpu), IY(cpu));
    ptr += sprintf(ptr, "ay reg%X ", ay_current_reg);
    for( int i = 0; i < 16; ++i ) 
    ptr += sprintf(ptr, "%02x", ay_registers[i]);
    ptr += sprintf(ptr, "\nmem%d%d%d%d%s", !!(page128&16), (page128&8?7:5), 2, page128&7, page128&32?"!":" "); 
    for( int i = 0; i < 16; ++i ) 
    ptr += sprintf(ptr, "%02x", ( crc32(0,RAM_BANK(i), 0x4000) ^ 0xab54d286) >> 24 );
    extern int rom_patches;
    ptr += sprintf(ptr, "\nFE:%02x 2A:%02x 128:%02x LED:%d MOD:%02x\n", ZXBorderColor, page2a, page128, !!fdc.led, rom_patches);

    return buf;
}
