// io

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>


#ifdef _WIN32
#define realpath(inpath,outpath) _fullpath(outpath, inpath, MAX_PATH - 1)
#endif

#define DIR_SEP '/'
#define DIR_MAX MAX_PATH

#include <stdio.h>
unsigned char *readfile(const char *pathfile, int *size) {
    char *bin = 0;
    for( FILE *fp = fopen(pathfile,"rb"); fp; fclose(fp), fp = 0) {
        fseek(fp, 0L, SEEK_END);
        size_t len = ftell(fp);
        if(size) *size = (int)len;
        fseek(fp, 0L, SEEK_SET);
        bin = malloc(len+1);
        if( bin && fread(bin, 1, len, fp) == len ) bin[len] = '\0';
        else free(bin), bin = 0;
    }
    return bin;
}
int writefile(const char *pathfile, const void *blob, int len) {
    int ok = 0;
    FILE *fp = fopen(pathfile, "wb");
    if( fp ) {
        ok = fwrite(blob, len, 1, fp) == 1;
        fclose(fp);
    }
    return ok;
}


void db_set(const char *key, int value) {
    char fname[1024+1];
    snprintf(fname, 1024, "%s.db", key);
    for(FILE *fp = fopen(fname, "wb"); fp; fclose(fp), fp=0) {
        fprintf(fp, "%d", value);
    }
}
int db_get(const char *key) {
    char fname[1024+1];
    snprintf(fname, 1024, "%s.db", key);
    int value = 0;
    for(FILE *fp = fopen(fname, "rb"); fp; fclose(fp), fp=0) {
        fscanf(fp, "%d", &value);
    }
//  fprintf(stderr, "%s=%d\n", key, value);
    return value;
}

void cwdexe(void) {
#ifdef __APPLE__
    char buffer[MAX_PATH]={0};
    realpath(__argv[0],buffer);
    puts(buffer);
    if(strrchr(buffer,'/')) 1[strrchr(buffer,'/')] = '\0';
    puts(buffer);
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
