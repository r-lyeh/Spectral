// io utilities,
// - rlyeh, public domain

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>


#ifdef _WIN32
#define realpath(inpath,outpath) _fullpath(outpath, inpath, MAX_PATH - 1)
#endif

#ifdef __linux__
#define basename basename__linux__
#endif

#define DIR_MAX MAX_PATH
#define DIR_SEP_ "/"

#ifdef _WIN32
#define open8(path,mode)  _wopen(widen(path))
#define popen8(path,mode) _wpopen(widen(path),widen(mode))
#define fopen8(path,mode) _wfopen(widen(path),widen(mode))
#define remove8(path)     _wremove(widen(path))
#define rename8(path)     _wrename(widen(path))
#define stat8(path,st)    _wstat(widen(path),st) // _stati64()
#define stat8_t           _stat                  // struct _stati64
#else
#define open8(path,mode)  open(path,mode)
#define popen8(path,mode) popen(path,mode)
#define fopen8(path,mode) fopen(path,mode)
#define remove8(path)     remove(path)
#define rename8(path)     rename(path)
#define stat8(path,st)    stat(path,st)    // _stati64()
#define stat8_t           stat             // struct _stati64
#endif

#include <stdio.h>
unsigned char *readfile(const char *pathfile, int *size) {
    unsigned char *bin = 0;
    if( size ) *size = 0;
    for( FILE *fp = fopen8(pathfile,"rb"); fp; fclose(fp), fp = 0) {
        fseek(fp, 0L, SEEK_END);
        size_t len = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        bin = (unsigned char*)malloc(len+1);
        if( bin && fread(bin, 1, len, fp) == len ) bin[len] = '\0';
        else free(bin), bin = 0;
        if(size && bin) *size = (int)len;
    }
    return bin;
}
int writefile(const char *pathfile, const void *blob, int len) {
    int ok = 0;
    FILE *fp = fopen8(pathfile, "wb");
    if( fp ) {
        ok = fwrite(blob, len, 1, fp) == 1;
        fclose(fp);
    }
    return ok;
}
int is_folder( const char *pathfile ) {
    // @fixme: win32+tcc wont like ending slashes in stat()
    struct stat8_t st;
    return stat8(pathfile, &st) >= 0 ? S_IFDIR == ( st.st_mode & S_IFMT ) : 0;
}
int is_file( const char *pathfile ) {
    struct stat8_t st;
    return stat8(pathfile, &st) >= 0;
}
const char *basename( const char *pathfile ) {
    const char *a = strrchr(pathfile, '/');  a += !!a;
    const char *b = strrchr(pathfile, '\\'); b += !!b;
    return a > b ? a : b > a ? b : pathfile;
}

void cwdexe(void) {
#ifdef __APPLE__
    char buffer[MAX_PATH]={0};
    realpath(__argv[0],buffer);
    if(strrchr(buffer,'/')) 1[strrchr(buffer,'/')] = '\0';
    chdir(buffer);
#elif defined _WIN32
    // relocate cwd to exe folder (relative paths wont work from this point)
    char path[MAX_PATH]={0};
    GetModuleFileName(0,path,MAX_PATH);
    *strrchr(path, '\\') = '\0';
    SetCurrentDirectoryA(path);
#else
    char buffer[MAX_PATH];
    char path[32] = {0};
    sprintf(path, "/proc/%d/exe", getpid());
    readlink(path, buffer, sizeof(buffer));
    if(strrchr(buffer,'/')) 1[strrchr(buffer,'/')] = '\0';
    chdir(buffer);
#endif
}


void hexdump( const void *ptr, unsigned len ) {
    FILE *fp = stdout;
    enum { width = 16 };
    unsigned char *data = (unsigned char*)ptr;
    for( unsigned jt = 0; jt <= len; jt += width ) {
        fprintf( fp, "; %05d%s", jt, jt == len ? "\n" : " " );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            fprintf( fp, "%02x %s", (unsigned char)data[it], &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
        fprintf( fp, "; %05d%s", jt, jt == len ? "\n" : " " );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            fprintf( fp, " %c %s", (signed char)data[it] >= 32 ? (signed char)data[it] : (signed char)'.', &" \n\0..."[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
    }
    fprintf(fp, " %d bytes\n", len);
}


const char* app_loadfile() {
    char cwd[DIR_MAX] = {0}; getcwd(cwd, DIR_MAX);
#ifdef TFD_IMPLEMENTATION
    const char *windowTitle = NULL;
    const char *filterHints = NULL; // "image files"
    const char *filters[] = { "*.*" };
    int allowMultipleSelections = 0;

    //tinyfd_assumeGraphicDisplay = 1;
    return tinyfd_openFileDialog( windowTitle, cwd, countof(filters), filters, filterHints, allowMultipleSelections );
#else
    return osdialog_file(OSDIALOG_OPEN, cwd, NULL, NULL);
#endif
}
const char* app_savefile() {
    char cwd[DIR_MAX] = {0}; getcwd(cwd, DIR_MAX);
#ifdef TFD_IMPLEMENTATION
    const char *windowTitle = NULL;
    const char *filterHints = NULL; // "image files"
    const char *filters[] = { "*.*" };

    //tinyfd_assumeGraphicDisplay = 1;
    return tinyfd_saveFileDialog( windowTitle, cwd, countof(filters), filters, filterHints );
#else
    return osdialog_file(OSDIALOG_SAVE, cwd, NULL, NULL);
#endif
}
const char* app_selectfolder(const char *title, const char *origin) {
    char cwd[DIR_MAX] = {0}; getcwd(cwd, DIR_MAX);
#ifdef TFD_IMPLEMENTATION
    return tinyfd_selectFolderDialog(title, origin && origin[0] ? origin : cwd);
#else
    return osdialog_file(OSDIALOG_OPEN_DIR, origin && origin[0] ? origin : cwd, NULL, NULL);
#endif
}


const char *cwd() {
    static char buf[DIR_MAX] = {0};
    return !buf[0] ? getcwd(buf, sizeof(buf)) : buf;
}

const char *normpath(const char *path) {
    if( !path[0] ) return "";

    static char result[DIR_MAX]; result[0] = '\0';

    // copy
    strncpy(result, path, DIR_MAX);
    // normalize
    for( int i = 0; result[i]; ++i ) if(result[i] == '\\') result[i] = '/';
    // no double slashes. use memmove (strcpy on same string is UB)
    for( int i = 0; result[i]; ++i ) {
        while( result[i] == '/' && result[i+1] == '/' )
            memmove(result+i, result+i+1, strlen(result+i+1)+1);
    }
    // if path points to a folder, mark it as such
    if( result[strlen(result)-1] != '/' && is_folder(result) )
        strcat(result, "/");

    return result;
}

const char *cmppath(const char *a, const char *b) { // returns byte in `a` where strings differ. NULL if strings are equal.
    while( *a && *b ) {
        int eq = *a == *b;
        if(eq) { ++a; ++b; continue; }
        eq = strchr("\\/", *a) && strchr("\\/", *b);
        if(eq) {
            do ++a; while(*a && strchr("\\/", *a));
            do ++b; while(*b && strchr("\\/", *b));
            continue;
        }
        #ifdef _WIN32
        eq = toupper(*a) == toupper(*b);
        if(eq) { ++a; ++b; continue; }
        #endif
        return a;
    }
    while(*a && strchr("\\/", *a)) ++a;
    while(*b && strchr("\\/", *b)) ++b;
    return *a == *b ? NULL : a;
}

const char *abspath(const char *rel_path) { // convert relative path to absolute. do not free()
    if( !rel_path[0] ) return "";

    static char result[DIR_MAX]; result[0] = '\0';
    realpath(rel_path, result);
    return normpath(result);
}

const char *relpath(const char *abs_path, const char *cwd) { // convert absolute path to relative. do not free()
    if( !abs_path[0] ) return "";

    static char result[DIR_MAX]; result[0] = '\0';

    // handle cases where paths are indentical or subfolders
    const char *mismatch = cmppath(abs_path, cwd);
    if( !mismatch ) {
        strcpy(result, "./");
    }
    else if( mismatch > abs_path ) {
        strcpy(result, "./");
        strcat(result, mismatch);
    }
    else {
        // find common prefix
        unsigned last_slash = 0;
        for( unsigned i = 0; cwd[i] && cwd[i] == abs_path[i]; ++i ) {
            if( strchr("/\\", abs_path[i]) ) last_slash = i;
        }
        int up_levels = 0;
        for( unsigned i = last_slash; cwd[i]; i++ ) {
            up_levels += !!strchr("/\\", cwd[i]);
        }

        // if no common prefix (different drives on Windows), return absolute path
        if( last_slash == 0 ) {
            strcpy(result, abs_path); // @fixme: should do differently?
        }
        else {
            // build the relative path
            for( int i = 0; i < up_levels; i++ ) {
                strcat(result, "../");
            }

            strcat(result, abs_path + last_slash + 1);
        }
    }

    return normpath(result);
}
