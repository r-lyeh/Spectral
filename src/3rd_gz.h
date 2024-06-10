// gunzip unpacker
// - rlyeh, public domain

char *gunzip(const void *data_, unsigned len, unsigned *outlen_) {
    const unsigned char *data = (const unsigned char *)data_;
    if( data && !memcmp(data, "\x1f\x8b\x08", 3) ) {
        int flags = data[3];

        const unsigned char *ptr = data + 3+1 + 4 + 1 + 1; // header+flags + timestamp + extra flags + os

        // https://www.rfc-editor.org/rfc/rfc1952
        if( flags &  4 ) ptr += ptr[0] << 0 | ptr[1] << 8; // FEXTRA
        if( flags &  8 ) ptr += strlen(ptr) + 1; // FNAME
        if( flags & 16 ) ptr += strlen(ptr) + 1; // FCOMMENT
        if( flags &  2 ) ptr += 2; // FHCRC
        // if( flags &  1 ) ptr += strlen(ptr) + 1; // FTEXT

        const unsigned outlen = *(unsigned *)&data[ len - 4 ];

        unsigned char *out = 0;
        out = realloc(out, outlen);

        mz_stream z = {0};
        z.avail_in = (len - 4 /*crc*/ - 4 /*outlen*/) - (ptr - data);
        z.next_in = ptr;
        z.avail_out = outlen;
        z.next_out = out;

        mz_inflateInit2(&z, -MZ_DEFAULT_WINDOW_BITS);
        int ret = mz_inflate(&z, MZ_SYNC_FLUSH);
        mz_inflateEnd(&z);

        if( ret >= 0 ) {
            if( outlen_ ) *outlen_ = outlen;
            return out;
        }

        // alert("error: cant decompress .gz file (gzip)");
        out = realloc(out, 0);
    }
    return NULL;
}
