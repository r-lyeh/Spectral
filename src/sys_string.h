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
