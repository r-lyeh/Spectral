//-----------------------------------------------------------------------------
// 3rd party libs

#define TIGR_C
#define TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
#include "3rd_tigr.h"
#include "3rd_tigrmousecursor.h"
#include "3rd_tigrdragndrop.h"

#if 0
#define LUA_IMPL                              // lua544
#define TK_END TK_END2
#define TK_RETURN TK_RETURN2
#define block block2
#include "3rd_lua.h"
#undef TK_END
#undef TK_RETURN
#endif

#define SOKOL_AUDIO_IMPL
#include "3rd_sokolaudio.h"

#define DEFLATE_C
#include "3rd_deflate.h"
#include "3rd_zlib.h" // for zlib streams, like CSW2
#define ZIP_C
#include "3rd_zip.h"
#include "3rd_rar.h"
#define DIR_C
#include "3rd_dir.h"

#if 0 // defined __has_include
#	if __has_include ("3rd_sqlite3.c")
// 		gcc: #pragma GCC warning "warning message"
// 		msc: #pragma message ( "your warning text here" ) // works in gcc too, albeit not as intended
#		pragma message ("ZXDB/Sqlite enabled")
#		define SQLITE_C 1
#	endif
#endif

#if SQLITE_C
//#include "3rd_sqlite3.h"
#include "3rd_sqlite3.c"
#undef TK_ESCAPE
#undef TK_SPACE
#undef TK_END
#undef TK_INSERT
#undef TK_DELETE
#undef TK_LSHIFT
#undef TK_RSHIFT
#undef TK_COMMA
#undef TK_MINUS
#undef TK_DOT
#undef TK_SLASH
#undef NB
#endif

#include "3rd_bin.h"
