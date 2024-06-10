//-----------------------------------------------------------------------------
// 3rd party libs

#define TIGR_C
#define TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
#include "3rd_tigr.h"
#include "3rd_tigrobjc.h"
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
#include "3rd_gz.h" // gunzip
#define ZIP_C
#include "3rd_zip.h"
#include "3rd_rar.h"
#define DIR_C
#include "3rd_dir.h"
#define STB_IMAGE_IMPLEMENTATION
#include "3rd_stbimage.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rd_stbimage_resize2.h"

#if 1
#define TFD_IMPLEMENTATION
extern Tigr *app;
#define GetForegroundWindow() ((HWND)(app->handle))
#include "3rd_tfd.h"
#undef  GetForegroundWindow
#else
#include "3rd_osdialog.h"
#include "3rd_osdialog.c"
#  if defined __APPLE__
#include "3rd_osdialog_mac.m"
#elif defined _WIN32
#include "3rd_osdialog_win.c"
#pragma comment(lib, "comdlg32")
#else
//#include "3rd_osdialog_gtk2.c"
#include "3rd_osdialog_gtk3.c"
#endif
#endif


#include "3rd_bin.h"


#ifdef _WIN32

#include <wininet.h>
#pragma comment(lib,"wininet")

char* download( const char *url, int *len ) { // must free() after use
    char *ptr = 0; int cap = 0;

    int ok = 0;
    char buffer[ 4096 ];
    DWORD response_size = 0;

    for( HINTERNET session = InternetOpenA("" /*"fwk.download_file"*/, PRE_CONFIG_INTERNET_ACCESS, NULL,NULL/*INTERNET_INVALID_PORT_NUMBER*/, 0); session; InternetCloseHandle(session), session = 0 ) // @fixme: download_file
    for( HINTERNET request = InternetOpenUrlA(session, url, NULL, 0, INTERNET_FLAG_RELOAD|INTERNET_FLAG_SECURE, 0); request; InternetCloseHandle(request), request = 0 )
    for( ; (ok = !!InternetReadFile(request, buffer, sizeof(buffer), &response_size)) && response_size > 0 ; ) {
        ptr = realloc(ptr, cap += response_size );
        memcpy(ptr + (cap - response_size), buffer, response_size);
    }

    if( !ok ) {
        if( ptr ) free(ptr);
        return NULL;
    }

    if( len ) *len = cap;
    return ptr;
}

#elif 1

char* download( const char *url, int *len ) { // must free() after use
    char *ptr = 0; int cap = 0;

    int ok = 0;
    char buffer[ 4096 ];

    if( url[0] != '!' ) {
        if( !ok ) sprintf(buffer, "!curl -L '%s' 2>/dev/null", url), ptr = download(buffer, len), ok = !!ptr;
        if( !ok ) sprintf(buffer, "!wget -qO- '%s' 2>/dev/null", url), ptr = download(buffer, len), ok = !!ptr;
        if(  ok ) return ptr;
    }
    else
    for( FILE *fp = popen(url+1, "r"); fp; pclose(fp), fp = 0)
    for(; !feof(fp); ) {
        int count = fread(buffer, 1, sizeof(buffer), fp);
        ok = count > 0;
        if(!ok) break;

        ptr = realloc(ptr, cap += count );
        memcpy(ptr + (cap - count), buffer, count);
    }

    if( !ok ) {
        if( ptr ) free(ptr);
        return NULL;
    }

    if( len ) *len = cap;
    return ptr;
}

#else

#define HTTPS_IMPLEMENTATION
#define copy copy2
#include "3rd_https.h"
#undef I
#undef R

int download_file( FILE *out, const char *url ) {
    int ok = false;
    if( out ) for( https_t *h = https_get(url, NULL); h; https_release(h), h = NULL ) {
        while (https_process(h) == HTTPS_STATUS_PENDING)
#ifdef _WIN32
        	Sleep(1); // 1ms
#else
        	usleep(1000); // 1ms
#endif
        printf("fetch status L%d, %d %s\n\n%.*s\n", https_process(h), h->status_code, h->content_type, (int)h->response_size, (char*)h->response_data);
        if(https_process(h) == HTTPS_STATUS_COMPLETED)
        ok = fwrite(h->response_data, h->response_size, 1, out) == 1;
    }
    return ok;
}

#endif
