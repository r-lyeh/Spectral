typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;
typedef unsigned int   rgba;
typedef uint8_t        byte;

#define do_once  static int _ = 1; for( ; _ ; _ = 0 )
#define __thread __declspec(thread)

#include "sys_file.h"
#include "sys_string.h"
#include "sys_sleep.h"
#include "sys_video.h"
#include "sys_audio.h"
#include "sys_icon.h"

#ifndef _WIN32
#define min(a,b) ((a)<(b)?(a):(b))
#define FALSE 0
#define ShowCursor(...)     (void)0
#define ClipCursor(...)     (void)0
#define SetWindowTextA(...) (void)0
#define GetWindowRect(...)  (void)0
typedef int RECT;
#define sys_sleep(x)        usleep((x)*1000)
#define _popen popen
#define _pclose pclose
#ifdef __GNUC__ // also, clang
    int __argc;
    char **__argv;
    __attribute__((constructor)) void init_argcv(int argc, char **argv) {
        __argc = argc;
        __argv = argv;
    }
#endif
#endif
