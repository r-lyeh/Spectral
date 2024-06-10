// header

typedef struct zxdb {
    char *copy, *tok;
    char *ids[8];        // "#id|year|title|alias|publisher|max_players|score|genre"
    char *authors[16];   // "@role author(team)"
    char *downloads[64]; // "/file_link|release_seq|filetype_id|filetype"
} zxdb;

void zxdb_init(const char *dbfile);
zxdb  zxdb_search(const char *entry); // either "#id", "*text*search*", or "/file.ext"
zxdb   zxdb_print(const zxdb);
char*  zxdb_url(const zxdb, const char *hint); // @todo:, unsigned release_seq);
zxdb   zxdb_free(zxdb);
char* zxdb_download(const char *url, int *len); // must free() after use

// impl
#include <string.h>

struct map zxdb2;
char *zxdb_alloc;

void zxdb_init(const char *dbfile) {
    // allocate db
    if(zxdb_alloc) return;
    int len;
    zxdb_alloc = readfile(dbfile, &len);

    // if not external file, must be embedded. skip binary and jump to content
    int skip = 0;
    if( zxdb_alloc && !strstr(dbfile, ".db") ) {
        skip = (len - 1) - 4 - *(unsigned *)&zxdb_alloc[ len - 4 ];
        len = *(unsigned *)&zxdb_alloc[ len - 4 ];
        if( memcmp(zxdb_alloc + skip, "\x1f\x8b\x08", 3) ) return;
    }

    if( zxdb_alloc && !memcmp(zxdb_alloc + skip, "\x1f\x8b\x08",3) ) {
        char *unc = gunzip(zxdb_alloc + skip, len, 0);
        if( !unc ) { alert("cannot uncompress .db file"); return; }
        free(zxdb_alloc);
        zxdb_alloc = unc;
    }

    // parse db. insert every entry
    if( zxdb_alloc )
    for( char *ptr = zxdb_alloc; *ptr; ) {
        char *entry = ptr;

        char *lf;
        for( ;; ) {
            lf = strchr(ptr, '\n');
            if(!lf) { ptr += strlen(ptr); break; } // go to eof
            ptr = ++lf;
            if(*ptr != '@' && *ptr != '/') break; // break at new entry
        }

        // inscribe entry, which goes from [entry,ptr)
        ptr[-1] = '\0';
        char *year  = strchr(entry, '|')+1; assert(year);
        char *title = strchr(year,  '|')+1; assert(title);
        char *alias = strchr(title, '|')+1; assert(alias); title = va("%.*s", alias-title-1, title);
        char *brand = strchr(alias, '|')+1; assert(brand); alias = va("%.*s", brand-alias-1, alias);

        map_insert(&zxdb2, title, entry);
        if( alias[0] ) map_insert(&zxdb2, alias, entry);
    }

    printf("%d ZXDB entries\n", map_count(&zxdb2));
}

static
void zxdb_add_ids(zxdb *z, char **ids) {
    for( int i = 0, j = 0; ids[i]; ++i) {
        j = i;
        if( j >= countof(z->ids) ) continue;
        z->ids[j++] = ids[i];
    }
}

static
void zxdb_add_authors(zxdb *z, char **authors) {
    for( int i = 0, j = 0; authors[i]; ++i) {
        if( j >= countof(z->authors) ) continue;
        z->authors[j++] = authors[i];
    }
}

static
void zxdb_add_downloads(zxdb *z, char **downloads) {
    for( int i = 0, j = 0; downloads[i]; ++i) {
        if( j >= countof(z->downloads) ) continue;

        // .headers on; select * from filetypes ORDER BY id;
        const char accepted_types[] = {
            1,  // Loading screen
            2,  // Running screen
            4,  // Inlay - Side
            5,  // Inlay - Front
            6,  // Inlay - Back
            7,  // Media scan
            8,  // Tape image
            10, // Snapshot image
            11, // Disk image
            17, // Computer/ZX Interface 2 cartridge ROM image dump
            18, // DOCK cartridge ROM image dump
            22, // BUGFIX tape image
            27, // Bonus soundtrack(s) in MP3 format
            28, // Instructions
            29, // Scanned instructions
            30, // Keyboard overlay
            31, // Game map
            33, // Code sheet
            59, // Comic
            63, // RZX playback file
            67, // ZX Interface 2 cartridge box scan
            74, // POK pokes file
            0,
        };

        // exclude types we are not likely going to ever use.
        char download_type = atoi(strchr(strchr(downloads[i],'|')+1,'|')+1);
        if( !strchr(accepted_types, download_type) ) continue;

        // exclude .pdf instructions. i dont really want to support this right now
        char *ext = strrchr(downloads[i], '.');
        if( ext && !memcmp(ext, ".pdf", 4) ) continue;

        // add download
        z->downloads[j++] = (downloads[i] + 6 * !strncmp(downloads[i], "http:/", 6) + 7 * !strncmp(downloads[i], "https:/", 7) );
    }
}

static
zxdb zxdb_search_by_name(const char *name) {
    zxdb z = {0};

    // search query
    VAL *found = map_find(&zxdb2, name);
    if( found ) {
        z.copy = strdup(*found);
        z.tok = strdup(*found);

        char *lines[128] = {0}, **line = lines, *sep;

        // split tok into lines. ignore entry strings (\r\n)
        sep = z.tok;
        for( char *ptr = strsep(&sep, "\r\n" ); ptr; ptr = strsep(&sep, "\r\n") ) {
            if( ptr[0] ) *line++ = ptr;
        }

        // inscribe download lines [2..]
        zxdb_add_downloads(&z, &lines[1 + (lines[1][0] == '@')]); // pick line [1] or [2] depending on line[1] being an @authors line

        // inscribe authors' line [1]. ignore entry strings (\r\n)
        if( lines[1][0] == '@' ) {
            char **authors = line;
            sep = lines[1];
            for( char *ptr = strsep(&sep, "@\r\n" ); ptr; ptr = strsep(&sep, "@\r\n" ) ) {
                if( ptr[0] ) *line++ = ptr;
            }
            zxdb_add_authors(&z, authors);
        }

        // inscribe main line [0]. include empty strings (1983||Manic|)
        {
            char **ids = line;
            sep = lines[0];
            for( char *ptr = strsep(&sep, "|\r\n" ); ptr; ptr = strsep(&sep, "|\r\n" ) ) {
                *line++ = ptr;
            }
            zxdb_add_ids(&z, ids);
        }
    }

    return z;
}

zxdb zxdb_print(const zxdb z) {
    z.copy && puts(z.copy);
    return z;
}

zxdb zxdb_free(zxdb z) {
    zxdb zero = {0};
    free(z.copy);
    free(z.tok);
    return zero;
}

static char *zxdb_filename2title(const char *filename) {
    // find basename
    const char *win = strrchr(filename, '\\');
    const char *unx = strrchr(filename, '/');

    // extract basename
    char *s = va("%s", win > unx ? win + 1 : unx > win ? unx + 1 : filename);

    // remove extensions
    // @todo: filter DB search by extension. ie, search exclusively for +3 disks if .dsk extension is found
    char *ext;
    do {
        ext = strlen(s) > 4 ? s + strlen(s) - 4 : 0;
        /**/ if( ext && ext[0] == '.' && !strchr(ext+1, '.') ) ext[0] = '\0'; // .rar .zip .tap .tzx etc
        else if( ext && ext[1] == '.' && !strchr(ext+1, '.') ) ext[1] = '\0'; // .gz
        else ext = 0;
    } while(ext);

    // extract (year)(publisher)
    if( strchr(s, '(') ) *strchr(s, '(') = '\0';

    // convert case edges into spaces (@fixme: utf8 games; russian? spanish? czech?)
    // there x2 room for worst case: aBcDeF>a*B*c*D*e*F, and also extra room for further string patches
    char *spaced = va("%*.s", strlen(s)*2+5, "");
    for( int i = 0, j = 0; s[i]; ++i ) {
        if( i > 1 ) {
            int upper = islower(s[i-1]) && isupper(s[i]);
            int digit = isalpha(s[i-1]) && isdigit(s[i]);
            if( upper || digit ) spaced[j++] = '*';
        }
        spaced[j++] = s[i];
    }
    s = spaced;

    // trim lead/final spaces and double spaces
    while( s[0] == ' ' ) ++s;
    while( s[strlen(s)-1] == ' ' ) s[strlen(s)-1] = '\0';
    while( strstr(s, "  ") ) replace(s, "  ", " ");

    // convert to uppercase (@fixme: utf8 games; russian? spanish? czech?)
    for( int i = 0; s[i]; ++i ) s[i] = toupper(s[i]);

    // reorder prefixes written as suffixes
    /**/ if( strendi(s, ", THE") ) memmove(s+4,s,strlen(s+4)), memcpy(s, "THE ", 4), s[strlen(s)-1] = '\0';
    else if( strendi(s, ", LOS") ) memmove(s+4,s,strlen(s+4)), memcpy(s, "LOS ", 4), s[strlen(s)-1] = '\0';
    else if( strendi(s, ", LAS") ) memmove(s+4,s,strlen(s+4)), memcpy(s, "LAS ", 4), s[strlen(s)-1] = '\0';
    else if( strendi(s, ", EL")  ) memmove(s+3,s,strlen(s+3)), memcpy(s, "EL ", 3), s[strlen(s)-1] = '\0';
    else if( strendi(s, ", LA")  ) memmove(s+3,s,strlen(s+3)), memcpy(s, "LA ", 3), s[strlen(s)-1] = '\0';
    else if( strendi(s, ", IL")  ) memmove(s+3,s,strlen(s+3)), memcpy(s, "IL ", 3), s[strlen(s)-1] = '\0';
    else if( strendi(s, ", A")   ) memmove(s+2,s,strlen(s+2)), memcpy(s, "A ", 2), s[strlen(s)-1] = '\0';

    // convert non-alpha into spaces (@fixme: utf8 games; russian? spanish? czech?)
    for( int i = 0; s[i]; ++i ) s[i] = isalnum(s[i]) ? s[i] : '*';

    // final touch, remove 48 and 128 from filenames: renegade128.tzx, rasputin48.sna, etc
    if( strstr(s, "*48K") ) memcpy(strstr(s, "*48K"), "****",  4);
    if( strstr(s,"*128K") ) memcpy(strstr(s,"*128K"), "*****", 5);
    if( strstr(s,  "*48") ) memcpy(strstr(s,  "*48"), "***",   3);
    if( strstr(s, "*128") ) memcpy(strstr(s, "*128"), "****",  4);

    return s;
}

zxdb zxdb_search(const char *filename) {
    char *s = zxdb_filename2title(filename);

    // search query
    zxdb z = zxdb_search_by_name( s );

    // if it fails, try again replacing trailing 2>"% II", 3>"% III", 4>"% IV", 5>"% V"
    // lines ago, we did pre-allocate room space for this patch.
    if( !z.ids[0] && strendi(s, "*2") ) strcpy((char*)strendi(s,"*2"),"* II"), z = zxdb_search_by_name(s);
    if( !z.ids[0] && strendi(s, "*3") ) strcpy((char*)strendi(s,"*3"),"* III"), z = zxdb_search_by_name(s);
    if( !z.ids[0] && strendi(s, "*4") ) strcpy((char*)strendi(s,"*4"),"* IV"), z = zxdb_search_by_name(s);
    if( !z.ids[0] && strendi(s, "*5") ) strcpy((char*)strendi(s,"*5"),"* V"), z = zxdb_search_by_name(s);
    return z;
}

char* zxdb_url(const zxdb z, const char *hint) {
    /**/ if( strstri(hint, "runn") ) hint = "|0|2|";
    else if( strstri(hint, "screen") ) hint = "|0|1|";
    else if( strstri(hint, "inlay") && strstri(hint, "side")) hint = "|0|4|";
    else if( strstri(hint, "inlay") && strstri(hint, "back")) hint = "|0|6|";
    else if( strstri(hint, "inlay")) hint = "|0|5|";
//  else if( strstri(hint, "tape")) hint = "|0|22|";
    else if( strstri(hint, "tape")) hint = "|0|8|";
    else if( strstri(hint, "snap")) hint = "|0|10|";
    else if( strstri(hint, "disk")) hint = "|0|11|";
    else if( strstri(hint, "rom")) hint = "|0|17|";
    else if( strstri(hint, "scanned")) hint = "|0|29|";
    else if( strstri(hint, "instr")) hint = "|0|28|";
    else if( strstri(hint, "overlay")) hint = "|0|30|";
    else if( strstri(hint, "map")) hint = "|0|31|";
    else if( strstri(hint, "comic")) hint = "|0|59|";
    else if( strstri(hint, "rzx")) hint = "|0|63|";
    else if( strstri(hint, "pok")) hint = "|0|74|";

    for( int i = 0; z.downloads[i]; ++i ) {
        if( strstri(z.downloads[i], hint) ) {
            char *url = va("%.*s", strchr(z.downloads[i],'|') - z.downloads[i], z.downloads[i]);
            return url;
        }
    }

    return NULL;
}

char* zxdb_download(const char *url, int *len) {
    if( !url ) return 0;
    if( url[0] == '/' ) {
        const char *mirror = 0;
        /**/ if( !strncmp(url, "/nvg/", 5) ) {
            mirror = "https://archive.org/download/mirror-ftp-nvg/Mirror_ftp_nvg.zip/";
            url += 5;
        }
        else if( !strncmp(url, "/pub/", 5) ) {
            // ok, redirection
            mirror = "https://archive.org/download/World_of_Spectrum_June_2017_Mirror/World%20of%20Spectrum%20June%202017%20Mirror.zip/World%20of%20Spectrum%20June%202017%20Mirror/";
            url += 5;
        }
        else if( !strncmp(url, "/zxdb/", 6) ) {
            // ok
            mirror = "https://spectrumcomputing.co.uk/";
            url += 1;
        }
        else {
            // try https:// as a fallback in case it is a full url
            mirror = "https://";
            url += 1;
        }
        return mirror ? download(va("%s%s", mirror, url), len) : 0;
    }
    // puts(url);
    return download(url, len);
}
