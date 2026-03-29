// @fixme: excessive va()

#include "res/scr/question_mark"

zxdb ZXDB2;

rgba* thumbnail_inplace(const byte *VRAM_, int len, unsigned downfactor, int ZXFlashFlag, rgba *texture, const rgba *palette) {
    int w = 256 / downfactor, h = 192 / downfactor;

    rgba *cpy = texture;
    if( len != 6912 && len != (6912+64) && len != (6144+768*4) && len != (6144+768*8) )
        return texture; // @fixme: ula+/.ifl/.mlt 12k(T),9k(Z)oomblox

    if( DEV && len == (6912+64) )
        alert(va("vram: %d bytes found", len));

    #define SCANLINE_(y) \
        ((((((y)%64) & 0x38) >> 3 | (((y)%64) & 0x07) << 3) + ((y)/64) * 64) << 5)

    for( int y = 0; y < 192; y += downfactor ) {
        // paper
        const byte *pixels=VRAM_+SCANLINE_(y);
        const byte *attribs=VRAM_+6144+((y&0xF8)<<2);
        rgba *bak = texture;

        for(int x = 0; x < 32; ++x ) {
            byte attr = *attribs;
            byte pixel = *pixels;

            // @fixme: make section branchless

            pixel ^= (attr & 0x80) && ZXFlashFlag ? 0xff : 0x00;

            byte bright = (attr & 0x40) >> 3; // 0 or 8
            byte fg = (attr & 0x07) | bright;
            byte bg = ((attr >> 3) & 0x07) | bright;

            if( downfactor == 1 ) {
            texture[0]=palette[pixel & 0x80 ? fg : bg];
            texture[1]=palette[pixel & 0x40 ? fg : bg];
            texture[2]=palette[pixel & 0x20 ? fg : bg];
            texture[3]=palette[pixel & 0x10 ? fg : bg];
            texture[4]=palette[pixel & 0x08 ? fg : bg];
            texture[5]=palette[pixel & 0x04 ? fg : bg];
            texture[6]=palette[pixel & 0x02 ? fg : bg];
            texture[7]=palette[pixel & 0x01 ? fg : bg];
            texture += 8;
            }
            else if( downfactor == 2 ) {
            texture[0]=palette[pixel & 0x80 ? fg : bg];
            texture[1]=palette[pixel & 0x20 ? fg : bg];
            texture[2]=palette[pixel & 0x08 ? fg : bg];
            texture[3]=palette[pixel & 0x02 ? fg : bg];
            texture += 4;
            }
            else if( downfactor == 4 ) {
            texture[0]=palette[pixel & 0x80 ? fg : bg];
            texture[1]=palette[pixel & 0x08 ? fg : bg];
            texture += 2;
            }
            else if( downfactor == 8 ) {
            texture[0]=palette[pixel & 0x80 ? fg : bg];
            texture += 1;
            }

            pixels++;
            attribs++;
        }

        texture = bak + w;
    }

    return cpy;
}

rgba* thumbnail(const byte *VRAM_, int len, unsigned downfactor, int ZXFlashFlag) {
    int w = 256 / downfactor, h = 192 / downfactor;

    const
    rgba *palette = ZXPalettes[ZX_PALETTE]; // use B/W palette for a good noir effect!
    rgba *texture = malloc( w * h * 4 );

    return thumbnail_inplace(VRAM_, len, downfactor, ZXFlashFlag, texture, palette);
}



static
const char *tab;

static
int zxdb_compare_by_name(const void *arg1, const void *arg2) { // @fixme: roman
    const char **a = (const char**)*(VAL**)arg1; const char *entry = *a;
    const char **b = (const char**)*(VAL**)arg2; const char *other = *b;

    const char *year1  = strchr(entry,  '|')+1;
    const char *title1 = strchr(year1,  '|')+1;
    const char *alias1 = strchr(title1, '|')+1;

    const char *year2  = strchr(other,  '|')+1;
    const char *title2 = strchr(year2,  '|')+1;
    const char *alias2 = strchr(title2, '|')+1;

    if( tab ) {
        if( *tab == '#' ) {
            if( *alias1 != '|' && !isdigit(*title1) && !ispunct(*title1) ) title1 = alias1;
            if( *alias2 != '|' && !isdigit(*title2) && !ispunct(*title2) ) title2 = alias2;
        } else {
            if( *title1 != *tab && *alias1 == *tab ) title1 = alias1;
            if( *title2 != *tab && *alias2 == *tab ) title2 = alias2;
        }
    }

    return strcmpi(title1, title2);
}

char *zxdb_screen(const char *id, int *len) {
    if( id && id[0] && strcmp(id, "0") && strcmp(id, "#") ) {
        ZXDB2 = zxdb_search( id, 0 );

        if( ZXDB2.ids[0] ) {
            static char *data = 0;
            if( data ) free(data), data = 0;
            if(!data ) data = zxdb_download(ZXDB2,zxdb_url(ZXDB2, "screen"), len);
            if(!data ) data = zxdb_download(ZXDB2,zxdb_url(ZXDB2, "running"), len);
            return data;
        }
    }
    return NULL;
}


// newer v2 function to grab a zxdb handle
zxdb* zxdb_handle2(const char *id_, const char **hint) { // do not zxdb_free()
    if( id_ && id_[0] && strcmp(id_, "0") && strcmp(id_, "#") ) {

        char *id = va("%s", id_);
        if(hint) *hint = "play";

        for( int gid, seq; sscanf(id, "#%d#%d", &gid, &seq ) == 2; ) {
            if(hint) *hint = strrchr(id,'#')+1;
            *strrchr(id,'#') = '\0';
            break;
        }

        static zxdb slots[16] = {0}; static int count = 0; count = (count + 1) % 16;
        if( slots[count].tok ) zxdb_free(slots[count]);
        slots[count] = zxdb_search(id, 0);

        return &slots[count];
    }
    return NULL;
}

// newer v2 function to retrieve game urls and specific releases
// supports both #4424 and #4424#10 formats
// @fixme: implement #id#play#1 or #id#mp3#2 in addition to #id#N
const char *zxdb_url2(const char *id_) {
    const char *hint;
    zxdb* handle = zxdb_handle2(id_, &hint);
    if( handle ) {
        char *url = zxdb_url(*handle, hint);
        if(url) url = va("%s", url);
        return url;
    }
    return NULL;
}

bool zxdb_unpack2(char **ptr, int *len) {
    repeat:; // avoid recursion

    // is it zip,rar or gzip? uncompress & try to recurse...
    if( !memcmp(*ptr, "\x1f\x8b\x08", 3) ) {
        unsigned unclen;
        char *unc = gunzip(*ptr, *len, &unclen);
        if( unc ) {
            free(*ptr);
            *ptr = unc;
            *len = unclen;
            goto repeat;
        }
        return 0;
    }
    if( !memcmp(*ptr, "Rar!", 4) ) {
        int unclen;
        char *ptr2 = unrar_mem(*ptr, *len, &unclen);
        if( ptr2 ) {
            free(*ptr);
            *ptr = ptr2;
            *len = unclen;
            goto repeat;
        }
        return 0;
    }
    if( !memcmp(*ptr, "PK\3\4", 4) || !memcmp(*ptr, "PK00PK\3\4", 8) ) { // @fixme: implement zip_openmem() in 3rd_zip.h and remove this local file
        for( FILE *fp = fopen(".Spectral/$$download2.zip", "wb"); fp; fwrite(*ptr, *len, 1, fp), fclose(fp), fp = 0) 
        {}
        char *ptr2 = unzip(".Spectral/$$download2.zip/*", len);
        unlink(".Spectral/$$download2.zip");
        if( ptr2 ) {
            free(*ptr);
            *ptr = ptr2;
            goto repeat;
        }
        return 0;
    }
    return 1;
}

// newer v2 function to download games and specific releases. also unzips.
// supports both #4424 and #4424#10 formats. outputs url as well if needed.
char *zxdb_download2(const char *id_, int *len) { // must free()
    char *data = 0;
    for( const char *url = zxdb_url2(id_); url; url = 0 ) {
        for( zxdb *handle = zxdb_handle2( id_, 0 ); handle; handle = 0 ) {
            data = zxdb_download(*handle,url,len);
        }
    }
    return data;
}







#pragma pack(push, 1)
typedef struct cache_t {
    uint16_t likes : 4;
    uint16_t flags : 4;
    uint16_t reserved : 8;
} cache_t;
#pragma pack(pop)

typedef int static_assert_cache_t[sizeof(cache_t) == 2 ? 1:-1];

enum {
    CACHE_TRANSFER = 2, // if download is in progress

    CACHE_MP3 = 1,
    CACHE_TXT = 2,
    CACHE_POK = 4,
    CACHE_SCR = 8,
    CACHE_SNA = 16,
    CACHE_JPG = 32,
    CACHE_MAP = 64,
};

uint16_t *cache;

void cache_load() {
    if(!cache) {
        cache = calloc(2, 65536); // zxdb_count());
        for( FILE *fp = fopen(".Spectral/Spectral.fav", "rb"); fp; fclose(fp), fp = 0) {
            fread(cache, 2 * 65536, 1, fp);
        }
    }
}
void cache_save() {
    for( FILE *fp = fopen(".Spectral/Spectral.fav", "wb"); fp; fclose(fp), fp = 0) {
        fwrite(cache, 2 * 65536, 1, fp);
    }
}
uint16_t cache_get(unsigned zxdb) {
    cache_load();
    return cache[zxdb];
}
uint16_t cache_set(unsigned zxdb, uint16_t v) {
    cache_load();
    int changed = cache[zxdb] ^ v;
    cache[zxdb] = v;
    if( changed ) cache_save();
    return v;
}

// screens + thumbnails
// 64K ZXDB entries max, x2 flash versions (on/off) each, x4 versions each (1:1,2:1,4:1,8:1 shrinks)
const byte* screens[65536][2][4];
unsigned short screens_len[65536];


thread_ptr_t worker, worker2;
thread_queue_t queue, queue2;
struct queue_t {
    zxdb z;
    char *url;
};
void* queue_values[12*12]; // 12x12 thumbnails max
void* queue_values2[12*12]; // 12x12 thumbnails max
struct queue_t *queue_t_new(zxdb z,const char *url) {
    struct queue_t *q = malloc(sizeof(struct queue_t));
    q->z = zxdb_dup(z);
    q->url = url ? strdup(url) : NULL;
    return q;
}
double worker_progress() {
    unsigned capacity = sizeof(queue_values) / sizeof(queue_values[0]);
    unsigned count = worker ? thread_queue_count(&queue) : 0;
    return 1 - (count / (double)capacity);
}
int worker_fn( void* userdata ) {
    for(;;) {
        void* item = thread_queue_consume((thread_queue_t*)userdata, THREAD_QUEUE_WAIT_INFINITE);
        struct queue_t *q = (struct queue_t*)item;
        printf("queue recv %s\n", q->url);

        int id = atoi(q->z.ids[0]), len = 0;
        if( screens_len[id] == 1 ) {
            char *bin = zxdb_download(q->z, q->url, &len);
            if( !bin || !len ) {
                // black screen
                // bin = calloc(1,len = 6912);
                bin = (char*)question_mark;
                len = question_mark_length;
            }

            {
                screens[id][0][0] =
                screens[id][0][1] =
                screens[id][0][2] =
                screens[id][0][3] =
                screens[id][1][0] =
                screens[id][1][1] =
                screens[id][1][2] =
                screens[id][1][3] = (const byte*)bin;

                int ix,iy,in;
                rgba *bitmap = (rgba*)stbi_load_from_memory(bin, len, &ix, &iy, &in, 4);
                if( bitmap ) {
                    screens[id][0][1] = screens[id][1][1] = (const byte*)ui_resize(bitmap, ix, iy, 256/2, 192/2, 1, 0);
                    screens[id][0][2] = screens[id][1][2] = (const byte*)ui_resize(bitmap, ix, iy, 256/4, 192/4, 0, 0);
                    screens[id][0][3] = screens[id][1][3] = (const byte*)ui_resize(bitmap, ix, iy, 256/8, 192/8, 0, 0);
                    stbi_image_free(bitmap);
                } else {
                    bitmap = thumbnail(bin, len, 1, 0); ix = 256, iy = 192;
                    screens[id][0][1] = screens[id][1][1] = (const byte*)ui_resize(bitmap, ix, iy, 256/2, 192/2, 1, 0);
                    screens[id][0][2] = screens[id][1][2] = (const byte*)ui_resize(bitmap, ix, iy, 256/4, 192/4, 0, 0);
                    screens[id][0][3] = screens[id][1][3] = (const byte*)ui_resize(bitmap, ix, iy, 256/8, 192/8, 0, 0);
                    free(bitmap);

                    if( len >= 768 )
                    for( int i = 0; i < 768; ++i) {
                        int has_flash = ((byte*)bin)[i] & 0x80;
                        if( has_flash ) {
                            bitmap = thumbnail(bin, len, 1, 1); ix = 256, iy = 192;
                            screens[id][1][1] = (const byte*)ui_resize(bitmap, ix, iy, 256/2, 192/2, 1, 0);
                            screens[id][1][2] = (const byte*)ui_resize(bitmap, ix, iy, 256/4, 192/4, 0, 0);
                            screens[id][1][3] = (const byte*)ui_resize(bitmap, ix, iy, 256/8, 192/8, 0, 0);
                            free(bitmap);
                            break;
                        }
                    }
                }

                screens_len[id] = len;
            }
        }

        zxdb_free(q->z);
        free(q->url);
        free(q);
    }
    return 0;
}
int worker_push(const zxdb z, const char *url, int ms) {
    unsigned hash = url ? fnv1a(url, strlen(url)) : (unsigned)(uintptr_t)url;
    unsigned bucket = hash & 1;

    if( hash & 1 ) {
        // init
        int capacity = sizeof(queue_values) / sizeof(queue_values[0]);
        if(!worker)  thread_queue_init(&queue, capacity, queue_values, 0);
        if(!worker)  thread_detach( worker = thread_init(worker_fn, &queue, "worker_fn", THREAD_STACK_SIZE_DEFAULT) );

        if( thread_queue_count(&queue) < capacity )
            if( thread_queue_produce(&queue, queue_t_new(z,url), ms ) ) // THREAD_QUEUE_WAIT_INFINITE );
                return printf("queue send1 %s\n", url), 1;
    } else {
        // init2
        int capacity2 = sizeof(queue_values2) / sizeof(queue_values2[0]);
        if(!worker2) thread_queue_init(&queue2, capacity2, queue_values2, 0);
        if(!worker2) thread_detach( worker2 = thread_init(worker_fn, &queue2, "worker_fn2", THREAD_STACK_SIZE_DEFAULT) );

        if( thread_queue_count(&queue2) < capacity2 )
            if( thread_queue_produce(&queue2, queue_t_new(z,url), ms ) ) // THREAD_QUEUE_WAIT_INFINITE );
                return printf("queue send2 %s\n", url), 1;
    }
    return 0;
}

const
char *zxdb_screen_async(const char *id, int *len, int factor) {
    if( id && id[0] && strcmp(id, "0") && strcmp(id, "#") ) {
        ZXDB2 = zxdb_search( id, 0 );

        if( ZXDB2.ids[0] ) {
            int zxdb_id = atoi(ZXDB2.ids[0]);
            if( screens_len[zxdb_id] == 0 ) {
                char *url = 0;
                if(!url ) url = zxdb_url(ZXDB2, "screen");
                if(!url ) url = zxdb_url(ZXDB2, "running");
                if( worker_push(ZXDB2, url, 1) )
                    screens_len[zxdb_id] = 1;
            }
            if( screens_len[zxdb_id] > 1 ) {
                if( len ) *len = screens_len[zxdb_id];
                return screens[zxdb_id][ZXFlashFlag][factor & 3];
            }
        }
    }
    return NULL;
}







char*filter; // current text filter being typed. can be NULL
char*filters[16] = {0}; // all filters in use
int  filters_event; // notifies both viewers (v1/v2) that filters have changed (1|2). |4 is used to notify that we want to initiate a filter input text programatically 
void filters_add(const char *filter) {
    if( filter && filter[0] ) {
        if( !strcmp(filter, "?") ) return; // this is a useless filter often clicked in Labyrinth(?)() expressions

        int found = 0;
        for( int i = 0; i < countof(filters); ++i )
            if( filters[i] && !strcmp(filters[i], filter) ) found = i+1;
        
        if( !found )
        for( int i = 0; i < countof(filters); ++i )
            if( !filters[i] ) { filters[i] = strdup(filter); filters_event = 1|2; break; }
    }
}
void filters_draw(Tigr *ui, int px, int py) {
    // display current filters, with outlines
    ui_at(ui, px, py);
    for( int i = 0, end = countof(filters); i < end; ++i) {
        const char *text = filters[i];
        if( text && text[0] ) ; else continue;

        char label[256] = {0}, *ptr = label;
        while( *text ) {
            const char *num = "𜳰𜳱𜳲𜳳𜳴𜳵𜳶𜳷𜳸𜳹";
            const char *abc = "𜳖𜳗𜳘𜳙𜳚𜳛𜳜𜳝𜳞𜳟𜳠𜳡𜳢𜳣𜳤𜳥𜳦𜳧𜳨𜳩𜳪𜳫𜳬𜳭𜳮𜳯";
            if( isdigit(*text) ) ptr += sprintf(ptr, "%.*s", 4, num + 4 * (*text - '0'));
            else
            if( isalpha(*text) ) ptr += sprintf(ptr, "%.*s", 4, abc + 4 * (toupper(*text) - 'A'));
            else
            ptr += sprintf(ptr, "%c", *text);
            text++;
        }

        if( ui_click("-Click to remove filter-", va("\f\f%s\f\f%s,\f\f", CLOSE_STR, label)) ) {
            free( filters[i] );
            for( int j = i; j < end; ++j ) {
                filters[j] = filters[j+1];
            }
            filters[--end] = 0;
            --i;

            filters_event = 1|2;
        }
    }
}
int filters_tick() { // returns true if any key was pressed
    enum { _16 = 32, _15 = _16 - 1 };
    static int chars[_16] = {0};
    static char utf8[5+_16*6+1] = "Find:";
    filter = utf8 + 5;

        int chars_count = 0;
        while(chars[chars_count] && chars_count < _16) chars_count++;

    int anykey = 0, done = 0, clear = 0;
    if( filters_event & 4 ) anykey = 1, filters_event &= ~4;

    // Grab any chars and add them to our buffer.
    for(int idx = 0;;++idx) {
        int c = key_char(idx);
        if (c == 0) break;
        if( key_pressed(TK_CONTROL)) break;
        if( c == '\n' || c == '\r' ) { done = 1; break; }
        if( key_pressed(TK_ESCAPE) ) { done = 1; clear = 1; break; } // memset(chars, 0, sizeof(int)*_16); chars_count = -1; break; }
        if( c == '\b' && chars_count == 0 ) { done = 1; clear = 1; break; }
        if( c == '\t' && chars_count > 0 ) { anykey = 1; break; } // ?
        if( c <  32 && c != '\b' ) continue;
        if( c == 32 && chars_count == 0 ) continue;
        if( c == 32 && chars_count >  0 && chars[chars_count-1] == 32 ) continue;
        
        anykey = 1;

        if( c != '\b' )
            chars[ (chars_count = min(chars_count+1,_15)) - 1 ] = c;
        else
        if( chars_count > 0 )
            chars[ --chars_count ] = 0;

        char *p = filter;
        for (int n=0;n<_16;n++)
            p = tigrEncodeUTF8(p, chars[n]);
        *p = 0;
    }

    // display

    int visible = num_options == 1 && (options[0].flags & 2) && options[0].text[0] == 'F';

    if( done ) {
//        filters_event = 1|2;

        memset(chars, 0, sizeof(int)*_16);

        if( clear ) *filter = '\0';

        ui_dialog_new(NULL);

        // append typed filter to list
        if( filter && filter[0] /*&& key_down(TK_RETURN)*/ ) {
            filters_event = 1|2;

            // convert special words into singular
            if( !strcmpi(filter, "demos") ) filter[4] = 0;
            //if( !strcmpi(filter, "tests") ) filter[4] = 0;
            if( !strcmpi(filter, "texts") ) filter[4] = 0;

            filters_add(filter);

            *filter = 0;
        }
    }
    else {
        if( !visible && anykey ) {
            ui_dialog_new(NULL);
            ui_dialog_option(2, va("<%s▁                             \n",utf8),NULL, 0,NULL);
        }
        if( visible && key_longpress( TK_BACKSPACE) ) {
            memset(chars, chars_count = 0, sizeof(int)*_16); *filter = 0; anykey = 1;
        }
        if( visible && anykey ) {
            (void)REALLOC((void*)options[0].text, 0);
            options[0].text = STRDUP(va("%s▁\n",utf8));
        }
    }

    return anykey;
}





bool browser;

extern Tigr *ui;

char **games;
int *dbgames;
int numgames;
int numok,numwarn,numerr; // stats
char *last_scanned_folder;
void rescan(const char *folder) {
    if(!folder) return;
    if(ZX_PLAYER) return; // zxplayer has no library

    char *copy = va("%s", folder); folder = copy;
    if( ZX_FOLDER ) free(ZX_FOLDER); ZX_FOLDER = STRDUP(folder);

    printf("scanning `%s` folder...\n", folder);

    // convert to absolute
    char buffer[MAX_PATH]={0};
    realpath(folder, buffer);
    folder = buffer;

    char parent[MAX_PATH]={0};
    snprintf(parent, MAX_PATH, "%s/../", folder);

    // clean up
    while( numgames ) free(games[--numgames]);
    games = realloc(games, 0);

    // refresh stats
    {
        numok=0,numwarn=0,numerr=0;

        // add parent folder `..`
        ++numgames;
        games = realloc(games, numgames * sizeof(char*) );
        games[numgames-1] = strdup(parent);
        dbgames = realloc(dbgames, numgames * sizeof(char*) );
        dbgames[numgames-1] = 0;

        for( dir *d = dir_open(folder, NULL); d; dir_close(d), d = NULL ) {
            for( unsigned is_file = 0; is_file < 2; ++is_file )
            for( unsigned count = 0, end = dir_count(d); count < end; ++count ) {
                if( is_file ^ dir_file(d,count) ) continue;

                const char *fname = dir_name(d, count);
                if( is_file ? file_is_supported(fname,ALL_FILES) : 1 ) {

                    // filtering
                    if( is_file ) {
                        // filtering
                        char wildcard[128] = {0};
                        int matches = 0, numfilters = 0;
                        for( int f = 0; f < countof(filters) && filters[f]; ++f, ++numfilters ) {
                            if( snprintf(wildcard, 128, "*%s*", filters[f]) )
                                if( strmatchi(basename(fname)/*title*/, wildcard) ) ++matches;
                        }
                        if( numfilters != matches ) { continue; }
                    }

                    // append
                    ++numgames;
                    games = realloc(games, numgames * sizeof(char*) );
                    games[numgames-1] = strdup(fname);
                    //
                    dbgames = realloc(dbgames, numgames * sizeof(char*) );
                    dbgames[numgames-1] = is_file ? db_get(fname) : 0;
                }
                if( is_file && strendi(fname, ".db") ) {
                    for(FILE *fp2 = fopen(fname, "rb"); fp2; fclose(fp2), fp2=0) {
                        int ch;
                        fscanf(fp2, "%d", &ch); ch &= 0xFF;
                        numok += ch == 1;
                        numerr += ch == 2;
                        numwarn += ch == 3;
                    }
                }
            }
        }
    }

    printf("%d games\n", numgames);

    if(last_scanned_folder) free(last_scanned_folder), last_scanned_folder = 0;
    last_scanned_folder = strdup(folder);
}
void draw_compatibility_stats(window *layer) {
    // compatibility stats
    int total = numok+numwarn+numerr;
    if(total && browser) {
    TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + _239 * _320];
    int num1 = (numok * (float)_319) / total;
    int num2 = (numwarn * (float)_319) / total;
    int num3 = (numerr * (float)_319) / total; if((num1+num2+num3)<_319) num1 += _319 - (num1+num2+num3);
    for( int x = 0; x <= num1; ++x ) bar[x-320]=bar[x] = tigrRGB(64,255,64);
    for( int x = 0; x <= num2; ++x ) bar[x+num1-320]=bar[x+num1] = tigrRGB(255,192,64);
    for( int x = 0; x <= num3; ++x ) bar[x+num1+num2-320]=bar[x+num1+num2] = tigrRGB(255,64,64);
    static char compat[64];
    snprintf(compat, 64, "  OK:%04.1f%%     ENTER:128, +SHIFT:48, +CTRL:Try turbo", (total-numerr) * 100.f / (total+!total));
    ui_print(layer, 0,(_240-12.0)/11, ui_colors, compat);
    }
}







int selected, scroll;

int game_browser_keyboard(const int ENTRIES, const int numgames) { // returns clicked entry, or <0 if none
    if( scroll < 0 ) scroll = 0;
    //if (!numgames) return -1;
    int last = numgames ? numgames-1 : 0;

#if 0
    if( key_pressed( TK_CONTROL) && key_trigger( TK_HOME) )
        return selected = 0, scroll = 0, -1;
    if( key_pressed( TK_CONTROL) && key_trigger( TK_END) )
        return selected = numgames % ENTRIES, scroll = numgames / ENTRIES, -1; //fixme
#endif

    unsigned pad = gamepad(1);

    static int tbl[256] = {0};
    int up = key_repeat_( TK_DOWN, tbl) - key_repeat_( TK_UP, tbl);
       up += key_repeat_( PADVK_D, tbl) - key_repeat_( PADVK_U, tbl);
    int pg = key_repeat_( TK_PAGEDN, tbl) - key_repeat_( TK_PAGEUP, tbl);
    int home = key_pressed( TK_HOME) || (key_pressed( TK_UP) && key_held(TK_CONTROL));
    int end  = key_pressed( TK_END) || (key_pressed( TK_DOWN) && key_held(TK_CONTROL));
    int trigger = key_trigger(TK_RETURN) || key_trigger(TK_TAB) || key_trigger(PADVK_A);

    float wheel = mouse().wheel;
    if( wheel ) {
        if( key_pressed( TK_CONTROL) ) {
            pg = wheel > 0 ? 1 : -1;
        }
        else {
            scroll += wheel;
        }
    }

    scroll = CLAMP(scroll, 0, last);
    selected = CLAMP(selected, 0, last);

    int top = scroll, bottom = scroll + ENTRIES - 1;
    if( pg == -1 ) if( selected != top    ) selected = top;    else selected = (scroll -= ENTRIES);
    if( pg == +1 ) if( selected != bottom ) selected = bottom; else selected = (scroll += ENTRIES) + ENTRIES - 1;
    if( up == -1 ) if( selected != top    ) --selected; else --selected, --scroll;
    if( up == +1 ) if( selected != bottom ) ++selected; else ++selected, ++scroll;

    if( home ) scroll = selected = 0;
    if( end  ) scroll = selected = last, scroll -= ENTRIES - 1;

    scroll = CLAMP(scroll, 0, last);
    selected = CLAMP(selected, 0, last);

    // update filter
    int has_finder = !!num_options;
    int any = filters_tick();

    if( trigger && !has_finder ) {
        return selected;
    }

    return -1;
}

char* game_browser_v1() {
    int press_backspace = key_down(TK_BACKSPACE);
    int has_finder = !!num_options;

    const int ENTRIES = (_240/11)-2;
    int chosen = game_browser_keyboard(ENTRIES, numgames);
        // return '..' if finder is not visible
        if( press_backspace && !has_finder ) return selected = 0, scroll = 0, games[0];
        // return game selection
        if( chosen >= 0 ) {
            // if chosen is a dir, reset scroll&cursor for next frame
            if( strendi(games[chosen], "/") ) {
                scroll = 0, selected = 0;
            }
            // return selection
            return games[chosen];
        }

    static char *buffer = 0; if(!buffer) { buffer = malloc(65536); /*rescan();*/ }

    int clicked = 0;

#if 1
    if( filters_event & 1 ) {
        filters_event &= ~1;
        rescan(last_scanned_folder);
    }
#endif

    int y = 0, numitems = 0;
    for( int j = 0; numitems < ENTRIES; ++j, ++numitems ) {
        int i = scroll + j;
        if( i < 0 ) continue;
        if( i >= numgames ) break;

        const char *sep = strrchr(games[i], *DIR_SEP_);
        int is_dir = sep[1] == '\0', is_file = !is_dir;
        if( is_dir ) while(0[--sep] != '/');

        const char *title = sep+1;

#if 0 // done in rescan() function now
        // filtering
        char wildcard[128] = {0};
        int matches = 0, numfilters = 0;
        for( int f = 0; f < countof(filters) && filters[f]; ++f, ++numfilters ) {
            if( snprintf(wildcard, 128, "*%s*", filters[f]) )
                if( strmatchi(title, wildcard) ) ++matches;
        }
        if( numfilters != matches ) { --numitems; continue; }
#endif

        // hinting
        char wildcard[128] = {0};
        if( filter && filter[0] && snprintf(wildcard, 128, "*%s*", filter) ) {
            ui_alpha = 128;
            if( strmatchi(title, wildcard) ) ui_alpha = 255;
        }

        // display item
        int flagged = 0;
        int starred = 0;
        if( i == selected ) {
        if( key_pressed( TK_SHIFT) && key_trigger( TK_SPACE) ) flagged = 1;
        if(!key_pressed( TK_SHIFT) && key_trigger( TK_SPACE) ) starred = 1;
        if( key_pressed( TK_F2) && is_file ) {
            const char *name = prompt("Rename", "Rename file to...", title);
            if( name && strcmp(name, title) ) { // if not canceled...
                if( !strchr(name, '/') && !strchr(name, '\\') ) { // if not a path...
                    // rename .db file...
                    char *src = va("%s/%s.db", last_scanned_folder, title);
                    char *dst = va("%s/%s.db", last_scanned_folder, name);
                    rename(src,dst);
                    // and actual game.
                    src[strlen(src)-3] = '\0';
                    dst[strlen(dst)-3] = '\0';
                    if( rename(src, dst) == 0 ) {
                        free( games[i] );
                        games[i] = strdup(dst);
                    }
                }
            }
        }
        }

        int stars = (dbgames[i] >> 8);
        int flags = (dbgames[i] & 0x7F);

        int loaded; {
        char filename[256]; snprintf(filename, 256, "%.*s", (int)(strlen(title) - is_dir), title);
        loaded = is_dir ? 0 : !!strendi( window_title(NULL), filename );
        }

        static char colors[] = "\7\2\6\4";
        colors[0] = loaded && ZXFlashFlag ? '\5' : '\7';
        int color = colors[flags];

        ui_at(ui, 0, (2+y++) * 11 + 2 + 2 );

        sprintf(buffer, "%c %c%3d.%s", colors[0], loaded ? '*':' ', i+1, i == selected ? ">":" ");
        ui_label(buffer);

#if 0
        ui_y--;
        if( is_file && ui_click("-Played-", ZXFlashFlag ? "":"▏▏\b\b\b\b" ) );
        ui_y++;
#endif

        if( is_file && ui_click("-Toggle bookmark-", va("%c\f", "\x10\x12"[!!stars])) )
            starred = 1;

        if( is_file && ui_click("-Toggle compatibility flags-\n\2fail\7, \6warn\7, \4good", va("%c%s", color, flags == 0 || flags == 3 ? "✓":"╳")) ) // "":""
            flagged = 1;

        sprintf(buffer, "%s%c%.*s\n",
            is_dir ? "\6" FOLDER_STR "\f" : " ", color, (int)(strlen(title) - is_dir), title );

        if( ui_monospaced = 0, ui_click(NULL, buffer) ) selected = i, clicked = 1;

        if( is_file )
        if( starred || flagged ) {
            int nextflag[] = { [0]=3,[1]=0,[2]=1,[3]=2 };
            if( flagged ) flags = nextflag[flags]; // (flags+1) % 4;
            if( starred ) stars = stars != 'B' ? 'B' : 0;
            db_set(games[i], dbgames[i] = flags + (stars << 8));
        }
    }

    // issue browser
    // if( key_trigger( TK_LEFT)  ) { for(--up; (selected+up) >= 0 && (dbgames[selected+up]&0xFF) <= 1; --up ) ; }
    // if( key_trigger( TK_RIGHT) ) { for(++up; (selected+up) < numgames && (dbgames[selected+up]&0xFF) <= 1; ++up ) ; }

    if( clicked ) {
        char *chosen = games[selected];
        // if chosen is a dir, reset scroll&cursor for next frame
        if( strendi(games[selected], "/") ) {
            scroll = 0, selected = 0;
        }
        return chosen;
    }

    return NULL;
}

static void rgba_blit(Tigr *dst, const rgba *src, int dx, int dy, int sx, int sy, int sw, int sh) {
    Tigr tx = {0}; tx.pix = (TPixel*)src; tx.w = sw, tx.h = sh;
    tigrBlit(dst, &tx, dx,dy, sx,sy,sw,sh);
}

char* game_browser_v2() {
    // decay to local file browser if no ZXDB is present
    if( !zxdb_loaded() ) return ZX_BROWSER = 1, NULL;

#if 0
    if (!numgames) return 0;
    if( scroll < 0 ) scroll = 0;
#endif

    int selection[2] = {0};

    // handle input
    struct mouse m = mouse();
    int up = key_repeat( TK_UP); up += key_repeat(PADVK_U);
    int down = key_repeat( TK_DOWN); down += key_repeat(PADVK_D);
    int left = key_repeat( TK_LEFT); left += key_repeat(PADVK_L);
    int right = key_repeat( TK_RIGHT); right += key_repeat(PADVK_R);
    int page_up = key_repeat(TK_PAGEUP);
    int page_down = key_repeat(TK_PAGEDN);

    // constants

    const int LINE_HEIGHT = 11;
    const int UPPER_SPACING = 2;
    const int BOTTOM_SPACING = 3 * LINE_HEIGHT;

    // vars

    static int page = 0;
    static int thumbnails = 0; // 0 text, 3 (3x3), 6 (6x6), 12 (12x12)

    static VAL **list = 0;
    static int list_num = 0;

    // background (text mode only)

    static rgba *background_texture = 0;
    if( thumbnails == 0 )
    if( background_texture ) {
#if 0
        int factor = 0, f256 = 256/(1<<factor), f192 = 192/(1<<factor);
        if( _320 > 256 && _240 > 192 ) // @fixme: impl clipping instead
        for(int i = 0, x = (_320-256)/2, y = (_240-192)/2; i < f192; ++i) {
            memcpy(&ui->pix[x+(y+i)*_320], background_texture + (0+i*f256), f256*4);
        }
#else
        rgba_blit(ui, background_texture, (_320-256)/2,(_240-192)/2, 0,0,256,192);
#endif

        // small hack to have further text alpha blendings correctly
        rgba_blit(app, background_texture, (_320-256)/2,(_240-192)/2, 0,0,256,192);
    }

    // upper tabs

//  static const char *tab = 0;
    static const char *tabs = "\x17#ABCDEFGHIJKLMNOPQRSTUVWXYZ\x12\x18"; // "⧖"

    do_once tab = tabs+ZX_TABS; // look up default 'A' tab

    // refresh list if filters were changed
    static int refresh = 0;
    if( filters_event & 2 ) refresh = 1, filters_event &= ~2;

    // display tabs
    ui_at(ui, (_320-330)/2, UPPER_SPACING);
    for(int i = 0; tabs[i]; ++i) {
        if( (ui_at(ui, ui_x+4, ui_y), ui_button(NULL, va("%c%c", (tab && tabs[i] == *tab) ? 5 : 7, tabs[i])) )) {
            if( ui_hover ) {
                /**/ if(tabs[i] == '\x17') ui_notify( "-Browse local folder-\nClick again to change folder" );
                else if(tabs[i] == '\x12') ui_notify( "-List Bookmarks-\nClick again to change thumbnails view" );
                else if(tabs[i] == '\x18') ui_notify( "-Search-" );
                else if(tabs[i] ==    '#') ui_notify( "-List other games-\nClick again to change thumbnails view" );
                else                       ui_notify( va("-List %c games-\nClick again to change thumbnails view", tabs[i]) );
            }
            if( ui_click ) {
                if( tab == (tabs+i) ) {
                    // reclick
                    if( *tab == '\x17' ) {
                        extern int cmdkey;
                        extern const char *cmdarg;
                        cmdkey = 'SCAN';
                        cmdarg = 0; // ZX_FOLDER;
                    }
                    else if( *tab == '\x18' ) {
                        filters_event |= 4;
                        page = 0;
                    }
                    else {
                        int next[] = { [0]=3,[3]=6,[6]=12,[12]=0 };
                        thumbnails = next[thumbnails];
                    }
                }
                tab = tabs + i;
            }
        }
    }

#if 0
    if( left )  if(!tab) tab = tabs; else if(*tab-- == '#') tab = tabs + 29;
    if( right ) if(!tab) tab = tabs; else if(*tab++ == 'Z') tab = tabs +  3;
#else
    const char *first = strchr(tabs,numgames ? '\x17' : '#'), *last = strrchr(tabs,'\x12');

    if(!tab) tab = tabs;
    tab += right - left;
    if( tab < first || tab > last ) tab = left ? last : right ? first : tab;
#endif

    if( tab && *tab != 0x18 ) // store user's tab
        ZX_TABS = (int)(strchr(tabs, *tab) - tabs);

    static const char *prev = 0;
    refresh |= tab && prev != tab;
    if( refresh ) {
        refresh = 0;

        selected = scroll = 0; // reset keyboard scroller

        if( *tab == '\x17' ) {
            extern int cmdkey;
            extern const char *cmdarg;
            do_once cmdkey = 'SCAN', cmdarg = ZX_FOLDER;

            if( background_texture ) free(background_texture), background_texture = 0;
            background_texture = thumbnail(VRAM, 6912, 1, ZXFlashFlag);
            // dim background
            for( rgba *p = background_texture, *pend = &p[256 + 191 * 256]; p < pend; ++p ) {
                // *p = ( *p & 0x00f8f8f8 ) >> 3 | 0xff000000;
                // *p = ( *p & 0x00fcfcfc ) >> 2 | 0xff000000;
                *p = ( *p & 0x00fefefe ) >> 1 | 0xff000000;
            }

            //ZX_BROWSER = 1; // decay to file browser

            //prev = tab;
            //return NULL;
            //tab = 0;
            //prev = 0;
            //list = 0;
            //list_num = 0;
            //return NULL;
        }
        else
        if( *tab == '\x18' ) {
            if( !filters[0] ) {
                filters_event |= 4;
                page = 0;
            }
        }
        else {
            page = 0;
        }

        int is_bookmark_tab = tab && *tab == '\x12';

        // search & sort
        free(list), list = 0;
        if( !is_bookmark_tab ) {
            list = map_multifind(&zxdb2, va("%.*s*", 1, *tab == '\x18' ? (filters[0] ? "" : "\1") : *tab == '#' ? "?" : tab), &list_num);
        }
        else {
            // bookmarks
            list_num = 0;
            list = realloc(list, 65535 * sizeof(VAL*));
            memset(list, 0, 65535 * sizeof(VAL*));
            for( int i = 0; i < 65535; ++i) {
                if( cache_get(i) & 0x0f ) {
                    VAL *ptr = map_find(&zxdb2, va("#%d", i));
                    if( ptr ) list[ list_num++ ] = ptr;
                }
            }
            // list = realloc(list, list_num * sizeof(VAL*));
        }

        int list_xxx = 0;
        int list_mia = 0;
        // int list_demos = 0;
        // int list_tests = 0; // @todo
        // int list_texts = 0; // text adventures

        int num_filters = 0;
        for( int f = 0; f < countof(filters) && filters[f]; ++f ) {
            /**/ if(!strcmpi(filters[f], "xxx"))  list_xxx ^= 1;
            else if(!strcmpi(filters[f], "mia"))  list_mia ^= 1;
            // else if(!strcmpi(filters[f], "demo")) list_demos ^= 1;
            //else if "test"
            // else if(!strcmpi(filters[f], "text")) list_texts ^= 1;
            else ++num_filters;
        }

        for( int i = 0; i < list_num; ) {
            const char *zx_id = (const char*)*list[i];
            const char *years = strchr(zx_id, '|')+1; int zx_id_len = years-zx_id-1;
            const char *title = strchr(years, '|')+1; int years_len = title-years-1;
            const char *alias = strchr(title, '|')+1; int title_len = alias-title-1;
            const char *brand = strchr(alias, '|')+1; int alias_len = brand-alias-1;
            const char *avail = strchr(brand, '|')+1; int brand_len = avail-brand-1;
            const char *score = strchr(avail, '|')+1; int avail_len = score-avail-1;
            const char *genre = strchr(score, '|')+1; int score_len = genre-score-1;
            const char *tags_ = strchr(genre, '|')+1; int genre_len = tags_-genre-1;

            // apply filters, if any.
            int matches = 0;
            for( int f = 0; f < countof(filters) && filters[f]; ++f ) {
                if(strcmpi(filters[f], "xxx"))
                if(strcmpi(filters[f], "mia"))
                //if(strcmpi(filters[f], "demo"))
                //if "test"
                //if(strcmpi(filters[f], "text"))
                {
                    int match = 0;
                    char tmp1[256], tmp2[256];

                    // year
                    if(years_len) if(!match) match |= !!strmatchi((snprintf(tmp1,256,"%.*s",years_len,years),tmp1), (snprintf(tmp2,256,"*%s*",filters[f]),tmp2));

                    // brand/genre
                    if(brand_len) if(!match) match |= !!strmatchi((snprintf(tmp1,256,"%.*s",brand_len,brand),tmp1), (snprintf(tmp2,256,"*%s*",filters[f]),tmp2));
                    if(genre_len) if(!match) match |= !!strmatchi((snprintf(tmp1,256,"%.*s",genre_len,genre),tmp1), (snprintf(tmp2,256,"%d*%s*",atoi(genre),filters[f]),tmp2));

                    // alias/title
                    if(alias_len) if(!match) match |= !!strmatchi((snprintf(tmp1,256,"%.*s",alias_len,alias),tmp1), (snprintf(tmp2,256,"*%s*",filters[f]),tmp2));
                                  if(!match) match |= !!strmatchi((snprintf(tmp1,256,"%.*s",title_len,title),tmp1), (snprintf(tmp2,256,"*%s*",filters[f]),tmp2));

                    // authors: @todo: trim downloads from here
                    if(1)         if(!match) match |= !!strmatchi(zx_id, (snprintf(tmp2,256,"*@?*%s*",filters[f]),tmp2));

                    // tags: @todo: trim authors and downloads from here
                    if(1)         if(!match) match |= !!strmatchi(tags_, (snprintf(tmp2,256,"*#%s*",filters[f]),tmp2));

                    matches += match;
                }
            }

            int is_xxx = avail[1] == 'X'; // XXX
            int is_mia = !strchr("ARD", avail[0]); // MIA(?) [include (A)vailable,(R)ecovered,(D)enied games],For(S)ale,(N)everReleased,Dupes(*)
            int is_demo = atoi(genre) >= 72; // demos 72..78
            int is_text = atoi(genre) == 5 || atoi(genre) == 6; // text-only(5) and text-illustrated(6)

            int excluded = matches != num_filters;

            if( is_bookmark_tab ) {
                excluded |= list_xxx   && !is_xxx;
                excluded |= list_mia   && !is_mia;
                //excluded |= list_demos && !is_demo;
                //excluded |= list_texts && !is_text;
            } else {
                excluded |= list_xxx   != is_xxx;
                excluded |= list_mia   != is_mia;
                //excluded |= list_demos != is_demo;
                //excluded |= list_texts != is_text;
            }

            if( excluded ) {
                // remove
                if (i + 1 < list_num) // ensure we dont remove final item in list
                    memmove(list + i, list + i + 1, ( list_num - i - 1 ) * sizeof(list[0]));
                --list_num;
            } else {
                // continue
                ++i;
            }
        }

        // sort list
        if( list_num ) qsort(list, list_num, sizeof(VAL*), zxdb_compare_by_name);

        // remove dupes (like aliases)
        int i = 1;
        while (i < list_num) {
            if( *(char**)list[i-1] == *(char**)list[i] ) { // remove current element (i)
                memmove(list + i, list + i + 1, (list_num - i - 1) * sizeof(list[0]));
                --list_num;
            }
            else ++i;
        }


        prev = tab;
    }

    // main content

    double progress_x = _320 * worker_progress(), progress_y = ui_y + LINE_HEIGHT;
    tigrLine(ui, 0, progress_y+1, progress_x, progress_y+1, ui_00);
    tigrLine(ui, 0, progress_y, progress_x, progress_y, ui_ff);

    if( !tab ) return NULL;

    char *rc = NULL;
    /**/ if( *tab == '\x17' ) rc = game_browser_v1(); // NULL;
    else {

        ui_at(ui, 0, UPPER_SPACING+2*LINE_HEIGHT+2);

        int ENTRIES_PER_PAGE = (_240-ui_y/*-BOTTOM_SPACING*/)/LINE_HEIGHT;

    //    if( thumbnails == 0 ) ENTRIES_PER_PAGE = 25;
        if( thumbnails ==  3 ) ENTRIES_PER_PAGE = 3*3;
        if( thumbnails ==  6 ) ENTRIES_PER_PAGE = 6*6;
        if( thumbnails == 12 ) ENTRIES_PER_PAGE = 12*12;

    #if 0
        int NUM_PAGES = list_num / ENTRIES_PER_PAGE;
        int trailing_page = list_num % ENTRIES_PER_PAGE;
        NUM_PAGES -= NUM_PAGES && !trailing_page;

        if( page > NUM_PAGES ) page = NUM_PAGES - 1;
        if( page < 0 ) page = 0;

        if( up || page_up )     if(--page < 0) page = 0;
        if( down || page_down ) if(++page >= NUM_PAGES) page = NUM_PAGES;

        if( key_pressed( TK_HOME) ) page = 0;
        if( key_pressed( TK_END)  ) page = NUM_PAGES;
    #endif

        int chosen = game_browser_keyboard(ENTRIES_PER_PAGE, list_num);

        static byte frame4 = 0; frame4 = (frame4 + 1) & 3; // 4-frame anim
        static byte frame8 = 0; frame8 = (frame8 + 1) & 7; // 8-frame anim

        if( 0 && thumbnails ) {
            scroll = scroll - (sqrt(ENTRIES_PER_PAGE) - (scroll % (int)sqrt(ENTRIES_PER_PAGE)));
            if( scroll < 0 ) scroll = 0;
        }

        int larger_factor = 0;
        const rgba *larger_preview = 0;

        if( list )
        for( int j = 0, cnt = 0, len; j < ENTRIES_PER_PAGE; ++j, ++cnt ) {
            int i = scroll + j;
            if( i < 0 ) continue;
            if( i >= list_num ) continue;

            const char *zx_id = (const char*)*list[i];
            const char *years = strchr(zx_id, '|')+1; int zx_id_len = years-zx_id-1;
            const char *title = strchr(years, '|')+1; int years_len = title-years-1;
            const char *alias = strchr(title, '|')+1; int title_len = alias-title-1;
            const char *brand = strchr(alias, '|')+1; int alias_len = brand-alias-1;
            const char *avail = strchr(brand, '|')+1; int brand_len = avail-brand-1;
            const char *score = strchr(avail, '|')+1; int avail_len = score-avail-1;
            const char *genre = strchr(score, '|')+1; int score_len = genre-score-1;
            const char *tags_ = strchr(genre, '|')+1; int genre_len = tags_-genre-1;

            // replace title if alias is what we're searching for
            if( *tab == '#' ) {
            if( *alias != '|' && !isdigit(*title) && !ispunct(*title) ) title = alias, title_len = alias_len;
            } else {
            if( title[0] != *tab && alias[0] == *tab ) title = alias, title_len = alias_len;
            }

            // also display alias right after any matching IDs are found twice
            // ie, gremlins 2: la nueva generacion (#2139) and gremlins 2: the new batch (#2139)
            bool use_alt = i && atoi((char*)*list[i]) == atoi((char*)*list[i-1]);
            if( use_alt ) {
                title = alias; title_len = alias_len;
            }

            // replace year if title was never released
            if( years[0] == '9' ) years = "?", years_len = 1; // "9999"

            // replace brand if no brand is given. use 1st author if possible
            if( brand[0] == '|' ) {
                const char *next = strchr(zx_id, '\n');
                if( next && next[1] == '@' ) { // x3 skips: '\n' + '@' + 'R'ole
                    brand = next+1+1+1, brand_len = strcspn(brand, "@\r\n");
                }
            }

            // stars, user-score
            const char *stars[] = {
                /*"\2"*/"\f\x10\f\x10\f\x10", // 0 0 0
                /*"\2"*/"\f\x11\f\x10\f\x10", // 0 0 1
                /*"\2"*/"\f\x12\f\x10\f\x10", // 0 1 0
                /*"\2"*/"\f\x12\f\x11\f\x10", // 0 1 1
                /*"\2"*/"\f\x12\f\x12\f\x10", // 1 0 0
                /*"\2"*/"\f\x12\f\x12\f\x11", // 1 0 1
                /*"\2"*/"\f\x12\f\x12\f\x12", // 1 1 0
                /*"\2"*/"\f\x12\f\x12\f\x12", // 1 1 1
            };

            extern zxdb ZXDB;
            int loaded = ZXDB.ids[0] && atoi(ZXDB.ids[0]) == atoi(zx_id);
            if( loaded ) selection[0] = ui_x, selection[1] = ui_y;

            static char colors[] = "\7\2\6\4";
            colors[0] = loaded && ZXFlashFlag ? '\5' : '\7';

            int dbid = atoi(zx_id);
            int vars = cache_get(dbid);
            int star = (vars >> 0) & 0x0f; assert(star <= 3);
            int flag = (vars >> 4) & 0x0f; assert(flag <= 3);

            // build title and clean it up
            char full_title[512], year_[16], brand_[32];
            snprintf(year_,  sizeof(year_),  "(%.*s)", years_len, years);
            snprintf(brand_, sizeof(brand_), "(%.*s)", brand_len, brand);
            snprintf(full_title, sizeof(full_title), " %.*s (%.*s)(%.*s)", title_len, title, years_len, years, brand_len, brand);
            for( int i = 1; full_title[i]; ++i )
                if( i == 1 || full_title[i-1] == '.' )
                    full_title[i] = toupper(full_title[i]);

            full_title[0] = colors[flag];

            int clicked = 0, flagged = 0, starred = 0;
            int iterated = selected == i;
            int dimmed = 0;

            // filtering
            char wildcard[128] = {0};
            int matches = 0, numfilters = 0;
            for( int f = 0; f < countof(filters) && filters[f]; ++f, ++numfilters ) {
                if( snprintf(wildcard, 128, "*%s*", filters[f]) ) {
                    matches += !!strmatchi(full_title, wildcard);
                }
            }
            if( matches == numfilters )

            // hinting
            if( filter && filter[0] && snprintf(wildcard, 128, "*%s*", filter) ) {
                dimmed = !strmatchi(full_title, wildcard);
                ui_alpha = !thumbnails && dimmed ? 64 : 255;
            }


            if( !thumbnails ) {

                extern int cmdkey; extern const char *cmdarg;
                char url[128];
                snprintf(url, 128-1, "-Open link-\nhttps://spectrumcomputing.co.uk/entry/%.*s", zx_id_len, zx_id);

                ui_label(va("%c %c%3d.%s", colors[0], loaded ? '*':' ', i+1, i==selected ? ">":" "));

                if( ui_click("-Toggle bookmark-", "%c\f", "\x10\x12"[!!star]) )
                    starred = 1;

                if( ui_click("-Toggle compatibility flags-\n\2fail\7, \6warn\7, \4good", "%c%s\f\f", colors[flag], flag == 0 || flag == 3 ? "✓":"╳") ) // "":""
                    flagged = 1;

                if( ui_click(url, "%c\x19\f\f", colors[flag] ) )
                    cmdkey = 'LINK', cmdarg = va("%s", url + countof("-Open link-\n")-1);

                ui_label(" ");

#if 0
                if( ui_monospaced = 0, ui_button(NULL,full_title) ) {
                    if( ui_hover ) {
                        if( ui_click ) {
                            clicked = 1;
                        }
#else
                int hovers = 0;
                char *title = full_title; strstr(full_title, " (")[1] = 0;
                if( ui_monospaced = 0, ui_button(NULL, title) ) { hovers|=ui_hover; if(ui_click) clicked = 1; }
                if( ui_monospaced = 0, ui_button(NULL, va("%c%s",colors[flag],year_)) ) { hovers|=ui_hover; if(ui_click) year_[strlen(year_)-1]=0,filters_add(year_+1); }
                if( ui_monospaced = 0, ui_button(NULL, va("%c%s",colors[flag],brand_))) { hovers|=ui_hover; if(ui_click) brand_[strlen(brand_)-1]=0,filters_add(brand_+1); }

                if( hovers ) {
                    if( 1 ) {
#endif
                        // replace background
                        const byte *data = zxdb_screen_async(va("#%.*s", zx_id_len, zx_id), &len, 0);
                        if( data ) {
                            if( background_texture ) free(background_texture), background_texture = 0;

                            int ix,iy,in;
                            rgba *bitmap = (rgba*)stbi_load_from_memory(data, len, &ix, &iy, &in, 4);
                            if( bitmap ) {
                                background_texture = ui_resize(bitmap, ix, iy, 256/1, 192/1, 1, 0);
                                stbi_image_free(bitmap);
                            } else {
                                background_texture = thumbnail(data, len, 1, ZXFlashFlag);
                            }
                        }

                        // dim background
                        if( background_texture )
                        for( rgba *p = background_texture, *pend = &p[256 + 191 * 256]; p < pend; ++p ) {
                            // *p = ( *p & 0x00f8f8f8 ) >> 3 | 0xff000000;
                            // *p = ( *p & 0x00fcfcfc ) >> 2 | 0xff000000;
                            *p = ( *p & 0x00fefefe ) >> 1 | 0xff000000;
                        }
                    }

                }

                ui_button(NULL, "\n");
                ui_y--; // compact
            }
            else {
                int t = thumbnails, _16 = 16; // _16 = 28; // progress_y+2;
                int w = _320/t, h = (_240-_16)/t;
                int x = (cnt % t) * w, y = _16 + (cnt / t) * h;
                int has_thumb = 0;

                int factor = t >= 12 ? 3 : t >= 6 ? 2 : t >= 3 ? 1 : 0;
                const char *data = zxdb_screen_async(va("#%.*s", zx_id_len, zx_id), &len, factor);
                if( data && len ) {
                    // blit
                    const rgba *bitmap = (rgba*)data;
                    int f256 = 256/(1<<factor), f192 = 192/(1<<factor);
                    int cx = (_320-_256)/t/2, cy = ((_240-_16)-_192)/t/2;
                    rgba_blit(ui, bitmap, x+cx,y+0*cy, 0,0,f256,f192);

                    has_thumb = 1;
                }

                if( dimmed && filter && filter[0] )
                for( int iy = y, iyend = y+h; iy < iyend; ++iy )
                for( TPixel *ix = &ui->pix[ x + iy * _320 ], *ixend = ix+w; ix < ixend; ++ix ) {
                    ix->rgba = ( ix->rgba & 0x00f8f8f8 ) >> 3 | 0xff000000;
                }

                int hovered = m.x >= x && m.x < (x+w) && m.y >= y && m.y < (y+h);
                if( hovered || iterated || loaded ) {
                    unsigned bak = ui_colors[1];
                    if( hovered  ) ui_colors[1] = RGB(255,255,255);
                    if( iterated ) ui_colors[1] = RGB(255,255,255);
                    if( loaded   ) ui_colors[1] = ZXFlashFlag ? RGB(0,255,255) : RGB(255,255,255);
                    ui_rect(ui, x,y, x+w-1,y+h-1);
                    ui_colors[1] = bak;
                }
                if( hovered || iterated ) {
                    ui_at(ui, x+2, y+2);

                    if( ui_click(NULL, va("%c\f", "\x10\x12"[!!star])) )
                        starred = 1;

                    if( ui_click(NULL, va("%c%s", colors[flag], flag == 0 || flag == 3 ? "✓":"╳")) ) // "":""
                        flagged = 1;

                    if( hovered ) {
                        if( filters[0] && y == 16 && (m.y < 16+11) ) {
                            // do nothing, as user is hovering the filters and not the gallery pics
                        }
                        else
                        if( m.x <= (x+10) && m.y <= (y+11) ) {
                            ui_notify("-Toggle bookmark-");
                        }
                        else
                        if( m.x <= (x+10+10-4) && m.y <= (y+11) ) {
                            ui_notify("-Toggle compatibility flags-\n\2fail\7, \6warn\7, \4good");
                        }
                        else {
                            mouse_cursor(2);
                            ui_notify(full_title);
                            clicked = m.lb;
                        }

                        // prepare to blit larger preview
                        if( thumbnails >= 6 ) {
                            larger_preview = (const rgba *)zxdb_screen_async(va("#%.*s", zx_id_len, zx_id), NULL, larger_factor = 1);
                        }
                    }
                }
                else {
                    ui_at(ui, x, y);

                    if( !has_thumb ) {
                        const char *anims8[] = {
                            "","","","",
                            "","","","",
                        };
                        const char *anims4[] = {
                            "","",
                            "","",
                        };
                        const char *id = va("%x\n%s", atoi(zx_id), anims4[ (atoi(zx_id) + frame4) & 3 ] );
                        if( ui_click(id, id) ) clicked = 1;
                    }
                }
            }

            if( !num_options )
            if( selected == i ) {
                if(!key_held(TK_SHIFT) && key_down(TK_SPACE) ) starred = 1;
                if( key_held(TK_SHIFT) && key_down(TK_SPACE) ) flagged = 1;
            }

            if( flagged || starred ) {
                int nextflag[] = { [0]=3,[1]=0,[2]=1,[3]=2 };
                if( flagged ) flag = nextflag[flag]; // (flag+1) % 4;
                if( starred ) star = !star;
                cache_set(dbid, (vars & 0xff00) | (flag << 4) | star);
            }
                        
            if( clicked || (chosen >= 0 && chosen == i) ) {
                selected = i;

                browser = 0;
    #if 0
    //          tab = 0;
                prev = 0;
                list = 0;
                list_num = 0;
    #endif

                extern int cmdkey;
                extern const char *cmdarg;
                cmdkey = 'ZXDB', cmdarg = va("#%.*s", zx_id_len, zx_id);

                //return NULL;
            }
        }

        // blit larger preview
        if( larger_preview ) {
            int f256 = 256/(1<<larger_factor), f192 = 192/(1<<larger_factor);

            int x = mouse().x; 
            int y = mouse().y; 

            {float dt = x / (float)_320; x += 8;  x -= (f256 + 8*2) * dt; }
            // {float dt = y / (float)_240; y += 16; y -= (f192 + 16*2) * dt; }

            // x = (x + f256) > _320 ? x - f256 - 8 : x + 8;
            y = (y + f192) > _240 ? y - f192 - 16 : y + 16;

            static float alpha = 0.75;
            static float at_x = 0, at_y = 0; do_once at_x = x, at_y = y;
            at_x = at_x * alpha + x * (1-alpha);
            at_y = at_y * alpha + y * (1-alpha);

            rgba_blit(ui, larger_preview, at_x,at_y, 0,0,f256,f192);
            ui_rect(ui, at_x-1,at_y-1,at_x+f256+1-1,at_y+f192+1-1);
        }

    }

    filters_draw(ui, (_320-330)/2, UPPER_SPACING*2+11);
    return rc;
}
