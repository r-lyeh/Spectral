#include "res/scr/question_mark"

zxdb ZXDB2;

rgba* thumbnail(const byte *VRAM_, int len, unsigned downfactor, int ZXFlashFlag) {
    int w = 256 / downfactor, h = 192 / downfactor;

    rgba *texture = malloc( w * h * 4 ), *cpy = texture;
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
            byte pixel = *pixels, fg, bg;

            // @fixme: make section branchless

            pixel ^= (attr & 0x80) && ZXFlashFlag ? 0xff : 0x00;
            fg = (attr & 0x07) | ((attr & 0x40) >> 3);
            bg = (attr & 0x78) >> 3;

            if( downfactor == 1 ) {
            texture[0]=ZXPalette[pixel & 0x80 ? fg : bg];
            texture[1]=ZXPalette[pixel & 0x40 ? fg : bg];
            texture[2]=ZXPalette[pixel & 0x20 ? fg : bg];
            texture[3]=ZXPalette[pixel & 0x10 ? fg : bg];
            texture[4]=ZXPalette[pixel & 0x08 ? fg : bg];
            texture[5]=ZXPalette[pixel & 0x04 ? fg : bg];
            texture[6]=ZXPalette[pixel & 0x02 ? fg : bg];
            texture[7]=ZXPalette[pixel & 0x01 ? fg : bg];
            texture += 8;
            }
            else if( downfactor == 2 ) {
            texture[0]=ZXPalette[pixel & 0x80 ? fg : bg];
            texture[1]=ZXPalette[pixel & 0x20 ? fg : bg];
            texture[2]=ZXPalette[pixel & 0x08 ? fg : bg];
            texture[3]=ZXPalette[pixel & 0x02 ? fg : bg];
            texture += 4;
            }
            else if( downfactor == 4 ) {
            texture[0]=ZXPalette[pixel & 0x80 ? fg : bg];
            texture[1]=ZXPalette[pixel & 0x08 ? fg : bg];
            texture += 2;
            }
            else if( downfactor == 8 ) {
            texture[0]=ZXPalette[pixel & 0x80 ? fg : bg];
            texture += 1;
            }

            pixels++;
            attribs++;
        }

        texture = bak + w;
    }

    return cpy;
}



static
const char *tab;

static
int zxdb_compare_by_name(const void *arg1, const void *arg2) { // @fixme: roman
    char **a = (char**)*(VAL**)arg1; char *entry = *a;
    char **b = (char**)*(VAL**)arg2; char *other = *b;

    char *year1  = strchr(entry,  '|')+1;
    char *title1 = strchr(year1,  '|')+1;
    char *alias1 = strchr(title1, '|')+1;

    char *year2  = strchr(other,  '|')+1;
    char *title2 = strchr(year2,  '|')+1;
    char *alias2 = strchr(title2, '|')+1;

    if( *tab == '#' ) {
        if( *alias1 != '|' && (isdigit(*alias1) || ispunct(*alias1)) ) title1 = alias1;
        if( *alias2 != '|' && (isdigit(*alias2) || ispunct(*alias2)) ) title2 = alias2;
    } else {
        if( *title1 != *tab && *alias1 == *tab ) title1 = alias1;
        if( *title2 != *tab && *alias2 == *tab ) title2 = alias2;
    }

    return strcmpi(title1, title2);
}

char *zxdb_screen(const char *id, int *len) {
    if( id && id[0] && strcmp(id, "0") && strcmp(id, "#") ) {
        ZXDB2 = zxdb_search( id );

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

bool zxdb_load(const char *id, int ZX_MODEL) {
    if( id && id[0] && strcmp(id, "0") && strcmp(id, "#") ) {
        ZXDB2 = zxdb_search( id );

        if( ZXDB2.ids[0] ) {
            int len;

            static char *data = 0;
            if( data ) free(data), data = 0;
            if(!data ) data = zxdb_download(ZXDB2,zxdb_url(ZXDB2, "play"), &len);
            if( data ) {

                if( ZX_MODEL ) {
                    boot(ZX = ZX_MODEL, ~0u);
                }
                else {
                    ZX_PENTAGON = 0;
                    char *model = strchr(ZXDB2.ids[5], ',')+1;
                    /**/ if( strstr(model, "Pentagon") ) boot(ZX = 128, ~0u), ZX_PENTAGON = 1, rom_restore();
                    else if( strstr(model, "+3") )       boot(ZX = 300, ~0u);
                    else if( strstr(model, "+2A") )      boot(ZX = 210, ~0u);
                    else if( strstr(model, "+2B") )      boot(ZX = 210, ~0u);
                    else if( strstr(model, "+2") )       boot(ZX = 200, ~0u);
                    else if( strstr(model, "USR0") )     boot(ZX = 128, ~0u); // @fixme
                    else if( strstr(model, "128") )      boot(ZX = 128, ~0u);
                    else if( strstr(model, "48") )       boot(ZX = 48 + 80 * (atoi(ZXDB2.ids[1]) >= 1987), ~0u);
                    else                                 boot(ZX = 16, ~0u);
                }

        #if 0
                loadbin(data, len, true);
        #else
                // this temp file is a hack for now. @fixme: move the zip/rar/fdi loaders into loadbin()
                for( FILE *fp = fopen("spectral.$$2", "wb"); fp; fwrite(data, len, 1, fp), fclose(fp), fp = 0) {
                }
                loadfile("spectral.$$2", 1);
                unlink("spectral.$$2");
        #endif
            }

            ZXDB = ZXDB2;

            return true; // @fixme: verify that previous step went right
        }
    }
    return false;
}

bool zxdb_reload(int ZX_MODEL) {
    return ZXDB.ids[0] ? zxdb_load(va("#%s", ZXDB.ids[0]), ZX_MODEL) : false;
}







#pragma pack(push, 1)
typedef struct cache_t {
    uint16_t likes : 4;
    uint16_t flags : 4;
    uint16_t reserved : 8;
} cache_t;
#pragma pack(pop)

typedef int static_assert_cache_t[sizeof(cache_t) == 2];

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


thread_ptr_t worker;
thread_queue_t queue;
struct queue_t {
    zxdb z;
    char *url;
};
void* queue_values[144]; // 12x12 thumbnails max
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
        void* item = thread_queue_consume(&queue, THREAD_QUEUE_WAIT_INFINITE);
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
                    screens[id][0][1] = screens[id][1][1] = (const byte*)ui_resize(bitmap, ix, iy, 256/2, 192/2, 1);
                    screens[id][0][2] = screens[id][1][2] = (const byte*)ui_resize(bitmap, ix, iy, 256/4, 192/4, 1);
                    screens[id][0][3] = screens[id][1][3] = (const byte*)ui_resize(bitmap, ix, iy, 256/8, 192/8, 1);
                    stbi_image_free(bitmap);
                } else {
                    bitmap = thumbnail(bin, len, 1, 0); ix = 256, iy = 192;
                    screens[id][0][1] = (const byte*)ui_resize(bitmap, ix, iy, 256/2, 192/2, 1);
                    screens[id][0][2] = (const byte*)ui_resize(bitmap, ix, iy, 256/4, 192/4, 1);
                    screens[id][0][3] = (const byte*)ui_resize(bitmap, ix, iy, 256/8, 192/8, 1);
                    free(bitmap);
                    bitmap = thumbnail(bin, len, 1, 1); ix = 256, iy = 192;
                    screens[id][1][1] = (const byte*)ui_resize(bitmap, ix, iy, 256/2, 192/2, 1);
                    screens[id][1][2] = (const byte*)ui_resize(bitmap, ix, iy, 256/4, 192/4, 1);
                    screens[id][1][3] = (const byte*)ui_resize(bitmap, ix, iy, 256/8, 192/8, 1);
                    free(bitmap);
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
    // init
    int capacity = sizeof(queue_values) / sizeof(queue_values[0]);
    if(!worker) thread_queue_init(&queue, capacity, queue_values, 0);
    if(!worker) thread_detach( worker = thread_init(worker_fn, NULL, "worker_fn", THREAD_STACK_SIZE_DEFAULT) );
    // 
    if( thread_queue_count(&queue) < capacity )
        if( thread_queue_produce(&queue, queue_t_new(z,url), ms ) ) // THREAD_QUEUE_WAIT_INFINITE );
            return printf("queue send %s\n", url), 1;
    return 0;
}

const
char *zxdb_screen_async(const char *id, int *len, int factor) {
    if( id && id[0] && strcmp(id, "0") && strcmp(id, "#") ) {
        ZXDB2 = zxdb_search( id );

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
                return *len = screens_len[zxdb_id], screens[zxdb_id][ZXFlashFlag][factor & 3];
            }
        }
    }
    return NULL;
}









int active; // @todo: rename to browser_active, or library_active

extern Tigr *app, *ui;
extern char *last_load;

char **games;
int *dbgames;
int numgames;
int numok,numwarn,numerr; // stats
void rescan(const char *folder) {
    if(!folder) return;
    if(ZX_PLAYER) return; // zxplayer has no library

    // clean up
    while( numgames ) free(games[--numgames]);
    games = realloc(games, 0);

    // refresh stats
    {
        numok=0,numwarn=0,numerr=0;

        for( dir *d = dir_open(folder, "r"); d; dir_close(d), d = NULL ) {
            for( unsigned count = 0, end = dir_count(d); count < end; ++count ) {
                if( !dir_file(d, count) ) continue;

                const char *fname = dir_name(d, count);
                if( strendi(fname, ".db") ) {
                    for(FILE *fp2 = fopen(fname, "rb"); fp2; fclose(fp2), fp2=0) {
                        int ch;
                        fscanf(fp2, "%d", &ch); ch &= 0xFF;
                        numok += ch == 1;
                        numerr += ch == 2;
                        numwarn += ch == 3;
                    }
                }
            }
            for( unsigned count = 0, end = dir_count(d); count < end; ++count ) {
                if( !dir_file(d, count) ) continue;

                const char *fname = dir_name(d, count);
                if( file_is_supported(fname,ALL_FILES) ) {
                    // append
                    ++numgames;
                    games = realloc(games, numgames * sizeof(char*) );
                    games[numgames-1] = strdup(fname);
                    //
                    dbgames = realloc(dbgames, numgames * sizeof(char*) );
                    dbgames[numgames-1] = db_get(fname);
                }
            }
        }
    }

    printf("%d games\n", numgames);
}
void draw_compatibility_stats(window *layer) {
    // compatibility stats
    int total = numok+numwarn+numerr;
    if(total && active) {
    TPixel white = {255,255,255,255}, black = {0,0,0,255}, *bar = &ui->pix[0 + _239 * _320];
    int num1 = (numok * (float)_319) / total;
    int num2 = (numwarn * (float)_319) / total;
    int num3 = (numerr * (float)_319) / total; if((num1+num2+num3)<_319) num1 += _319 - (num1+num2+num3);
    for( int x = 0; x <= num1; ++x ) bar[x-320]=bar[x] = tigrRGB(64,255,64);
    for( int x = 0; x <= num2; ++x ) bar[x+num1-320]=bar[x+num1] = tigrRGB(255,192,64);
    for( int x = 0; x <= num3; ++x ) bar[x+num1+num2-320]=bar[x+num1+num2] = tigrRGB(255,64,64);
    static char compat[64];
    snprintf(compat, 64, "  OK:%04.1f%%     ENTER:128, +SHIFT:48, +CTRL:Try turbo", (total-numerr) * 100.f / (total+!total));
    window_printxy(layer, compat, 0,(_240-12.0)/11);
    }
}

int selected, scroll;
char* game_browser_v1() {
//  tigrBlitTint(app, app, 0,0, 0,0, _320,_240, tigrRGB(128,128,128));

    enum { ENTRIES = (_240/11)-4 };
    static char *buffer = 0; if(!buffer) { buffer = malloc(65536); /*rescan();*/ }
    if (!numgames) return 0;
    if( scroll < 0 ) scroll = 0;
    for( int i = scroll; i < numgames && i < scroll+ENTRIES; ++i ) {
        const char starred = dbgames[i] >> 8 ? (char)(dbgames[i] >> 8) : ' ';
        sprintf(buffer, "%c %3d.%s%s\n", starred, i+1, i == selected ? " > ":" ", 1+strrchr(games[i], DIR_SEP) );
        window_printxycol(ui, buffer, 1, 3+(i-scroll-1),
            (dbgames[i] & 0x7F) == 0 ? tigrRGB(255,255,255) : // untested
            (dbgames[i] & 0x7F) == 1 ? tigrRGB(64,255,64) :   // ok
            (dbgames[i] & 0x7F) == 2 ? tigrRGB(255,64,64) : tigrRGB(255,192,64) ); // bug:warn
    }

    int up = 0, pg = 0;

    static int UPcnt = 0; UPcnt *= !!window_pressed(app, TK_UP);
    if( window_pressed(app, TK_UP) && (UPcnt++ == 0 || UPcnt > 32) ) {
        up = -1;
    } UPcnt *= !!window_pressed(app, TK_UP);
    static int DNcnt = 0; DNcnt *= !!window_pressed(app, TK_DOWN);
    if( window_pressed(app, TK_DOWN) && (DNcnt++ == 0 || DNcnt > 32) ) {
        up = +1;
    } DNcnt *= !!window_pressed(app, TK_DOWN);
    static int PGUPcnt = 0; PGUPcnt *= !!window_pressed(app, TK_PAGEUP);
    if( window_pressed(app, TK_PAGEUP) && (PGUPcnt++ == 0 || PGUPcnt > 32) ) {
        pg = -1;
    } PGUPcnt *= !!window_pressed(app, TK_PAGEUP);
    static int PGDNcnt = 0; PGDNcnt *= !!window_pressed(app, TK_PAGEDN);
    if( window_pressed(app, TK_PAGEDN) && (PGDNcnt++ == 0 || PGDNcnt > 32) ) {
        pg = +1;
    } PGDNcnt *= !!window_pressed(app, TK_PAGEDN);

    // issue browser
    if( window_trigger(app, TK_LEFT)  ) { for(--up; (selected+up) >= 0 && (dbgames[selected+up]&0xFF) <= 1; --up ) ; }
    if( window_trigger(app, TK_RIGHT) ) { for(++up; (selected+up) < numgames && (dbgames[selected+up]&0xFF) <= 1; ++up ) ; }

    for(;up < 0;++up) {
        --selected;
        if( selected < scroll ) --scroll;
    }
    for(;up > 0;--up) {
        ++selected;
        if( selected >= (scroll+ENTRIES) ) ++scroll;
    }
    for(;pg < 0;++pg) {
        if( selected != scroll ) selected = scroll;
        else scroll -= ENTRIES, selected -= ENTRIES;
    }
    for(;pg > 0;--pg) {
        if( selected != scroll+ENTRIES-1 ) selected = scroll+ENTRIES-1;
        else scroll += ENTRIES, selected += ENTRIES;
    }

    scroll = scroll < 0 ? 0 : scroll >= numgames - ENTRIES ? numgames-ENTRIES-1 : scroll;
    selected = selected < scroll ? scroll : selected >= (scroll + ENTRIES + 1) ? scroll + ENTRIES : selected;
    selected = selected < 0 ? 0 : selected >= numgames ? numgames-1 : selected;


        static int chars[16] = {0}, chars_count = -1;
        #define RESET_INPUTBOX() do { memset(chars, 0, sizeof(int)*16); chars_count = -1; } while(0)
        int any = 0;
        // Grab any chars and add them to our buffer.
        for(;;) {
            int c = tigrReadChar(app);
            if (c == 0) break;
            if( window_pressed(app,TK_CONTROL)) break;
            if( c == 8 ) { RESET_INPUTBOX(); break; } // memset(chars, 0, sizeof(int)*16); chars_count = -1; break; }
            if( c == '\t' && chars_count > 0 ) { any = 1; break; }
            if( c <= 32 ) continue;
            else any = 1;
            chars[ chars_count = min(chars_count+1, 15) ] = c;
        }
        // Print out the character buffer too.
        char tmp[1+16*6], *p = tmp;
        for (int n=0;n<16;n++)
            p = tigrEncodeUTF8(p, chars[n]);
        *p = 0;
        char tmp2[16+16*6] = "Find:"; strcat(tmp2, tmp);
        window_printxycol(ui, tmp2, 3,1, tigrRGB(0,192,255));
        if( any ) {
            static char lowercase[1024];
            for(int i = 0; tmp[i]; ++i) tmp[i] |= 32;
            int found = 0;
            if(!found)
            for( int i = scroll+1; i < numgames; ++i ) {
                if (i < 0) continue;
                for(int j = 0; games[i][j]; ++j) lowercase[j+1] = 0, lowercase[j] = games[i][j] | 32;
                if( strstr(lowercase, tmp) ) {
                    scroll = selected = i;
                    found = 1;
                    break;
                }
            }
            if(!found)
            for( int i = 0; i < scroll; ++i ) {
                for(int j = 0; games[i][j]; ++j) lowercase[j+1] = 0, lowercase[j] = games[i][j] | 32;
                if( strstr(lowercase, tmp) ) {
                    scroll = selected = i;
                    found = 1;
                    break;
                }
            }
        }

    if( window_pressed(app, TK_CONTROL) || window_trigger(app, TK_SPACE) ) {
        int update = 0;
        int starred = dbgames[selected] >> 8;
        int color = dbgames[selected] & 0xFF;
        if( window_trigger(app, TK_SPACE) ) color = (color+1) % 4, update = 1;
        if( window_trigger(app, 'D') ) starred = starred != 'D' ? 'D' : 0, update = 1; // disk error
        if( window_trigger(app, 'T') ) starred = starred != 'T' ? 'T' : 0, update = 1; // tape error
        if( window_trigger(app, 'I') ) starred = starred != 'I' ? 'I' : 0, update = 1; // i/o ports error
        if( window_trigger(app, 'R') ) starred = starred != 'R' ? 'R' : 0, update = 1; // rom/bios error
        if( window_trigger(app, 'E') ) starred = starred != 'E' ? 'E' : 0, update = 1; // emulation error
        if( window_trigger(app, 'Z') ) starred = starred != 'Z' ? 'Z' : 0, update = 1; // zip error
        if( window_trigger(app, 'S') ) starred = starred != 'S' ? 'S' : 0, update = 1; // star
        if( window_trigger(app, '3') ) starred = starred != '3' ? '3' : 0, update = 1; // +3 only error
        if( window_trigger(app, '4') ) starred = starred != '4' ? '4' : 0, update = 1; // 48K only error
        if( window_trigger(app, '1') ) starred = starred != '1' ? '1' : 0, update = 1; // 128K only error
        if( window_trigger(app, '0') ) starred = starred != '0' ? '0' : 0, update = 1; // USR0 only error
        if( window_trigger(app, 'A') ) starred = starred != 'A' ? 'A' : 0, update = 1; // ay/audio error
        if( window_trigger(app, 'V') ) starred = starred != 'V' ? 'V' : 0, update = 1; // video/vram error
        if( window_trigger(app, 'H') ) starred = starred != 'H' ? 'H' : 0, update = 1; // hardware error
        if( window_trigger(app, 'M') ) starred = starred != 'M' ? 'M' : 0, update = 1; // mem/multiload error
        if(update) {
            dbgames[selected] = color + (starred << 8);
            db_set(games[selected], dbgames[selected]);
        }
    }

    if( window_trigger(app, TK_RETURN) ) {
        RESET_INPUTBOX();
        return games[selected];
    }

    return NULL;
}



char* game_browser_v2() {
    // decay to local file browser if no ZXDB is present
    if( !zxdb_loaded() ) return ZX_BROWSER = 1, NULL;

#if 0
    if (!numgames) return 0;
    if( scroll < 0 ) scroll = 0;
#endif

    // handle input
    struct mouse m = mouse();
    int up = window_keyrepeat(app, TK_UP);
    int down = window_keyrepeat(app, TK_DOWN);
    int left = window_keyrepeat(app, TK_LEFT);
    int right = window_keyrepeat(app, TK_RIGHT);
    int page_up = window_keyrepeat(app,TK_PAGEUP);
    int page_down = window_keyrepeat(app,TK_PAGEDN);


    // constants

    const int LINE_HEIGHT = 11;
    const int UPPER_SPACING = 2;
    const int BOTTOM_SPACING = (DEV ? 5 : 3) * LINE_HEIGHT;

    // vars

    static int page = 0;
    static int thumbnails = 0; // 0 text, 3 (3x3), 6 (6x6), 12 (12x12)

    static VAL **list = 0;
    static int list_num = 0;

    // background (text mode only)

    static rgba *background_texture = 0;
    if( thumbnails == 0 )
    if( background_texture ) {
        int factor = 0, f256 = 256/(1<<factor), f192 = 192/(1<<factor);
        for(int i = 0, x = _32, y = _24; i < f192; ++i) {
            memcpy(&ui->pix[x+(y+i)*_320], background_texture + (0+i*f256), f256*4);
        }
    }

    // upper tabs

    ui_at(ui, 11-4-2, UPPER_SPACING);

//  static const char *tab = 0;
    static const char *tabs = "\x15#ABCDEFGHIJKLMNOPQRSTUVWXYZ\x12\x17\x18\x19";

    do_once tab = tabs+2; // 'A'

    for(int i = 0; tabs[i]; ++i) {
        if( (ui_at(ui, ui_x+4+(i&1), ui_y), ui_button(NULL, va("%c%c", (tab && tabs[i] == *tab) ? 5 : 7, tabs[i])) )) {
            if( ui_hover ) {
                /**/ if(tabs[i] == '\x15') ui_notify( "-Resume-" );
                else if(tabs[i] == '\x12') ui_notify( "-Favourites-" );
                else if(tabs[i] == '\x17') ui_notify( "-Browse folder-" );
                else if(tabs[i] == '\x18') ui_notify( "-Search game-" );
                else if(tabs[i] == '\x19') ui_notify( "-View Mode-" );
                else if(tabs[i] ==    '#') ui_notify( "-List other games-" );
                else                       ui_notify( va("-List %c games-", tabs[i]) );
            }
            if( ui_click ) {
                tab = tabs + i;
            }
        }
    }

    if( left )  if(!tab) tab = tabs; else if(*tab-- == '#')    tab = tabs + 28;
    if( right ) if(!tab) tab = tabs; else if(*tab++ == '\x12') tab = tabs + 01;

    static const char *prev = 0;
    if( tab && prev != tab ) {

        if( *tab == '\x15' ) {
            tab = 0;
            //active = 0;
            return NULL;
        }
        else
        if( *tab == '\x18' ) {
            if( zxdb_load(prompt("Game title or ZXDB #ID", "Game title or ZXDB #ID", "0"), 0) ) {
                active = 0;

                tab = 0;
                prev = 0;
                //list = 0;
                //list_num = 0;
                return NULL;
            }
        }
        else
        if( *tab == '\x17' ) {
            extern int cmdkey;
            cmdkey = 'SCAN';

            ZX_BROWSER = 1; // decay to file browser

            tab = 0;
            prev = 0;
            //list = 0;
            //list_num = 0;
            return NULL;
        }
        else
        if( *tab == '\x19' ) {
            int next[] = { [0]=3,[3]=6,[6]=12,[12]=0 };

            thumbnails = next[thumbnails];

            tab = prev;
            prev = prev;
            //list = 0;
            //list_num = 0;
            //return NULL;
        }
        else {
            page = 0;
        }

        // search & sort
        free(list), list = 0;
        if( *tab != '\x12' ) {
            list = map_multifind(&zxdb2, va("%c*", *tab == '#' ? '?' : *tab), &list_num);
        }
        else {
            // bookmarks
            list_num = 0;
            list = realloc(list, 65535 * sizeof(VAL*));
            for( int i = 0; i < 65535; ++i) {
                if( cache_get(i) & 0x0f ) {
                    list[ list_num++ ] = map_find(&zxdb2, va("#%d", i));
                }
            }
            // list = realloc(list, list_num * sizeof(VAL*));
        }
        if( list_num ) qsort(list, list_num, sizeof(VAL*), zxdb_compare_by_name);

        // remove dupes (like aliases)
        for( int i = 1; i < list_num; ++i ) {
            if( *(char**)list[i-1] == *(char**)list[i] ) {
                memmove(list + i - 1, list + i, ( list_num - i ) * sizeof(list[0]));
                --list_num;
            }
        }

        // exclude XXX games
        // exclude For(S)ale,(N)everReleased,Dupes(*),MIA(?) [include (A)vailable,(R)ecovered,(D)enied games]
        // exclude demos 72..78
        for( int i = 0; i < list_num; ++i ) {
            char *zx_id = (char*)*list[i];
            char *years = strchr(zx_id, '|')+1; int zx_id_len = years-zx_id-1;
            char *title = strchr(years, '|')+1; int years_len = title-years-1;
            char *alias = strchr(title, '|')+1; int title_len = alias-title-1;
            char *brand = strchr(alias, '|')+1; int alias_len = brand-alias-1;
            char *avail = strchr(brand, '|')+1; int brand_len = avail-brand-1;
            char *score = strchr(avail, '|')+1; int avail_len = score-avail-1;
            char *genre = strchr(score, '|')+1; int score_len = genre-score-1;
            char *tags_ = strchr(genre, '|')+1; int genre_len = tags_-genre-1;

            if( avail[1] == 'X' || !strchr("ARD", avail[0]) || atoi(genre) >= 72 ) {
                memmove(list + i, list + i + 1, ( list_num - i - 1 ) * sizeof(list[0]));
                --list_num;
                --i;
            }
        }

        prev = tab;
    }

    // main content

    double progress_x = _320 * worker_progress(), progress_y = ui_y + LINE_HEIGHT;
    tigrLine(ui, 0, progress_y+1, progress_x, progress_y+1, ui_00);
    tigrLine(ui, 0, progress_y, progress_x, progress_y, ui_ff);

    if( !tab ) return NULL;

    ui_at(ui, 0, UPPER_SPACING+2*LINE_HEIGHT);

    int ENTRIES_PER_PAGE = (_240-ui_y-BOTTOM_SPACING)/LINE_HEIGHT;

    if( thumbnails == 3 ) ENTRIES_PER_PAGE = 3*3;
    if( thumbnails == 6 ) ENTRIES_PER_PAGE = 6*6;
    if( thumbnails == 12 ) ENTRIES_PER_PAGE = 12*12;

    int NUM_PAGES = list_num / ENTRIES_PER_PAGE;
    int trailing_page = list_num % ENTRIES_PER_PAGE;
    NUM_PAGES -= NUM_PAGES && !trailing_page;

    if( page > NUM_PAGES ) page = NUM_PAGES - 1;
    if( page < 0 ) page = 0;

    if( up || page_up )     if(--page < 0) page = 0;
    if( down || page_down ) if(++page >= NUM_PAGES) page = NUM_PAGES;

    static byte frame4 = 0; frame4 = (frame4 + 1) & 3; // 4-frame anim
    static byte frame8 = 0; frame8 = (frame8 + 1) & 7; // 8-frame anim

    if( list )
    for( int len, cnt = 0, i = page * ENTRIES_PER_PAGE, end = (page + 1) * ENTRIES_PER_PAGE;
        i < end && i < list_num; ++i, ++cnt ) {

        char *zx_id = (char*)*list[i];
        char *years = strchr(zx_id, '|')+1; int zx_id_len = years-zx_id-1;
        char *title = strchr(years, '|')+1; int years_len = title-years-1;
        char *alias = strchr(title, '|')+1; int title_len = alias-title-1;
        char *brand = strchr(alias, '|')+1; int alias_len = brand-alias-1;
        char *avail = strchr(brand, '|')+1; int brand_len = avail-brand-1;
        char *score = strchr(avail, '|')+1; int avail_len = score-avail-1;
        char *genre = strchr(score, '|')+1; int score_len = genre-score-1;
        char *tags_ = strchr(genre, '|')+1; int genre_len = tags_-genre-1;

        // replace title if alias is what we're looking for
        if( *tab == '#' ) {
        if( *alias != '|' && (isdigit(*alias) || ispunct(*alias)) ) title = alias, title_len = alias_len;
        } else {
        if( title[0] != *tab && alias[0] == *tab ) title = alias, title_len = alias_len;
        }

        // replace year if title was never released
        if( years[0] == '9' ) years = "?", years_len = 1; // "9999"

        // replace brand if no brand is given. use 1st author if possible
        if( brand[0] == '|' ) {
            char *next = strchr(zx_id, '\n');
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
        static const char *colors = "\7\2\6\4";
        int dbid = atoi(zx_id);
        int vars = cache_get(dbid);
        int star = (vars >> 0) & 0x0f; assert(star <= 3);
        int flag = (vars >> 4) & 0x0f; assert(flag <= 3);

        // build title and clean it up
        char full_title[128];
        snprintf(full_title, sizeof(full_title), " %.*s (%.*s)(%.*s)\n", title_len, title, years_len, years, brand_len, brand);
        for( int i = 1; full_title[i]; ++i )
            if( i == 1 || full_title[i-1] == '.' )
                full_title[i] = toupper(full_title[i]);

        full_title[0] = colors[flag];

        if( !thumbnails ) {

            ui_label(va("  %3d. ", i+1));

            if( ui_click("-Likes-", va("%c\f", "\x10\x12"[!!star])) )
                star = !star;

            if( ui_click("-Flags-", va("%c%s", colors[flag], flag == 0 || flag == 3 ? "":"")) )
                flag = (flag + 1) % 4;

            cache_set(dbid, (vars & 0xff00) | (flag << 4) | star);

            ui_label(" ");

            ui_monospaced = 0;
            if( ui_button(NULL, full_title) ) {
                
                if( ui_hover ) {
                    if( background_texture ) free(background_texture), background_texture = 0;

                    const byte *data = zxdb_screen_async(va("#%.*s", zx_id_len, zx_id), &len, 0);
                    if( data ) {
                        int ix,iy,in;
                        rgba *bitmap = (rgba*)stbi_load_from_memory(data, len, &ix, &iy, &in, 4);
                        if( bitmap ) {
                            background_texture = ui_resize(bitmap, ix, iy, 256/1, 192/1, 1);
                            stbi_image_free(bitmap);
                        } else {
                            background_texture = thumbnail(data, len, 1, ZXFlashFlag);
                        }
                    }
                }

                if( ui_click ) {
                    zxdb_load(va("#%.*s", zx_id_len, zx_id), 0);

                    active = 0;

#if 0
        //          tab = 0;
                    prev = 0;
                    list = 0;
                    list_num = 0;
#endif
                    return NULL;
                }
            }
        }
        else {
            int t = thumbnails;
            int w = _320/t, h = (_240-16)/t;
            int x = (cnt % t) * w, y = 16 + (cnt / t) * h;
            int clicked = 0;
            int has_thumb = 0;

            int factor = t == 3 ? 1 : t == 6 ? 2 : 3;
            const char *data = zxdb_screen_async(va("#%.*s", zx_id_len, zx_id), &len, factor);
            if( data && len ) {
                // blit
                const rgba *bitmap = (rgba*)data;
                int f256 = 256/(1<<factor), f192 = 192/(1<<factor);
                for(int i = 0; i < f192; ++i) {
                    memcpy(&ui->pix[x+(y+i)*_320], bitmap + (0+i*f256), f256*4);
                }

                has_thumb = 1;
            }

            if( m.x >= x && m.x < (x+w) && m.y >= y && m.y < (y+h) ) {
                ui_rect(ui, x,y, x+w-1,y+h-1);

                ui_at(ui, x+2, y+2);

                if( ui_click(NULL, va("%c\f", "\x10\x12"[!!star])) )
                    star = !star;

                if( ui_click(NULL, va("%c%s", colors[flag], flag == 0 || flag == 3 ? "":"")) )
                    flag = (flag + 1) % 4;

                cache_set(dbid, (vars & 0xff00) | (flag << 4) | star);

                if( m.x <= (x+10) && m.y <= (y+11) ) {
                    ui_notify("-Likes-");
                }
                else
                if( m.x <= (x+10+10-4) && m.y <= (y+11) ) {
                    ui_notify("-Flags-");
                }
                else {
                    mouse_cursor(2);
                    ui_notify(full_title);
                    clicked = m.lb;
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
                    const char *id = va("%.*s\n%s", zx_id_len, zx_id, anims4[ (atoi(zx_id) + frame4) & 3 ] );
                    if( ui_click(id, id) ) clicked = 1;
                }
            }

            if( clicked ) {
                zxdb_load(va("#%.*s", zx_id_len, zx_id), 0);

                active = 0;

#if 0
    //          tab = 0;
                prev = 0;
                list = 0;
                list_num = 0;
#endif
                return NULL;
            }
        }
    }

    return NULL;
}
