// snapshot file spec
// header [16] + version [16]
// z80 regs in alphabetical order [16 each]
//  AF AF2 BC BC2 DE DE2 HL HL2 IFF12 IM IR IX IY PC SP
// num ports (in ascending order) [16]
//  addr [16], data [16]
// num sequential pages (in ascending order) [16]
//  size [16], blob [N] (z80rle if size < 16k; not paged in if size == 0, thus blob is skipped)
// num devices (tap, tzx, dsk, ...) [16]
//  size [16], blob [N]

const uint16_t STATE_HEADER = 'Xp';
const uint16_t STATE_VERSION = '\x00\x11';

int export_state(FILE *fp) {
    if( !fp ) return 0;

    regs("export_state");

    uint16_t count = 0, errors = 0, temp;

    #define putnn(ptr,len) \
        ( errors += fwrite( (count += (len), (ptr)), (len), 1, fp ) != 1 )

    #define put16(value) \
        putnn( ( temp = (value), &temp ), 2 )

    put16(STATE_HEADER);
    put16(STATE_VERSION);

    put16(ZX);

    put16(cpu.af);
    put16(cpu.af2);
    put16(cpu.bc);
    put16(cpu.bc2);
    put16(cpu.de);
    put16(cpu.de2);
    put16(cpu.hl);
    put16(cpu.hl2);       uint16_t cpu_iff12 = IFF1(cpu) << 8 | IFF2(cpu) & 255;
    put16(cpu_iff12);
    put16(cpu.im);        uint16_t cpu_ir = I(cpu) << 8 | R(cpu) & 255;
    put16(cpu_ir);
    put16(cpu.ix);
    put16(cpu.iy);
    put16(cpu.pc);
    put16(cpu.sp);

    if( ZX >= 210 ) put16(0x1ffd), put16(page2a); // +2A, +3
    if( ZX >= 128 ) put16(0x7ffd), put16(page128); // any 128
    if( ZX >= 128 ) for( int i = 0; i < 16; ++i ) { // any 128
                        int reg = ( ay_current_reg + i + 1 ) & 15;
                        put16(0xfffd), put16(reg);
                        put16(0xbffd), put16(ay_registers[reg]);
                    }
                    put16(0x00fe), put16(ZXBorderColor); // any ZX

    put16(0x0000); // terminator

    // @todo: ulaplus+

    for( int i = 0; i < 16; ++i ) {
        put16(0x4000);
        putnn(RAM_BANK(i), 0x4000);
    }

    if( errors ) warning("export errors");
    return errors ? 0 : count;
}

int import_state(FILE *fp) {
    if( !fp ) return 0;

    uint16_t count = 0, errors = 0, temp;

    #define getnn(ptr,len) \
        ( errors += fread( (count += (len), (ptr)), (len), 1, fp ) != 1 )

    #define get16(value) \
        (getnn(&temp, 2), value = temp)

    #define check16(value) \
        if( (get16(temp), temp) != value ) return 0

    check16(STATE_HEADER);
    check16(STATE_VERSION);

    get16(ZX);
    reset(ZX);
//  memset(ay_registers, 0, sizeof(ay_registers)); ay_current_reg = 0; // why not in reset()?

    get16(cpu.af);
    get16(cpu.af2);
    get16(cpu.bc);
    get16(cpu.bc2);
    get16(cpu.de);
    get16(cpu.de2);
    get16(cpu.hl);
    get16(cpu.hl2);     uint16_t cpu_iff12;
    get16(cpu_iff12);   IFF1(cpu)=cpu_iff12>>8; IFF2(cpu)=cpu_iff12&255;
    get16(cpu.im);      uint16_t cpu_ir;
    get16(cpu_ir);      I(cpu)=cpu_ir>>8; R(cpu)=cpu_ir&255;
    get16(cpu.ix);
    get16(cpu.iy);
    get16(cpu.pc);
    get16(cpu.sp);

    for(;;) {
        uint16_t port, data;
        get16(port); if(!port) break; get16(data);

        /**/ if( port == 0x00fe ) outport(port, ZXBorderColor = data );
        else if( port == 0x7ffd ) outport(port, page128 = data );
        else if( port == 0x1ffd ) outport(port, page2a = data );
        else                      outport(port, data);
    }

    for( int i = 0; i < 16; ++i ) {
        uint16_t banklen;
        get16(banklen);

        getnn(RAM_BANK(i), banklen);
    }

    regs("import_state");

    if( errors ) warning("import errors");
    return errors ? 0 : count;
}
