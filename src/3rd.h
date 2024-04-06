//-----------------------------------------------------------------------------
// 3rd party libs

#define TIGR_C
#include "3rd_tigr.h"

#define LUA_IMPL                              // lua544
#define TK_END TK_END2
#define TK_RETURN TK_RETURN2
#define block block2
#include "3rd_lua.h"
#undef TK_END
#undef TK_RETURN

#define MA_NO_FLAC                            // miniaudio
#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING                 // miniaudio osx
#endif
#define MINIAUDIO_IMPLEMENTATION              // miniaudio
#include "3rd_miniaudio.h"

#define DEFLATE_C
#include "3rd_deflate.h"
#define ZIP_C
#include "3rd_zip.h"
#define DIR_C
#include "3rd_dir.h"
