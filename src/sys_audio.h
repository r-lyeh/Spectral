#include "sys_audio2.h"


// Audio system for a single-threaded ZX Spectrum emulator
// Implements a circular buffer with dynamic overrun prevention, framerate adaptation, and sample averaging

#define AUDIO_FREQUENCY     44100
#define AUDIO_BUFFERLEN      8192 // 16384:~371.52ms at 44.1kHz, 4096:~93ms at 44.1kHz
#define AUDIO_CHANNELS          2 // 1=Mono, 2=stereo

#if AUDIO_CHANNELS==2
typedef struct { float l,r; } float2;
#else
typedef union { float l,r; } float2;
#endif

// Circular buffer for audio channels
float audio_buffer1[AUDIO_BUFFERLEN*2] = {0}; // beeper
float audio_buffer2[AUDIO_BUFFERLEN*2] = {0}; // AY1, ch0
float audio_buffer3[AUDIO_BUFFERLEN*2] = {0}; // AY1, ch1
float audio_buffer4[AUDIO_BUFFERLEN*2] = {0}; // AY1, ch2
float audio_buffer5[AUDIO_BUFFERLEN*2] = {0}; // AY2, ch0
float audio_buffer6[AUDIO_BUFFERLEN*2] = {0}; // AY2, ch1
float audio_buffer7[AUDIO_BUFFERLEN*2] = {0}; // AY2, ch2
float2 audio_mixed[AUDIO_BUFFERLEN*2] = {0};   // Mixed output

int audio_write = 0;         // Write position
int audio_read = 0;          // Read position

// Helper function to push audio to sokol audio
static void push_audio(int count) {
    float mix(float);  // External mixing function

    if( (audio_read + count) <= AUDIO_BUFFERLEN ) {
        // 1-pass
        float *s = (float*)&audio_mixed[ audio_read ];
        for (int i = 0; i < count * AUDIO_CHANNELS; i += AUDIO_CHANNELS) {
            float m = mix(0.5f);
#if AUDIO_CHANNELS == 2
            s[i+0] += m;
            s[i+1] += m;
#else
            s[ i ] += m;
#endif

#if 0
            // Normalize to prevent clipping
            if (s[i] > 1.0f) s[i] = 1.0f;
            else
            if (s[i] < -1.0f) s[i] = -1.0f;
#endif
        }
        if( count ) {
            saudio_push(s, count * AUDIO_CHANNELS);
            audio_read += count;
        }
    } else {
        // 2-passes
        int avail = AUDIO_BUFFERLEN - audio_read;
        push_audio( avail );
        audio_read = 0;
        push_audio( count - avail );
    }
}

void audio_queue(float sample, float samples[7], int ays) {
    // Current and cumulative errors count
    static int underruns = 0, total_underruns = 0;
    static int overruns = 0, total_overruns = 0;

    // Calculate num available samples
    int avail_samples = (audio_write - audio_read + AUDIO_BUFFERLEN) % AUDIO_BUFFERLEN;

    // Check if sokolaudio needs samples
    int expected = saudio_expect();
    if (expected > 0) {
        // Push available samples
        if( expected < avail_samples ) {
            push_audio( expected );
        } else {
            underruns++; // AUDIO_LOG_LIGHT("audio underrun: insufficient samples");
            total_underruns++;
        }
    }

    // Queue sample
    int next_pos = (audio_write + 1) % AUDIO_BUFFERLEN;
    if (next_pos != audio_read) {
        audio_mixed[audio_write].l =
        audio_mixed[audio_write].r = sample;
#if AUDIO_CHANNELS == 2
        extern int ZX_STEREO;
        if( ZX_STEREO )
        {
            float BUZZ = samples[0];
            float AY_A =(samples[1] + samples[4]) / ays;
            float AY_B =(samples[2] + samples[5]) / ays;
            float AY_C =(samples[3] + samples[6]) / ays;
            float AY_L = AY_A + AY_C * 0.50; AY_L /= 1.5;
            float AY_R = AY_B + AY_C * 0.50; AY_R /= 1.5;
            float master = 0.98f * !!sample; // * !!ZX_AY;

            // Store the samples in the circular buffer
            audio_mixed[audio_write].l = (sample * 0.50 + AY_L * 0.50) * master; // (BUZZ * 0.75 + AY_L * 0.25) * master;
            audio_mixed[audio_write].r = (sample * 0.50 + AY_R * 0.50) * master; // (BUZZ * 0.75 + AY_R * 0.25) * master;
        }
#endif

        audio_buffer1[audio_write] = samples[0];
        audio_buffer2[audio_write] = samples[1];
        audio_buffer3[audio_write] = samples[2];
        audio_buffer4[audio_write] = samples[3];
        audio_buffer5[audio_write] = samples[4];
        audio_buffer6[audio_write] = samples[5];
        audio_buffer7[audio_write] = samples[6];
        // Advance write pointer
        audio_write = next_pos;
    } else {
        // drop sample. cannot queue
        overruns++; // AUDIO_LOG_LIGHT("audio overrun: buffer full, cannot queue");
        total_overruns++;
    }

    // Periodic logging with deduplication flush
    static uint64_t last_log_time = 0;
    for(uint64_t now = time_ns(); now - last_log_time > 1000000000; last_log_time = now) { // Log once per second

        extern float ZX_FPS;
        extern uint64_t ticks;
        lprintf("%c%5.1f need:%4d have:%4d from:(%5d-%5d)/%d overruns:%d/%d underruns:%d/%d [ns:%llu] [z80-ticks:%llu]\n",
               underruns || overruns ? 'N': 'Y', ZX_FPS,
               expected, avail_samples,audio_write,audio_read,AUDIO_BUFFERLEN,
               overruns,total_overruns,
               underruns,total_underruns,
               time_ns(), ticks);

        underruns = overruns = 0; // Reset counters after logging
    }
}

void audio_reset(void) {
    memset(audio_buffer1, 0, sizeof(audio_buffer1));
    memset(audio_buffer2, 0, sizeof(audio_buffer2));
    memset(audio_buffer3, 0, sizeof(audio_buffer3));
    memset(audio_buffer4, 0, sizeof(audio_buffer4));
    memset(audio_buffer5, 0, sizeof(audio_buffer5));
    memset(audio_buffer6, 0, sizeof(audio_buffer6));
    memset(audio_buffer7, 0, sizeof(audio_buffer7));
    memset(audio_mixed, 0, sizeof(audio_mixed));

    // Start with 75% buffer fill
    audio_write = AUDIO_BUFFERLEN / 2; // (3 * AUDIO_BUFFERLEN) / 4;
    audio_read = 0;

    // Push initial silence to avoid immediate underruns
    float2 silence[1024] = {0};
    for( int i = 0; i < ((AUDIO_BUFFERLEN/2)/1024); ++i)
        saudio_push((float*)silence, 1024 /* * AUDIO_CHANNELS */);
}

void audio_quit(void) {
    saudio_shutdown();
}

void audio_init(void) {
    saudio_desc desc = {0};
    desc.sample_rate = AUDIO_FREQUENCY;
    desc.buffer_frames =   (AUDIO_BUFFERLEN/8); // number of frames in the buffer (2048: ~46.4ms)
    desc.packet_frames = (AUDIO_BUFFERLEN/128); // number of packets per thread transfer
    desc.num_packets =   (AUDIO_BUFFERLEN/256); // number of packets in ring buffer
    desc.num_channels = AUDIO_CHANNELS;
    saudio_setup(&desc);
    atexit(audio_quit);

    lprintf("%d audio rate\n", saudio_sample_rate());
    lprintf("%d audio frames\n", saudio_buffer_frames());
    lprintf("%d audio channels\n", saudio_channels());

    if( saudio_sample_rate() != AUDIO_FREQUENCY || saudio_channels() != AUDIO_CHANNELS )
        alert(va("Cannot initialize audio [%dHz,%dch]", saudio_sample_rate(), saudio_channels())),
        exit(-1);

    audio_reset();
}
