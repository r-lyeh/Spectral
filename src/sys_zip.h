// zipfilepath` syntax: "c:/prj/my.zip/mydir/myfile"

bool zipme(const char *fullpath, const char *bin, int len) {
    const char *filename = strstr(fullpath, ".zip/"); filename += !!filename * 5;
    if( !filename ) return false;
    const char *zipfile = va("%.*s", (int)(filename - 1 - fullpath), fullpath);

    struct zip *z = zip_open(zipfile, "a+b");
    if( z ) {
        unsigned compress_level = 9;
        const char *entryname = filename;
        const char *comment = "";
        bool ok = zip_append_mem(z, entryname, comment, bin, len, compress_level);
        zip_close(z);
        return ok;
    }

    return false;
}

char *unzip_ex(const char *fullpath, int *len, const char **url) { // must free() rc after use. do not free url.
    if( url ) *url = 0;
    if( len ) *len = 0;
    const char *filename = strstr(fullpath, ".zip/"); filename += !!filename * 5;
    if( !filename ) return NULL;
    const char *zipfile = va("%.*s", (int)(filename - 1 - fullpath), fullpath);

    char *bin = 0;
    struct zip *z = zip_open(zipfile, "rb");
    if( z ) {
        int index = -1;
        if( strchr(filename, '*') ) {
            for( unsigned i = 0, end = zip_count(z); i < end; ++i ) {
                if( strmatchi(zip_name(z,i), filename) ) {
                    index = i;
                }
            }
        }
        else if( strchr(filename, '@') ) {
            unsigned i = atoi(strchr(filename, '@')+1);
            if( i < zip_count(z) ) {
                index = i;
            }
        }
        else {
            index = zip_find(z, filename);
        }
        if( index >= 0 ) {
            bin = zip_extract(z, index);
            if(len) *len = zip_size(z, index);
            if(url) *url = va("%s", zip_name(z, index));
        }
        zip_close(z);
    }

    return bin;
}

char *unzip(const char *fullpath, int *len) { // must free() after use
    return unzip_ex(fullpath, len, NULL);
}


char *unrar_ex(const char *fullpath, int *len, const char **url) { // must free() rc after use. do not free url.
    if( url ) *url = 0;
    if( len ) *len = 0;
    const char *filename = strstr(fullpath, ".rar/"); filename += !!filename * 5;
    if( !filename ) return NULL;
    const char *rarfile = va("%.*s", (int)(filename - 1 - fullpath), fullpath);

    char *bin = 0;
    rar *z = rar_open(rarfile, "rb");
    if( z ) {
        int index = -1;
        if( strchr(filename, '*') ) {
            for( unsigned i = 0, end = rar_count(z); i < end; ++i ) {
                if( strmatchi(rar_name(z,i), filename) ) {
                    index = i;
                }
            }
        }
        else if( strchr(filename, '@') ) {
            unsigned i = atoi(strchr(filename, '@')+1);
            if( i < rar_count(z) ) {
                index = i;
            }
        }
        else {
            index = rar_find(z, filename);
        }
        if( index >= 0 ) {
            bin = rar_extract(z, index);
            if(len) *len = rar_size(z, index);
            if(url) *url = va("%s", rar_name(z, index));
        }
        rar_close(z);
    }

    return bin;
}

char *unrar(const char *fullpath, int *len) { // must free() after use
    return unrar_ex(fullpath, len, NULL);
}

#if 0
char *unrar_mem(const void *inbin, unsigned inlen, unsigned *outlen) { // must rar_free()
    if( inbin && inlen > 4 && !memcmp(inbin, "Rar!", 4) ) {
        rar *r = rar_openmem(inbin, inlen);
        if( r ) {
            char* out = rar_extract(r, 0);
            if(out && outlen) *outlen = rar_size(r, 0);
            rar_close(r);
            return out;
        }
    }
    return NULL;
}

char *unzip_mem(const void *inbin, unsigned inlen, unsigned *outlen) { // must zip_free()
    if( inbin && inlen > 4 && (!memcmp(inbin, "PK\3\4", 4) || !memcmp(inbin, "PK00", 4)) ) {
        zip *r = zip_openmem(inbin, inlen);
        if( r ) {
            char* out = zip_extract(r, 0);
            if(out && outlen) *outlen = zip_size(r, 0);
            zip_close(r);
            return out;
        }
    }
    return NULL;
}
#endif
