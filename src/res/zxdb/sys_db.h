// sqlite utilities,
// - rlyeh, public domain

// header

typedef int (*db_callback)(void *userdata, int argc, char **argv, char **colname);

bool   db_open(const char *dbfile);
bool    db_exec(const char *sql, db_callback cb);
char**  db_query(const char *sql);
char**   db_print(char **list);
bool   db_close();

// impl

#if SQLITE_C

static __thread char **dbrow;
static __thread unsigned dbrowcap, dbrowcnt;
static void dbrow_push(int argc, char **argv) {
    int lens[128] = {0}, len = 0;
    for( int i = 0; i < argc; ++i) len += (lens[i] = argv[i] ? strlen(argv[i]) : 0) + 1;

    if( dbrowcnt >= dbrowcap ) {
        unsigned bak = dbrowcap;
        dbrow = realloc(dbrow, sizeof(char*) * (dbrowcap += 1 + (dbrowcap * 1.5)));
        memset(dbrow + bak, 0, (dbrowcap - bak) * sizeof(char*));
    }
    dbrow[dbrowcnt] = realloc(dbrow[dbrowcnt], len + 1); dbrow[dbrowcnt][0] = '\0';

    if( argc ) {
        char *p = dbrow[dbrowcnt++];
        for( int i = 0; i < argc; ++i) p += sprintf(p, "%.*s%c", lens[i], argv[i], "|\0"[(i+1)==argc]);
    }
}

static __thread sqlite3 *db;

bool db_open(const char *fname) {
    return !db ? sqlite3_open(fname, &db) == SQLITE_OK : false;
}
void db_close_(void) {
    if(db) sqlite3_close(db);
}
bool db_close() {
    return db ? sqlite3_close(db) == SQLITE_OK : false;
}
bool db_exec(const char *sql, db_callback cb) {
    return sqlite3_exec(db, sql, cb, NULL, NULL) == SQLITE_OK;
}

static __thread int DEBUG_HEADERS = 0;
static int db_query_callback(void *userdata, int argc, char **argv, char **colname) {
    if( DEBUG_HEADERS ) DEBUG_HEADERS = 0, dbrow_push(argc, colname);
    return dbrow_push(argc, argv), 0;
}
char** db_query(const char *sql) {
    dbrowcnt = 0;
    DEBUG_HEADERS = sql[0] == '.'; sql += DEBUG_HEADERS;
    if( DEBUG_HEADERS ) puts(sql);
    return db_exec(sql, db_query_callback) ? (dbrow_push(0, NULL), dbrow) : NULL;
}
char** db_print(char **list) { //, FILE *fp);
    if( list ) for(unsigned i = 0; list[i][0]; ++i) fprintf(stdout, "%s\n", list[i]);
    else fprintf(stdout, "(null)\n");
    return list;
}

#endif
