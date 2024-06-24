// zip un/packer. based on JUnzip library by Joonas Pihlajamaa (UNLICENSE)
// - rlyeh, public domain.
//
// notes about compression_level:
// - plain integers use DEFLATE. Levels are [0(store)..6(default)..9(max)]
// - compress.c compression flags are also supported. Just use LZMA|5, ULZ|9, LZ4X|3, etc.
// - see zip_put.c for more info.

#ifndef ZIP_H
#define ZIP_H
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct zip zip;

zip* zip_open(const char *file, const char *mode /*r,w,a*/);
zip* zip_open_handle(FILE*fp, const char *mode /*r,w,a*/);

    // only for (w)rite or (a)ppend mode
    bool zip_append_file(zip*, const char *entryname, const char *comment, FILE *in, unsigned compress_level);
    bool zip_append_file_timeinfo(zip*, const char *entryname, const char *comment, FILE *in, unsigned compress_level, struct tm *);
    bool zip_append_mem(zip*, const char *entryname, const char *comment, const void *in, unsigned inlen, unsigned compress_level);
    bool zip_append_mem_timeinfo(zip*, const char *entryname, const char *comment, const void *in, unsigned inlen, unsigned compress_level, struct tm *);

    // only for (r)ead mode
    int zip_find(zip*, const char *entryname); // convert entry to index. returns <0 if not found.
    unsigned zip_count(zip*);
        char*    zip_name(zip*, unsigned index);
        char*    zip_modt(zip*, unsigned index);
        unsigned zip_size(zip*, unsigned index);
        unsigned zip_hash(zip*, unsigned index);
        bool     zip_file(zip*, unsigned index); // is_file? (dir if name ends with '/'; file otherwise)
        bool     zip_test(zip*, unsigned index);
        char*    zip_comment(zip*, unsigned index);
        unsigned zip_codec(zip*, unsigned index);
        unsigned zip_offset(zip*, unsigned index);
        unsigned zip_excess(zip*, unsigned index);
        void*    zip_extract(zip*, unsigned index); // must free() after use
        bool     zip_extract_file(zip*, unsigned index, FILE *out);
        unsigned zip_extract_inplace(zip*, unsigned index, void *out, unsigned outlen_with_excess);

void zip_close(zip*);

#endif // ZIP_H

// -----------------------------------------------------------------------------

#ifdef ZIP_C
//#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#ifdef _WIN32
#include <io.h>       // _chsize_s
#endif

#ifndef REALLOC
#define REALLOC realloc
#endif

#ifndef STRDUP
#define STRDUP  strdup
#endif

#ifndef FPRINTF
#define FPRINTF(...)    ((void)0) // printf for error logging
#endif

#ifndef ERR
#define ERR(NUM, ...)   (FPRINTF(stderr, "" __VA_ARGS__), FPRINTF(stderr, "(%s:%d) %s\n", __FILE__, __LINE__, strerror(errno)), /*fflush(stderr),*/ (NUM)) // (NUM)
#endif

#ifndef COMPRESS
#define COMPRESS(...)   ((unsigned)0)
#endif

#ifndef DECOMPRESS
#define DECOMPRESS(...) ((unsigned)0)
#endif

#ifndef BOUNDS
#define BOUNDS(...)     ((unsigned)0)
#endif

#ifndef EXCESS
#define EXCESS(...)     ((unsigned)0)
#endif

#ifdef COMPRESS_H
    #undef COMPRESS
    #undef DECOMPRESS
    #undef BOUNDS
    #undef EXCESS
    static unsigned COMPRESS(const void *in, unsigned inlen, void *out, unsigned outlen, unsigned flags /*[0..1]*/) {
        return ( flags > 10 ? mem_encode : deflate_encode )(in, inlen, out, outlen, flags);
    }
    static unsigned DECOMPRESS(const void *in, unsigned inlen, void *out, unsigned outlen, unsigned flags) {
        return ( flags > 10 ? mem_decode : deflate_decode )(in, inlen, out, outlen);
    }
    static unsigned BOUNDS(unsigned inlen, unsigned flags) {
        return ( flags > 10 ? mem_bounds : deflate_bounds )(inlen, flags);
    }
    static unsigned EXCESS(unsigned flags) {
        return ( flags > 10 ? mem_excess : deflate_excess )(flags);
    }
#elif defined DEFLATE_H
    #undef COMPRESS
    #undef DECOMPRESS
    #undef BOUNDS
    #undef EXCESS
    static unsigned COMPRESS(const void *in, unsigned inlen, void *out, unsigned outlen, unsigned flags /*[0..1]*/) {
        return deflate_encode(in, inlen, out, outlen, flags);
    }
    static unsigned DECOMPRESS(const void *in, unsigned inlen, void *out, unsigned outlen, unsigned flags) {
        return deflate_decode(in, inlen, out, outlen);
    }
    static unsigned BOUNDS(unsigned inlen, unsigned flags) {
        return deflate_bounds(inlen, flags);
    }
    static unsigned EXCESS(unsigned flags) {
        return deflate_excess(flags);
    }
#endif

#pragma pack(push, 1)

typedef struct {
    uint32_t signature; // 0x02014B50
    uint16_t versionMadeBy; // unsupported
    uint16_t versionNeededToExtract; // unsupported
    uint16_t generalPurposeBitFlag; // unsupported
    uint16_t compressionMethod; // 0-store,8-deflate
    uint16_t lastModFileTime;
    uint16_t lastModFileDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength; // unsupported
    uint16_t fileCommentLength; // unsupported
    uint16_t diskNumberStart; // unsupported
    uint16_t internalFileAttributes; // unsupported
    uint32_t externalFileAttributes; // unsupported
    uint32_t relativeOffsetOflocalHeader;
} JZGlobalFileHeader;

typedef struct {
    uint32_t signature; // 0x06054b50
    uint16_t diskNumber; // unsupported
    uint16_t centralDirectoryDiskNumber; // unsupported
    uint16_t numEntriesThisDisk; // unsupported
    uint16_t numEntries;
    uint32_t centralDirectorySize;
    uint32_t centralDirectoryOffset;
    uint16_t zipCommentLength;
    // Followed by .ZIP file comment (variable size)
} JZEndRecord;

#pragma pack(pop)

// Verifying that structs are correct sized...
typedef int static_assert_sizeof_JZGlobalFileHeader[sizeof(JZGlobalFileHeader) == 46 ? 1:-1];
typedef int static_assert_sizeof_JZEndRecord[sizeof(JZEndRecord) == 22 ? 1:-1];
enum { sizeof_JZLocalFileHeader = 30 };

// Constants
enum { JZ_OK = 0, JZ_ERRNO = -1, JZ_BUFFER_SIZE = 65536 };

// Callback prototype for central reading function. Returns Z_OK while done.
typedef int (*JZRecordCallback)(FILE *fp, int index, JZGlobalFileHeader *header,
    char *filename, void *extra, char *comment, void *user_data);

// Read ZIP file end record. Will move within file. Returns Z_OK, or error code
int jzReadEndRecord(FILE *fp, JZEndRecord *endRecord) {
    long fileSize, readBytes, i;
    JZEndRecord *er = 0;

    if(fseek(fp, 0, SEEK_END)) {
        return ERR(JZ_ERRNO, "Couldn't go to end of zip file!");
    }

    if((fileSize = ftell(fp)) <= sizeof(JZEndRecord)) {
        return ERR(JZ_ERRNO, "Too small file to be a zip!");
    }

    unsigned char jzBuffer[JZ_BUFFER_SIZE]; // maximum zip descriptor size

    readBytes = (fileSize < sizeof(jzBuffer)) ? fileSize : sizeof(jzBuffer);

    if(fseek(fp, fileSize - readBytes, SEEK_SET)) {
        return ERR(JZ_ERRNO, "Cannot seek in zip file!");
    }

    if(fread(jzBuffer, 1, readBytes, fp) < readBytes) {
        return ERR(JZ_ERRNO, "Couldn't read end of zip file!");
    }

    // Naively assume signature can only be found in one place...
    for(i = readBytes - sizeof(JZEndRecord); i >= 0; i--) {
        er = (JZEndRecord *)(jzBuffer + i);
        if(er->signature == 0x06054B50)
            break;
    }

    if(i < 0 || !er) {
        return ERR(JZ_ERRNO, "End record signature not found in zip!");
    }

    memcpy(endRecord, er, sizeof(JZEndRecord));

    JZEndRecord *e = endRecord;
    FPRINTF(stdout, "end)\n\tsignature: 0x%X\n", e->signature ); // 0x06054b50
    FPRINTF(stdout, "\tdiskNumber: %d\n", e->diskNumber ); // unsupported
    FPRINTF(stdout, "\tcentralDirectoryDiskNumber: %d\n", e->centralDirectoryDiskNumber ); // unsupported
    FPRINTF(stdout, "\tnumEntriesThisDisk: %d\n", e->numEntriesThisDisk ); // unsupported
    FPRINTF(stdout, "\tnumEntries: %d\n", e->numEntries );
    FPRINTF(stdout, "\tcentralDirectorySize: %u %#x\n", e->centralDirectorySize, e->centralDirectorySize );
    FPRINTF(stdout, "\tcentralDirectoryOffset: %u %#x\n", e->centralDirectoryOffset, e->centralDirectoryOffset );
    FPRINTF(stdout, "\tzipCommentLength: %d\n---\n", e->zipCommentLength );

    if(endRecord->diskNumber || endRecord->centralDirectoryDiskNumber ||
            endRecord->numEntries != endRecord->numEntriesThisDisk) {
        return ERR(JZ_ERRNO, "Multifile zips not supported!");
    }

    return JZ_OK;
}

// Read ZIP file global directory. Will move within file. Returns Z_OK, or error code
// Callback is called for each record, until callback returns zero
int jzReadCentralDirectory(FILE *fp, JZEndRecord *endRecord, JZRecordCallback callback, void *user_data, void *user_data2) {
    JZGlobalFileHeader fileHeader;

    if(fseek(fp, endRecord->centralDirectoryOffset, SEEK_SET)) {
        return ERR(JZ_ERRNO, "Cannot seek in zip file!");
    }

    for(int i=0; i<endRecord->numEntries; i++) {
        FPRINTF(stdout, "%d)\n@-> %lu %#lx\n", i+1, (unsigned long)ftell(fp), (unsigned long)ftell(fp));
        long offset = ftell(fp); // store current position

        if(fread(&fileHeader, 1, sizeof(JZGlobalFileHeader), fp) < sizeof(JZGlobalFileHeader)) {
            return ERR(JZ_ERRNO, "Couldn't read file header #%d!", i);
        }

        fileHeader.relativeOffsetOflocalHeader += (uintptr_t)user_data2;

        JZGlobalFileHeader *g = &fileHeader, copy = *g;
        FPRINTF(stdout, "\tsignature: %u %#x\n", g->signature, g->signature); // 0x02014B50
        FPRINTF(stdout, "\tversionMadeBy: %u %#x\n", g->versionMadeBy, g->versionMadeBy); // unsupported
        FPRINTF(stdout, "\tversionNeededToExtract: %u %#x\n", g->versionNeededToExtract, g->versionNeededToExtract); // unsupported
        FPRINTF(stdout, "\tgeneralPurposeBitFlag: %u %#x\n", g->generalPurposeBitFlag, g->generalPurposeBitFlag); // unsupported
        FPRINTF(stdout, "\tcompressionMethod: %u %#x\n", g->compressionMethod, g->compressionMethod); // 0-store,8-deflate
        FPRINTF(stdout, "\tlastModFileTime: %u %#x\n", g->lastModFileTime, g->lastModFileTime);
        FPRINTF(stdout, "\tlastModFileDate: %u %#x\n", g->lastModFileDate, g->lastModFileDate);
        FPRINTF(stdout, "\tcrc32: %#x\n", g->crc32);
        FPRINTF(stdout, "\tcompressedSize: %u\n", g->compressedSize);
        FPRINTF(stdout, "\tuncompressedSize: %u\n", g->uncompressedSize);
        FPRINTF(stdout, "\tfileNameLength: %u\n", g->fileNameLength);
        FPRINTF(stdout, "\textraFieldLength: %u\n", g->extraFieldLength); // unsupported
        FPRINTF(stdout, "\tfileCommentLength: %u\n", g->fileCommentLength); // unsupported
        FPRINTF(stdout, "\tdiskNumberStart: %u\n", g->diskNumberStart); // unsupported
        FPRINTF(stdout, "\tinternalFileAttributes: %#x\n", g->internalFileAttributes); // unsupported
        FPRINTF(stdout, "\texternalFileAttributes: %#x\n", g->externalFileAttributes); // unsupported
        FPRINTF(stdout, "\trelativeOffsetOflocalHeader: %u %#x\n", g->relativeOffsetOflocalHeader, g->relativeOffsetOflocalHeader);

        if(fileHeader.signature != 0x02014B50) {
            return ERR(JZ_ERRNO, "Invalid file header signature %#x #%d!", fileHeader.signature, i);
        }

        if(fileHeader.fileNameLength + 1 >= JZ_BUFFER_SIZE) {
            return ERR(JZ_ERRNO, "Too long file name %u #%d!", fileHeader.fileNameLength, i);
        }

        // filename
        char jzFilename[JZ_BUFFER_SIZE/3];
        if(fread(jzFilename, 1, fileHeader.fileNameLength, fp) < fileHeader.fileNameLength) {
            return ERR(JZ_ERRNO, "Couldn't read filename #%d!", i);
        }
        jzFilename[fileHeader.fileNameLength] = '\0'; // NULL terminate

        // extra block
        unsigned char jzExtra[JZ_BUFFER_SIZE/3];
        if(fread(jzExtra, 1, fileHeader.extraFieldLength, fp) < fileHeader.extraFieldLength) {
            return ERR(JZ_ERRNO, "Couldn't read extra block #%d!", i);
        }

        // comment block
        char jzComment[JZ_BUFFER_SIZE/3];
        if(fread(jzComment, 1, fileHeader.fileCommentLength, fp) < fileHeader.fileCommentLength) {
            return ERR(JZ_ERRNO, "Couldn't read comment block #%d!", i);
        }
        jzComment[fileHeader.fileCommentLength] = '\0'; // NULL terminate

        // seek to local file header, then skip file header + filename + extra field length
        if(fseek(fp, fileHeader.relativeOffsetOflocalHeader + sizeof_JZLocalFileHeader - 2 - 2, SEEK_SET)) {
            return ERR(JZ_ERRNO, "Cannot seek in file!");
        }

        if(fread(&fileHeader.fileNameLength, 1, 2, fp) < 2) {
            return ERR(JZ_ERRNO, "Couldn't read local filename #%d!", i);
        }
        if(fread(&fileHeader.extraFieldLength, 1, 2, fp) < 2) {
            return ERR(JZ_ERRNO, "Couldn't read local extrafield #%d!", i);
        }
        if(fseek(fp, fileHeader.relativeOffsetOflocalHeader + sizeof_JZLocalFileHeader + fileHeader.fileNameLength + fileHeader.extraFieldLength, SEEK_SET)) {
            return ERR(JZ_ERRNO, "Cannot seek in file!");
        }

        FPRINTF(stdout, "@-> %lu %#lx\n---\n", (unsigned long)ftell(fp), (unsigned long)ftell(fp));

        if( JZ_OK != callback(fp, i, &fileHeader, jzFilename, jzExtra, jzComment, user_data) )
            break; // keep going while callback returns ok

        fseek(fp, offset, SEEK_SET); // return to position
        fseek(fp, sizeof(JZGlobalFileHeader) + copy.fileNameLength, SEEK_CUR); // skip entry
        fseek(fp, copy.extraFieldLength + copy.fileCommentLength, SEEK_CUR); // skip entry
    }

    return JZ_OK;
}

// Read data from file stream, described by header, to preallocated buffer. Returns Z_OK, or error code
int jzReadData(FILE *fp, JZGlobalFileHeader *header, void *out) {
    if(header->compressionMethod == 0) { // Store - just read it
        if(fread(out, 1, header->uncompressedSize, fp) < header->uncompressedSize || ferror(fp))
            return JZ_ERRNO;
    } else if((header->compressionMethod & 255) == 8) { // Deflate
        uint16_t level = header->compressionMethod >> 8;
        unsigned outlen = header->uncompressedSize;
        unsigned inlen = header->compressedSize;
        void *in = REALLOC(0, inlen + 8); // small excess as some decompressors are really wild with output buffers (lz4x)
        if(in == NULL) return ERR(JZ_ERRNO, "Could not allocate mem for decompress");
        unsigned read = fread(in, 1, inlen, fp);
        if(read != inlen) return ERR(JZ_ERRNO, "Could not read file"); // TODO: more robust read loop
        unsigned ret = DECOMPRESS(in, inlen, out, outlen, level);
        REALLOC(in, 0);
        if(!ret) return ERR(JZ_ERRNO, "Could not decompress");
    } else {
        return JZ_ERRNO;
    }

    return JZ_OK;
}

#define JZHOUR(t) ((t)>>11)
#define JZMINUTE(t) (((t)>>5) & 63)
#define JZSECOND(t) (((t) & 31) * 2)
#define JZTIME(h,m,s) (((h)<<11) + ((m)<<5) + (s)/2)

#define JZYEAR(t) (((t)>>9) + 1980)
#define JZMONTH(t) (((t)>>5) & 15)
#define JZDAY(t) ((t) & 31)
#define JZDATE(y,m,d) ((((y)-1980)<<9) + ((m)<<5) + (d))

// end of junzip.c ---

struct zip {
    FILE *in, *out;
    struct zip_entry {
    JZGlobalFileHeader header;
    char timestamp[40];
    char *filename;
    uint64_t offset;
    void *extra;
    char *comment;
    } *entries;
    unsigned count;
};

uint32_t zip__crc32(uint32_t crc, const void *data, size_t n_bytes) {
    // CRC32 routine is from Björn Samuelsson's public domain implementation at http://home.thep.lu.se/~bjorn/crc/
    static uint32_t table[256] = {0};
    if(!*table) for(uint32_t i = 0; i < 0x100; ++i) {
        uint32_t r = i;
        for(int j = 0; j < 8; ++j) r = (r & 1 ? 0 : (uint32_t)0xEDB88320L) ^ r >> 1;
        table[i] = r ^ (uint32_t)0xFF000000L;
    }
    for(size_t i = 0; i < n_bytes; ++i) {
        crc = table[(uint8_t)crc ^ ((uint8_t*)data)[i]] ^ crc >> 8;
    }
    return crc;
}

int zip__callback(FILE *fp, int idx, JZGlobalFileHeader *header, char *filename, void *extra, char *comment, void *user_data) {
    zip *z = user_data;
    unsigned index = z->count;
    z->entries = REALLOC(z->entries, (++z->count) * sizeof(struct zip_entry) );

    struct zip_entry *e = &z->entries[index];
    e->header = *header;
    e->filename = STRDUP(filename);
    e->offset = ftell(fp);
    e->extra = REALLOC(0, header->extraFieldLength);
    memcpy(e->extra, extra, header->extraFieldLength);
    e->comment = STRDUP(comment);

    snprintf(e->timestamp, sizeof(e->timestamp), "%04d/%02d/%02d %02d:%02d:%02d" "%c" "%04d%02d%02d%02d%02d%02d",
        JZYEAR(header->lastModFileDate), JZMONTH(header->lastModFileDate), JZDAY(header->lastModFileDate),
        JZHOUR(header->lastModFileTime), JZMINUTE(header->lastModFileTime), JZSECOND(header->lastModFileTime),
        '\0', // hidden date in base10
        JZYEAR(header->lastModFileDate), JZMONTH(header->lastModFileDate), JZDAY(header->lastModFileDate),
        JZHOUR(header->lastModFileTime), JZMINUTE(header->lastModFileTime), JZSECOND(header->lastModFileTime)
        );

    return JZ_OK;
}

// zip read

int ZIP_DEBUG = 0;

int zip_find(zip *z, const char *entryname) {
    int zip_debug = ZIP_DEBUG; ZIP_DEBUG = 0;
    if(zip_debug) FPRINTF(stdout, "zip_find(%s)\n", entryname);
    if( z->in ) for( int i = z->count; --i >= 0; ) { // in case of several copies, grab most recent file (last coincidence)
        if(zip_debug) FPRINTF(stdout, "\t%d) %s\n", i, z->entries[i].filename);
        if( 0 == strcmp(entryname, z->entries[i].filename)) return i;
    }
    return -1;
}

bool zip_file(zip *z, unsigned index) { // is_file? (dir if attrib&15 or name ends with '/'; file otherwise)
    if( z->in && index < z->count ) {
        char *name = zip_name(z, index);
        return (name[ strlen(name) ] != '/') && !(z->entries[index].header.externalFileAttributes & 0x10);
    }
    return 0;
}

unsigned zip_count(zip *z) {
    return z->in ? z->count : 0;
}

unsigned zip_hash(zip *z, unsigned index) {
    return z->in && index < z->count ? z->entries[index].header.crc32 : 0;
}

char *zip_modt(zip *z, unsigned index) {
    return z->in && index < z->count ? z->entries[index].timestamp : 0;
}

char *zip_name(zip *z, unsigned index) {
    return z->in && index < z->count ? z->entries[index].filename : NULL;
}

char *zip_comment(zip *z, unsigned index) {
    return z->in && index < z->count ? z->entries[index].comment : NULL;
}

unsigned zip_size(zip *z, unsigned index) {
    return z->in && index < z->count ? z->entries[index].header.uncompressedSize : 0;
}

unsigned zip_offset(zip *z, unsigned index) {
    return z->in && index < z->count ? z->entries[index].offset : 0;
}

unsigned zip_codec(zip *z, unsigned index) {
    if( z->in && index < z->count ) {
        unsigned cm = z->entries[index].header.compressionMethod;
        return cm < 255 ? cm : cm >> 8;
    }
    return 0;
}

unsigned zip_excess(zip *z, unsigned index) {
    if( z->in && index < z->count ) {
        unsigned level = z->entries[index].header.compressionMethod;
        unsigned flags = level >> 8;
        return EXCESS(flags);
    }
    return 0;
}

unsigned zip_extract_inplace(zip *z, unsigned index, void *out, unsigned outlen) {
    if( z->in && index < z->count ) {
        JZGlobalFileHeader *header = &(z->entries[index].header);
        if( outlen >= header->uncompressedSize ) {
            fseek(z->in, z->entries[index].offset, SEEK_SET);
            int ret = jzReadData(z->in, header, (char*)out);
            return ret == JZ_OK ? header->uncompressedSize : 0;
        }
    }
    return 0;
}

void *zip_extract(zip *z, unsigned index) { // must free()
    if( z->in && index < z->count ) {
        unsigned outlen = (unsigned)z->entries[index].header.uncompressedSize;
        char *out = (char*)REALLOC(0, outlen + 1 + zip_excess(z, index));
        unsigned ret = zip_extract_inplace(z, index, out, outlen);
        return ret ? (out[outlen] = '\0', out) : (REALLOC(out, 0), out = 0);
    }
    return NULL;
}

bool zip_extract_file(zip *z, unsigned index, FILE *out) {
    void *data = zip_extract(z, index);
    if( !data ) return false;
    unsigned datalen = (unsigned)z->entries[index].header.uncompressedSize;
    bool ok = fwrite(data, 1, datalen, out) == datalen;
    REALLOC( data, 0 );
    return ok;
}

bool zip_test(zip *z, unsigned index) {
    void *ret = zip_extract(z, index);
    bool ok = !!ret;
    REALLOC(ret, 0);
    return ok;
}

// zip append/write

bool zip_append_file(zip *z, const char *entryname, const char *comment, FILE *in, unsigned compress_level) {
    if( !entryname ) return ERR(false, "No filename provided");

    struct stat st;
    struct tm *timeinfo;
    stat(entryname, &st);
    timeinfo = localtime(&st.st_mtime);

    return zip_append_file_timeinfo(z, entryname, comment, in, compress_level, timeinfo);
}

bool zip_append_file_timeinfo(zip *z, const char *entryname, const char *comment, FILE *in, unsigned compress_level, struct tm* timeinfo) {
    if( !in ) return ERR(false, "No input file provided");
    if( !entryname ) return ERR(false, "No filename provided");
    if( !timeinfo ) return ERR(false, "No timeinfo provided");

    // @fixme: calc whole crc contents
    uint32_t crc = 0;
    unsigned char buf[4096];
    while(!feof(in) && !ferror(in)) crc = zip__crc32(crc, buf, fread(buf, 1, sizeof(buf), in));
    if(ferror(in)) return ERR(false, "Error while calculating CRC, skipping store.");

    unsigned index = z->count;
    z->entries = REALLOC(z->entries, (++z->count) * sizeof(struct zip_entry));
    if(z->entries == NULL) return ERR(false, "Failed to allocate new entry!");

    struct zip_entry *e = &z->entries[index], zero = {0};
    *e = zero;
    e->filename = STRDUP(entryname);
    e->comment = comment ? STRDUP(comment) : 0;

    e->header.signature = 0x02014B50;
    e->header.versionMadeBy = 10; // random stuff
    e->header.versionNeededToExtract = 10;
    e->header.generalPurposeBitFlag = 0;
    e->header.lastModFileTime = JZTIME(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    e->header.lastModFileDate = JZDATE(timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday);
    e->header.crc32 = crc;
    e->header.uncompressedSize = ftell(in);
    e->header.fileNameLength = strlen(entryname);
    e->header.extraFieldLength = 0;
    e->header.fileCommentLength = comment ? strlen(comment) : 0;
    e->header.diskNumberStart = 0;
    e->header.internalFileAttributes = 0;
    e->header.externalFileAttributes = 0x20; // whatever this is
    e->header.relativeOffsetOflocalHeader = ftell(z->out);

#if defined _MSC_VER || (defined __TINYC__ && defined _WIN32)
    static __declspec(thread)
#else
    static __thread
#endif
    void* comp = 0, *data = 0;

    // if(comp) comp = REALLOC(comp, 1); // re-entry optimization: hopefully the allocator will optimize this out (N>1-byte)
    // if(data) data = REALLOC(data, 1); // re-entry optimization: hopefully the allocator will optimize this out (N>1-byte)

    if(!compress_level) goto dont_compress;

    // Read whole file and and use compress(). Simple but won't handle GB files well.
    unsigned dataSize = e->header.uncompressedSize, compSize = BOUNDS(e->header.uncompressedSize, compress_level);

    comp = REALLOC(comp, compSize);
    if(comp == NULL) goto cant_compress;

    data = REALLOC(data, dataSize + 8); // small excess as some compressors are really wild when reading from buffers (lz4x)
    if(data == NULL) goto cant_compress; else memset((char*)data + dataSize, 0, 8);

    fseek(in, 0, SEEK_SET); // rewind
    size_t bytes = fread(data, 1, dataSize, in);
    if(bytes != dataSize) {
        return ERR(false, "Failed to read file in full (%lu vs. %ld bytes)", (unsigned long)bytes, dataSize);
    }

    compSize = COMPRESS(data, (unsigned)dataSize, comp, (unsigned)compSize, compress_level);
    if(!compSize) goto cant_compress;
    if(compSize >= (dataSize * 0.98) ) goto dont_compress;

    uint16_t cl = 8 | (compress_level > 10 ? compress_level << 8 : 0);
    e->header.compressedSize = compSize;
    e->header.compressionMethod = cl;
    goto common;

cant_compress:
dont_compress:;
    e->header.compressedSize = ftell(in);
    e->header.compressionMethod = 0; // store method

common:;
    // write local header
    uint32_t signature = 0x04034B50;
    fwrite(&signature, 1, sizeof(signature), z->out);
    fwrite(&(e->header.versionNeededToExtract), 1, sizeof_JZLocalFileHeader - sizeof(signature), z->out);
    // write filename
    fwrite(entryname, 1, strlen(entryname), z->out);
    // write comment
    // if( comment ) fwrite(comment, 1, strlen(comment), z->out);

    if(e->header.compressionMethod) {
        // store compressed blob
        fwrite(comp, compSize, 1, z->out);
    } else {
        // store uncompressed blob
        fseek(in, 0, SEEK_SET);
        while(!feof(in) && !ferror(in)) {
            size_t bytes = fread(buf, 1, sizeof(buf), in);
            fwrite(buf, 1, bytes, z->out);
        }
    }

//  REALLOC(comp, 0); // see re-entry optimization above
//  REALLOC(data, 0); // see re-entry optimization above
    return true;
}

bool zip_append_mem(zip *z, const char *entryname, const char *comment, const void *in, unsigned inlen, unsigned compress_level) {
    if( !entryname ) return ERR(false, "No filename provided");

    struct stat st;
    struct tm *timeinfo;
    stat(entryname, &st);
    timeinfo = localtime(&st.st_mtime);

    return zip_append_mem_timeinfo(z, entryname, comment, in, inlen, compress_level, timeinfo);
}

bool zip_append_mem_timeinfo(zip *z, const char *entryname, const char *comment, const void *in, unsigned inlen, unsigned compress_level, struct tm* timeinfo) {
    if( !in ) return ERR(false, "No input file provided");
    if( !entryname ) return ERR(false, "No filename provided");
    if( !timeinfo ) return ERR(false, "No timeinfo provided");

    uint32_t crc = zip__crc32(0, in, inlen);

    unsigned index = z->count;
    z->entries = REALLOC(z->entries, (++z->count) * sizeof(struct zip_entry));
    if(z->entries == NULL) return ERR(false, "Failed to allocate new entry!");

    struct zip_entry *e = &z->entries[index], zero = {0};
    *e = zero;
    e->filename = STRDUP(entryname);
    e->comment = comment ? STRDUP(comment) : 0;

    e->header.signature = 0x02014B50;
    e->header.versionMadeBy = 10; // random stuff
    e->header.versionNeededToExtract = 10;
    e->header.generalPurposeBitFlag = 0;
    e->header.lastModFileTime = JZTIME(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    e->header.lastModFileDate = JZDATE(timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday);
    e->header.crc32 = crc;
    e->header.uncompressedSize = inlen;
    e->header.fileNameLength = strlen(entryname);
    e->header.extraFieldLength = 0;
    e->header.fileCommentLength = comment ? strlen(comment) : 0;
    e->header.diskNumberStart = 0;
    e->header.internalFileAttributes = 0;
    e->header.externalFileAttributes = 0x20; // whatever this is
    e->header.relativeOffsetOflocalHeader = ftell(z->out);

#if defined _MSC_VER || (defined __TINYC__ && defined _WIN32)
    static __declspec(thread)
#else
    static __thread
#endif
    void* comp = 0, *data = 0;

    // if(comp) comp = REALLOC(comp, 1); // re-entry optimization: hopefully the allocator will optimize this out (N>1-byte)
    // if(data) data = REALLOC(data, 1); // re-entry optimization: hopefully the allocator will optimize this out (N>1-byte)

    if(!compress_level) goto dont_compress;

    // Read whole file and and use compress(). Simple but won't handle GB files well.
    unsigned dataSize = e->header.uncompressedSize, compSize = BOUNDS(e->header.uncompressedSize, compress_level);

    comp = REALLOC(comp, compSize);
    if(comp == NULL) goto cant_compress;

    data = REALLOC(data, dataSize + 8); // small excess as some compressors are really wild when reading from buffers (lz4x)
    if(data == NULL) goto cant_compress; else memset((char*)data + dataSize, 0, 8);

    size_t bytes = inlen;
    memcpy(data, in, inlen);
    if(bytes != dataSize) {
        return ERR(false, "Failed to read file in full (%lu vs. %ld bytes)", (unsigned long)bytes, dataSize);
    }

    compSize = COMPRESS(data, (unsigned)dataSize, comp, (unsigned)compSize, compress_level);
    if(!compSize) goto cant_compress;
    if(compSize >= (dataSize * 0.98) ) goto dont_compress;

    uint16_t cl = 8 | (compress_level > 10 ? compress_level << 8 : 0);
    e->header.compressedSize = compSize;
    e->header.compressionMethod = cl;
    goto common;

cant_compress:
dont_compress:;
    e->header.compressedSize = inlen;
    e->header.compressionMethod = 0; // store method

common:;
    // write local header
    uint32_t signature = 0x04034B50;
    fwrite(&signature, 1, sizeof(signature), z->out);
    fwrite(&(e->header.versionNeededToExtract), 1, sizeof_JZLocalFileHeader - sizeof(signature), z->out);
    // write filename
    fwrite(entryname, 1, strlen(entryname), z->out);
    // write comment
    // if( comment ) fwrite(comment, 1, strlen(comment), z->out);

    if(e->header.compressionMethod) {
        // store compressed blob
        fwrite(comp, compSize, 1, z->out);
    } else {
        // store uncompressed blob
        fwrite(in, 1, inlen, z->out);
    }

//  REALLOC(comp, 0); // see re-entry optimization above
//  REALLOC(data, 0); // see re-entry optimization above
    return true;
}

// zip common

#if 1
#       define zip_lockfile(f)   (void)(f)
#       define zip_unlockfile(f) (void)(f)
#else
#   if (defined(__TINYC__) && defined(_WIN32))
#       define zip_lockfile(f)   (void)(f)
#       define zip_unlockfile(f) (void)(f)
#   elif defined _MSC_VER
#       define zip_lockfile(f)   _lock_file(f)
#       define zip_unlockfile(f) _unlock_file(f)
#   else
#       define zip_lockfile(f)   flockfile(f)
#       define zip_unlockfile(f) funlockfile(f)
#   endif
#endif

zip* zip_open_handle(FILE *fp, const char *mode) {
    if( !fp ) return ERR(NULL, "cannot open file for %s mode", mode);
    zip zero = {0}, *z = (zip*)REALLOC(0, sizeof(zip));
    if( !z ) return ERR(NULL, "out of mem"); else *z = zero;
    if( mode[0] == 'w' ) {
        zip_lockfile(z->out = fp);
        return z;
    }
    if( mode[0] == 'r' || mode[0] == 'a' ) {
        zip_lockfile(z->in = fp);

        unsigned long long seekcur = ftell(z->in);

        JZEndRecord jzEndRecord = {0};
        if(jzReadEndRecord(fp, &jzEndRecord) != JZ_OK) {
            REALLOC(z, 0);
            return ERR(NULL, "Couldn't read ZIP file end record.");
        }

        jzEndRecord.centralDirectoryOffset += seekcur;

        if(jzReadCentralDirectory(fp, &jzEndRecord, zip__callback, z, (void*)(uintptr_t)seekcur ) != JZ_OK) {
            REALLOC(z, 0);
            return ERR(NULL, "Couldn't read ZIP file central directory.");
        }
        if( mode[0] == 'a' ) {

            // resize (by truncation)
            size_t resize = jzEndRecord.centralDirectoryOffset;
            int fd = fileno(fp);
            if( fd != -1 ) {
                #ifdef _WIN32
                    int ok = 0 == _chsize_s( fd, resize );
                #else
                    int ok = 0 == ftruncate( fd, (off_t)resize );
                #endif
                fflush(fp);
                fseek( fp, 0L, SEEK_END );
            }

            z->out = z->in;
            z->in = NULL;
        }
        return z;
    }
    REALLOC(z, 0);
    return ERR(NULL, "Unknown open mode %s", mode);
}

zip* zip_open(const char *file, const char *mode /*r,w,a*/) {
    struct stat buffer;
    int exists = (stat(file, &buffer) == 0);
    if( mode[0] == 'a' && !exists ) mode = "wb";
    FILE *fp = fopen(file, mode[0] == 'w' ? "wb" : mode[0] == 'a' ? "a+b" : "rb");
    if (!fp) return NULL;
    if (mode[0] == 'a') fseek(fp, 0L, SEEK_SET);
    zip *z = zip_open_handle(fp, mode);
    if (!z) return fclose(fp), NULL;
    return z;
}

void zip_close(zip* z) {
    if( z->out && z->count ) {
        // prepare end record
        JZEndRecord end = {0};
        end.signature = 0x06054b50;
        end.diskNumber = 0;
        end.centralDirectoryDiskNumber = 0;
        end.numEntriesThisDisk = z->count;
        end.numEntries = z->count;
        end.centralDirectoryOffset = ftell(z->out);
        // flush global directory: global file+filename each
        for(unsigned i = 0; i < z->count; i++) {
            struct zip_entry *h = &z->entries[i];
            JZGlobalFileHeader *g = &h->header;
            fwrite(g, 1, sizeof(JZGlobalFileHeader), z->out);
            fwrite(h->filename, 1, g->fileNameLength, z->out);
            fwrite(h->extra, 1, g->extraFieldLength, z->out);
            fwrite(h->comment, 1, g->fileCommentLength, z->out);
        }
        end.centralDirectorySize = ftell(z->out) - end.centralDirectoryOffset;
        end.zipCommentLength = 0;

        // flush end record
        fwrite(&end, 1, sizeof(end), z->out);
    }
    if( z->out ) zip_unlockfile(z->out), fclose(z->out);
    if( z->in ) zip_unlockfile(z->in), fclose(z->in);
    // clean up
    for(unsigned i = 0; i < z->count; ++i ) {
        REALLOC(z->entries[i].filename, 0);
        if(z->entries[i].extra)   REALLOC(z->entries[i].extra, 0);
        if(z->entries[i].comment) REALLOC(z->entries[i].comment, 0);
    }
    if(z->entries) REALLOC(z->entries, 0);
    zip zero = {0}; *z = zero; REALLOC(z, 0);
}

#endif // ZIP_C
