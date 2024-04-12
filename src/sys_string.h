#include <stdio.h>
#include <stdarg.h>
char* va(const char *fmt, ...) {
    static char buf[16][256];
    static int l = 0; l = (l+1) % 16;
    va_list vl;
    va_start(vl,fmt);
    int rc = vsnprintf(buf[l], 256-1, fmt, vl);
    va_end(vl);
    buf[l][rc<0?0:rc] = 0;
    return buf[l];
}
#include <string.h>
char *replace( char *copy, const char *target, const char *replacement ) {
    // replaced only if new text is shorter than old one
    int rlen = strlen(replacement), diff = strlen(target) - rlen;
    if( diff < 0 ) return 0;
    for( char *s = copy, *e = s + strlen(copy); /*s < e &&*/ 0 != (s = strstr(s, target)); ) {
        if( rlen ) s = (char*)memcpy( s, replacement, rlen ) + rlen;
        if( diff ) memmove( s, s + diff, (e - (s + diff)) + 1 );
    }
    return copy;
}
char *strstri(const char *a, const char *b) {
    for(char *p = (char*)(a = va("%s",a)); *p; ++p) *p = toupper(*p);
    for(char *p = (char*)(b = va("%s",b)); *p; ++p) *p = toupper(*p);
    return strstr(a, b);
}
bool strendi(const char *src, const char *sub) { // returns true if both strings match at end. case insensitive
    int srclen = strlen(src);
    int sublen = strlen(sub);
    if( sublen > srclen ) return 0;
    return !strcmpi(src + srclen - sublen, sub);
}
int qsort_strcmp(const void * a, const void * b ) {
    // smart strcmp which does:
    // Narc - 128k.tzx < Narc - 128k - Levels.tzx, and also:
    // Academy - Side 1.tzx < Academy - Side 2.tzx < Academy - Side 3.tzx < Academy - Side 4.tzx, and also:
    // Outrun - Tape 1 - Side 1.tzx < Outrun - Tape 2 - Side 1.tzx < Outrun - Tape 2 - Side 2.tzx
    const char *pa = *(const char**)a;
    const char *pb = *(const char**)b;
    int la = strlen(pa), lb = strlen(pb);
    if( la < lb ) return -1;
    if( la > lb ) return +1;
    return strcmp(pa,pb);
}

// find a mem blob in a mem section; similar to strstr()
const void *memmem(const void *stack, size_t stack_len, const void * const blob, const size_t blob_len) {
    if((uintptr_t)stack * stack_len * (uintptr_t)blob * blob_len)
    for (const char *h = stack; stack_len >= blob_len; ++h, --stack_len) {
        if (!memcmp(h, blob, blob_len)) {
            return h;
        }
    }
    return NULL;
}

// memset words instead of chars
void *memset32(void *dst, unsigned ch, unsigned bytes) {
    unsigned ch4 = (ch >> 24) & 0xff;
    unsigned ch3 = (ch >> 16) & 0xff;
    unsigned ch2 = (ch >>  8) & 0xff;
    unsigned ch1 = (ch >>  0) & 0xff;
    char *ptr = (char*)dst;
    while( bytes ) {
        if( bytes-- ) *ptr++ = ch4;
        if( bytes-- ) *ptr++ = ch3;
        if( bytes-- ) *ptr++ = ch2;
        if( bytes-- ) *ptr++ = ch1;
    }
    return dst;
}
