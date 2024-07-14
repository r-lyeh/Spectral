#pragma once

#define deflate mz_deflate
#define deflateInit2 mz_deflateInit2
#define inflate mz_inflate
#define inflateInit2 mz_inflateInit2
#define z_stream mz_stream

#define Z_ADLER32_INIT MZ_ADLER32_INIT
#define Z_DEFAULT_STRATEGY MZ_DEFAULT_STRATEGY
#define Z_DEFAULT_WINDOW_BITS MZ_DEFAULT_WINDOW_BITS
#define Z_DEFLATED MZ_DEFLATED
#define Z_EMPTY MZ_EMPTY
#define Z_FINISH MZ_FINISH
#define Z_NO_FLUSH MZ_NO_FLUSH
#define Z_NULL MZ_NULL
#define Z_OK MZ_OK
#define Z_RLE MZ_RLE
#define Z_STREAM_END MZ_STREAM_END
#define Z_STREAM_ERROR MZ_STREAM_ERROR

#ifndef MINIZ_EXPORT
#define MINIZ_EXPORT
#endif

#ifdef MINIZ_NO_MALLOC
#define MZ_MALLOC(x) NULL
#define MZ_FREE(x) (void)x, ((void)0)
#define MZ_REALLOC(p, x) NULL
#else
#define MZ_MALLOC(x) malloc(x)
#define MZ_FREE(x) free(x)
#define MZ_REALLOC(p, x) realloc(p, x)
#endif

/* Works around MSVC's spammy "warning C4127: conditional expression is constant" message. */
#ifdef _MSC_VER
#define MZ_MACRO_END while (0, 0)
#else
#define MZ_MACRO_END while (0)
#endif

typedef unsigned char mz_uint8;
typedef signed short mz_int16;
typedef unsigned short mz_uint16;
typedef unsigned int mz_uint32;
typedef unsigned int mz_uint;
typedef int64_t mz_int64;
typedef uint64_t mz_uint64;
typedef int mz_bool;
typedef unsigned long mz_ulong;

#define MZ_FALSE (0)
#define MZ_TRUE (1)

/* Initializes the decompressor to its initial state. */
#define tinfl_init(r)     \
    do                    \
    {                     \
        (r)->m_state = 0; \
    }                     \
    MZ_MACRO_END
#define tinfl_get_adler32(r) (r)->m_check_adler32

/* Window bits */
#define MZ_DEFAULT_WINDOW_BITS 15

/* Max size of LZ dictionary. */
#define TINFL_LZ_DICT_SIZE 32768

/* Flush values. For typical usage you only need MZ_NO_FLUSH and MZ_FINISH. The other values are for advanced use (refer to the zlib docs). */
enum
{
    MZ_NO_FLUSH = 0,
    MZ_PARTIAL_FLUSH = 1,
    MZ_SYNC_FLUSH = 2,
    MZ_FULL_FLUSH = 3,
    MZ_FINISH = 4,
    MZ_BLOCK = 5
};

/* Return status codes. MZ_PARAM_ERROR is non-standard. */
enum
{
    MZ_OK = 0,
    MZ_STREAM_END = 1,
    MZ_NEED_DICT = 2,
    MZ_ERRNO = -1,
    MZ_STREAM_ERROR = -2,
    MZ_DATA_ERROR = -3,
    MZ_MEM_ERROR = -4,
    MZ_BUF_ERROR = -5,
    MZ_VERSION_ERROR = -6,
    MZ_PARAM_ERROR = -10000
};

enum
{
    MZ_DEFAULT_STRATEGY = 0,
    MZ_FILTERED = 1,
    MZ_HUFFMAN_ONLY = 2,
    MZ_RLE = 3,
    MZ_FIXED = 4
};

/* Compression levels: 0-9 are the standard zlib-style levels, 10 is best possible compression (not zlib compatible, and may be very slow), MZ_DEFAULT_COMPRESSION=MZ_DEFAULT_LEVEL. */
enum
{
    MZ_NO_COMPRESSION = 0,
    MZ_BEST_SPEED = 1,
    MZ_BEST_COMPRESSION = 9,
    MZ_UBER_COMPRESSION = 10,
    MZ_DEFAULT_LEVEL = 6,
    MZ_DEFAULT_COMPRESSION = -1
};

#define MZ_ADLER32_INIT 1
#define MZ_DEFLATED 8
#define MZ_DEFAULT_WINDOW_BITS 15

typedef void *(*mz_alloc_func)(void *opaque, size_t items, size_t size);
typedef void (*mz_free_func)(void *opaque, void *address);
typedef void *(*mz_realloc_func)(void *opaque, void *address, size_t items, size_t size);

MINIZ_EXPORT void *miniz_def_alloc_func(void *opaque, size_t items, size_t size)
{
    (void)opaque, (void)items, (void)size;
    return MZ_MALLOC(items * size);
}
MINIZ_EXPORT void miniz_def_free_func(void *opaque, void *address)
{
    (void)opaque, (void)address;
    MZ_FREE(address);
}
MINIZ_EXPORT void *miniz_def_realloc_func(void *opaque, void *address, size_t items, size_t size)
{
    (void)opaque, (void)address, (void)items, (void)size;
    return MZ_REALLOC(address, items * size);
}



struct mz_internal_state;

/* Compression/decompression stream struct. */
typedef struct mz_stream_s
{
    const unsigned char *next_in; /* pointer to next byte to read */
    unsigned int avail_in;        /* number of bytes available at next_in */
    mz_ulong total_in;            /* total number of bytes consumed so far */

    unsigned char *next_out; /* pointer to next byte to write */
    unsigned int avail_out;  /* number of bytes that can be written to next_out */
    mz_ulong total_out;      /* total number of bytes produced so far */

    char *msg;                       /* error msg (unused) */
    struct mz_internal_state *state; /* internal state, allocated by zalloc/zfree */

    mz_alloc_func zalloc; /* optional heap allocation function (defaults to malloc) */
    mz_free_func zfree;   /* optional heap free function (defaults to free) */
    void *opaque;         /* heap alloc function user pointer */

    int data_type;     /* data_type (unused) */
    mz_ulong adler;    /* adler32 of the source or uncompressed data */
    mz_ulong reserved; /* not used */
} mz_stream;

typedef mz_stream *mz_streamp;













typedef struct
{
    tinfl_decompressor m_decomp;
    mz_uint m_dict_ofs, m_dict_avail, m_first_call, m_has_flushed;
    int m_window_bits;
    mz_uint8 m_dict[TINFL_LZ_DICT_SIZE];
    tinfl_status m_last_status;
} inflate_state;

int mz_inflateInit2(mz_streamp pStream, int window_bits)
{
    inflate_state *pDecomp;
    if (!pStream)
        return MZ_STREAM_ERROR;
    if ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS))
        return MZ_PARAM_ERROR;

    pStream->data_type = 0;
    pStream->adler = 0;
    pStream->msg = NULL;
    pStream->total_in = 0;
    pStream->total_out = 0;
    pStream->reserved = 0;
    if (!pStream->zalloc)
        pStream->zalloc = miniz_def_alloc_func;
    if (!pStream->zfree)
        pStream->zfree = miniz_def_free_func;

    pDecomp = (inflate_state *)pStream->zalloc(pStream->opaque, 1, sizeof(inflate_state));
    if (!pDecomp)
        return MZ_MEM_ERROR;

    pStream->state = (struct mz_internal_state *)pDecomp;

    tinfl_init(&pDecomp->m_decomp);
    pDecomp->m_dict_ofs = 0;
    pDecomp->m_dict_avail = 0;
    pDecomp->m_last_status = TINFL_STATUS_NEEDS_MORE_INPUT;
    pDecomp->m_first_call = 1;
    pDecomp->m_has_flushed = 0;
    pDecomp->m_window_bits = window_bits;

    return MZ_OK;
}

int mz_inflateInit(mz_streamp pStream)
{
    return mz_inflateInit2(pStream, MZ_DEFAULT_WINDOW_BITS);
}

int mz_inflateReset(mz_streamp pStream)
{
    inflate_state *pDecomp;
    if (!pStream)
        return MZ_STREAM_ERROR;

    pStream->data_type = 0;
    pStream->adler = 0;
    pStream->msg = NULL;
    pStream->total_in = 0;
    pStream->total_out = 0;
    pStream->reserved = 0;

    pDecomp = (inflate_state *)pStream->state;

    tinfl_init(&pDecomp->m_decomp);
    pDecomp->m_dict_ofs = 0;
    pDecomp->m_dict_avail = 0;
    pDecomp->m_last_status = TINFL_STATUS_NEEDS_MORE_INPUT;
    pDecomp->m_first_call = 1;
    pDecomp->m_has_flushed = 0;
    /* pDecomp->m_window_bits = window_bits */;

    return MZ_OK;
}

int mz_inflate(mz_streamp pStream, int flush)
{
    inflate_state *pState;
    mz_uint n, first_call, decomp_flags = TINFL_FLAG_COMPUTE_ADLER32;
    size_t in_bytes, out_bytes, orig_avail_in;
    tinfl_status status;

    if ((!pStream) || (!pStream->state))
        return MZ_STREAM_ERROR;
    if (flush == MZ_PARTIAL_FLUSH)
        flush = MZ_SYNC_FLUSH;
    if ((flush) && (flush != MZ_SYNC_FLUSH) && (flush != MZ_FINISH))
        return MZ_STREAM_ERROR;

    pState = (inflate_state *)pStream->state;
    if (pState->m_window_bits > 0)
        decomp_flags |= TINFL_FLAG_PARSE_ZLIB_HEADER;
    orig_avail_in = pStream->avail_in;

    first_call = pState->m_first_call;
    pState->m_first_call = 0;
    if (pState->m_last_status < 0)
        return MZ_DATA_ERROR;

    if (pState->m_has_flushed && (flush != MZ_FINISH))
        return MZ_STREAM_ERROR;
    pState->m_has_flushed |= (flush == MZ_FINISH);

    if ((flush == MZ_FINISH) && (first_call))
    {
        /* MZ_FINISH on the first call implies that the input and output buffers are large enough to hold the entire compressed/decompressed file. */
        decomp_flags |= TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
        in_bytes = pStream->avail_in;
        out_bytes = pStream->avail_out;
        status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pStream->next_out, pStream->next_out, &out_bytes, decomp_flags);
        pState->m_last_status = status;
        pStream->next_in += (mz_uint)in_bytes;
        pStream->avail_in -= (mz_uint)in_bytes;
        pStream->total_in += (mz_uint)in_bytes;
        pStream->adler = tinfl_get_adler32(&pState->m_decomp);
        pStream->next_out += (mz_uint)out_bytes;
        pStream->avail_out -= (mz_uint)out_bytes;
        pStream->total_out += (mz_uint)out_bytes;

        if (status < 0)
            return MZ_DATA_ERROR;
        else if (status != TINFL_STATUS_DONE)
        {
            pState->m_last_status = TINFL_STATUS_FAILED;
            return MZ_BUF_ERROR;
        }
        return MZ_STREAM_END;
    }
    /* flush != MZ_FINISH then we must assume there's more input. */
    if (flush != MZ_FINISH)
        decomp_flags |= TINFL_FLAG_HAS_MORE_INPUT;

    if (pState->m_dict_avail)
    {
        n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
        memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
        pStream->next_out += n;
        pStream->avail_out -= n;
        pStream->total_out += n;
        pState->m_dict_avail -= n;
        pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);
        return ((pState->m_last_status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
    }

    for (;;)
    {
        in_bytes = pStream->avail_in;
        out_bytes = TINFL_LZ_DICT_SIZE - pState->m_dict_ofs;

        status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, &out_bytes, decomp_flags);
        pState->m_last_status = status;

        pStream->next_in += (mz_uint)in_bytes;
        pStream->avail_in -= (mz_uint)in_bytes;
        pStream->total_in += (mz_uint)in_bytes;
        pStream->adler = tinfl_get_adler32(&pState->m_decomp);

        pState->m_dict_avail = (mz_uint)out_bytes;

        n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
        memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
        pStream->next_out += n;
        pStream->avail_out -= n;
        pStream->total_out += n;
        pState->m_dict_avail -= n;
        pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);

        if (status < 0)
            return MZ_DATA_ERROR; /* Stream is corrupted (there could be some uncompressed data left in the output dictionary - oh well). */
        else if ((status == TINFL_STATUS_NEEDS_MORE_INPUT) && (!orig_avail_in))
            return MZ_BUF_ERROR; /* Signal caller that we can't make forward progress without supplying more input or by setting flush to MZ_FINISH. */
        else if (flush == MZ_FINISH)
        {
            /* The output buffer MUST be large to hold the remaining uncompressed data when flush==MZ_FINISH. */
            if (status == TINFL_STATUS_DONE)
                return pState->m_dict_avail ? MZ_BUF_ERROR : MZ_STREAM_END;
            /* status here must be TINFL_STATUS_HAS_MORE_OUTPUT, which means there's at least 1 more byte on the way. If there's no more room left in the output buffer then something is wrong. */
            else if (!pStream->avail_out)
                return MZ_BUF_ERROR;
        }
        else if ((status == TINFL_STATUS_DONE) || (!pStream->avail_in) || (!pStream->avail_out) || (pState->m_dict_avail))
            break;
    }

    return ((status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
}

int mz_inflateEnd(mz_streamp pStream)
{
    if (!pStream)
        return MZ_STREAM_ERROR;
    if (pStream->state)
    {
        pStream->zfree(pStream->opaque, pStream->state);
        pStream->state = NULL;
    }
    return MZ_OK;
}
int mz_uncompress2(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong *pSource_len)
{
    mz_stream stream;
    int status;
    memset(&stream, 0, sizeof(stream));

    /* In case mz_ulong is 64-bits (argh I hate longs). */
    if ((mz_uint64)(*pSource_len | *pDest_len) > 0xFFFFFFFFU)
        return MZ_PARAM_ERROR;

    stream.next_in = pSource;
    stream.avail_in = (mz_uint32)*pSource_len;
    stream.next_out = pDest;
    stream.avail_out = (mz_uint32)*pDest_len;

    status = mz_inflateInit(&stream);
    if (status != MZ_OK)
        return status;

    status = mz_inflate(&stream, MZ_FINISH);
    *pSource_len = *pSource_len - stream.avail_in;
    if (status != MZ_STREAM_END)
    {
        mz_inflateEnd(&stream);
        return ((status == MZ_BUF_ERROR) && (!stream.avail_in)) ? MZ_DATA_ERROR : status;
    }
    *pDest_len = stream.total_out;

    return mz_inflateEnd(&stream);
}

int mz_uncompress(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len)
{
    return mz_uncompress2(pDest, pDest_len, pSource, &source_len);
}

// ----------------------------------------------------------------------------

static const mz_uint s_tdefl_num_probes[11] = { 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 };

mz_uint32 tdefl_get_adler32(tdefl_compressor *d)
{
    return d->m_adler32;
}

/* level may actually range from [0,10] (10 is a "hidden" max level, where we want a bit more compression and it's fine if throughput to fall off a cliff on some files). */
mz_uint tdefl_create_comp_flags_from_zip_params(int level, int window_bits, int strategy)
{
    mz_uint comp_flags = s_tdefl_num_probes[(level >= 0) ? MZ_MIN(10, level) : MZ_DEFAULT_LEVEL] | ((level <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
    if (window_bits > 0)
        comp_flags |= TDEFL_WRITE_ZLIB_HEADER;

    if (!level)
        comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
    else if (strategy == MZ_FILTERED)
        comp_flags |= TDEFL_FILTER_MATCHES;
    else if (strategy == MZ_HUFFMAN_ONLY)
        comp_flags &= ~TDEFL_MAX_PROBES_MASK;
    else if (strategy == MZ_FIXED)
        comp_flags |= TDEFL_FORCE_ALL_STATIC_BLOCKS;
    else if (strategy == MZ_RLE)
        comp_flags |= TDEFL_RLE_MATCHES;

    return comp_flags;
}

int mz_deflateInit2(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy);
int mz_deflateEnd(mz_streamp pStream);

int mz_deflateInit(mz_streamp pStream, int level)
{
    return mz_deflateInit2(pStream, level, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
}

int mz_deflateInit2(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy)
{
    tdefl_compressor *pComp;
    mz_uint comp_flags = TDEFL_COMPUTE_ADLER32 | tdefl_create_comp_flags_from_zip_params(level, window_bits, strategy);

    if (!pStream)
        return MZ_STREAM_ERROR;
    if ((method != MZ_DEFLATED) || ((mem_level < 1) || (mem_level > 9)) || ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS)))
        return MZ_PARAM_ERROR;

    pStream->data_type = 0;
    pStream->adler = MZ_ADLER32_INIT;
    pStream->msg = NULL;
    pStream->reserved = 0;
    pStream->total_in = 0;
    pStream->total_out = 0;
    if (!pStream->zalloc)
        pStream->zalloc = miniz_def_alloc_func;
    if (!pStream->zfree)
        pStream->zfree = miniz_def_free_func;

    pComp = (tdefl_compressor *)pStream->zalloc(pStream->opaque, 1, sizeof(tdefl_compressor));
    if (!pComp)
        return MZ_MEM_ERROR;

    pStream->state = (struct mz_internal_state *)pComp;

    if (tdefl_init(pComp, NULL, NULL, comp_flags) != TDEFL_STATUS_OKAY)
    {
        mz_deflateEnd(pStream);
        return MZ_PARAM_ERROR;
    }

    return MZ_OK;
}

int mz_deflateReset(mz_streamp pStream)
{
    if ((!pStream) || (!pStream->state) || (!pStream->zalloc) || (!pStream->zfree))
        return MZ_STREAM_ERROR;
    pStream->total_in = pStream->total_out = 0;
    tdefl_init((tdefl_compressor *)pStream->state, NULL, NULL, ((tdefl_compressor *)pStream->state)->m_flags);
    return MZ_OK;
}

int mz_deflate(mz_streamp pStream, int flush)
{
    size_t in_bytes, out_bytes;
    mz_ulong orig_total_in, orig_total_out;
    int mz_status = MZ_OK;

    if ((!pStream) || (!pStream->state) || (flush < 0) || (flush > MZ_FINISH) || (!pStream->next_out))
        return MZ_STREAM_ERROR;
    if (!pStream->avail_out)
        return MZ_BUF_ERROR;

    if (flush == MZ_PARTIAL_FLUSH)
        flush = MZ_SYNC_FLUSH;

    if (((tdefl_compressor *)pStream->state)->m_prev_return_status == TDEFL_STATUS_DONE)
        return (flush == MZ_FINISH) ? MZ_STREAM_END : MZ_BUF_ERROR;

    orig_total_in = pStream->total_in;
    orig_total_out = pStream->total_out;
    for (;;)
    {
        tdefl_status defl_status;
        in_bytes = pStream->avail_in;
        out_bytes = pStream->avail_out;

        defl_status = tdefl_compress((tdefl_compressor *)pStream->state, pStream->next_in, &in_bytes, pStream->next_out, &out_bytes, (tdefl_flush)flush);
        pStream->next_in += (mz_uint)in_bytes;
        pStream->avail_in -= (mz_uint)in_bytes;
        pStream->total_in += (mz_uint)in_bytes;
        pStream->adler = tdefl_get_adler32((tdefl_compressor *)pStream->state);

        pStream->next_out += (mz_uint)out_bytes;
        pStream->avail_out -= (mz_uint)out_bytes;
        pStream->total_out += (mz_uint)out_bytes;

        if (defl_status < 0)
        {
            mz_status = MZ_STREAM_ERROR;
            break;
        }
        else if (defl_status == TDEFL_STATUS_DONE)
        {
            mz_status = MZ_STREAM_END;
            break;
        }
        else if (!pStream->avail_out)
            break;
        else if ((!pStream->avail_in) && (flush != MZ_FINISH))
        {
            if ((flush) || (pStream->total_in != orig_total_in) || (pStream->total_out != orig_total_out))
                break;
            return MZ_BUF_ERROR; /* Can't make forward progress without some input.
                                  */
        }
    }
    return mz_status;
}

int mz_deflateEnd(mz_streamp pStream)
{
    if (!pStream)
        return MZ_STREAM_ERROR;
    if (pStream->state)
    {
        pStream->zfree(pStream->opaque, pStream->state);
        pStream->state = NULL;
    }
    return MZ_OK;
}
