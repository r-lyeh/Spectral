//-----------------------------------------------------------------------------
// 3rd party emu libs

#if 0

#pragma push_macro("IN")
#pragma push_macro("OUT")		
#undef IN
#undef OUT
#include "emu_z80red.h"
#pragma pop_macro("OUT")
#pragma pop_macro("IN")		

#define z80_t Z80
#define z80_init(x) (z80_power(x,1), 0)
#define z80_reset(x) z80_instant_reset(x)
#define z80_tick(x,pins) (z80_run(x,1),0) // z80_execute()

#else

#define CHIPS_IMPL
#include "emu_z80.h"

#endif

#include "emu_spk.h"
#if AYUMI
#include "emu_ayumi.h"
#else
#include "emu_ay.h"
#endif
#include "emu_fdc.h"
