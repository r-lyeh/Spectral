// console utilities,
// - rlyeh, public domain

#if 0
#define ANSI_BOLD       "\x1B[1m"
#define ANSI_BRIGHT     "\x1B[1m"
#define ANSI_BLINK      "\x1B[5m"
#define ANSI_BLINK_OFF  "\x1B[25m"
#define ANSI_CLEAR_LINE "\x1B[2K"
#define ANSI_RESET      "\x1B[0m"
#endif



#define ANSI_BLUE   "\x1B[34;1m"
#define ANSI_RED    "\x1B[31;1m"
#define ANSI_PURPLE "\x1B[35;1m"
#define ANSI_GREEN  "\x1B[32;1m"
#define ANSI_CYAN   "\x1B[36;1m"
#define ANSI_YELLOW "\x1B[33;1m"
#define ANSI_WHITE  "\x1B[37;1m"
#define ANSI_GREY   "\x1B[30;1m"
#define ANSI_RESET  "\x1B[m"

#ifdef _WIN32
#include <io.h>
void ansi_quit(void) { printf(ANSI_RESET); }
void ansi() {
    atexit(ansi_quit);
    unsigned mode;
    void *handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleMode(handle, &mode)) {
        mode |= 0x0004; /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
        SetConsoleMode(handle, mode); /* ignore errors */
    }
    system("chcp 65001 >nul"); // enable unicode
}
#else
void ansi() {}
#endif

#define cprintf(fmt, ...) cprintf(va(fmt, __VA_ARGS__))

int (cprintf)(const char *x) {
    static int once = 1; for(;once;once=0) ansi();
    const char *colors[] = {
        ANSI_BLUE,   // "\x1B[1;34;40m"
        ANSI_RED,    // "\x1B[1;31;40m"
        ANSI_PURPLE, // "\x1B[1;35;40m"
        ANSI_GREEN,  // "\x1B[1;32;40m"
        ANSI_CYAN,   // "\x1B[1;36;40m"
        ANSI_YELLOW, // "\x1B[1;33;40m"
        ANSI_GREY,   // 
//        ANSI_WHITE,  // "\x1B[1;37;40m"
    };
    int bytes = 0;
    for ( int i = 0; x[i]; ++i ) {
        if( (unsigned)x[i] < 8 ) printf("%s", colors[x[i]-1]);
        else printf("%c", i[x]), ++bytes;
    }
    return bytes;
}

int cputs(const char *x) {
    return !!cprintf("%s\n", x);
}
