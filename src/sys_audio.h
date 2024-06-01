#define AUDIO_FREQUENCY     44100 // 11025,22050,44100,48000
//#define AUDIO_LATENCY       100 // ms
#define AUDIO_BUFFERLEN     _SAUDIO_DEFAULT_BUFFER_FRAMES // (AUDIO_FREQUENCY/(1000/AUDIO_LATENCY))

#define audio_queue(sample) do { \
    audio_buffer[audio_write++] = sample; \
    if (audio_write >= AUDIO_BUFFERLEN) audio_write = 0, saudio_push(audio_buffer, AUDIO_BUFFERLEN); \
} while(0)

int audio_write = 0;
float audio_buffer[AUDIO_BUFFERLEN] = {0};

void sys_audio();

void audio_quit(void) {
    saudio_shutdown();
}
void audio_init() {
    saudio_desc desc = {0};
    desc.sample_rate = AUDIO_FREQUENCY;
    desc.buffer_frames = AUDIO_BUFFERLEN; // _SAUDIO_DEFAULT_BUFFER_FRAMES(1024);
    desc.packet_frames = 128/1; // _SAUDIO_DEFAULT_PACKET_FRAMES(128);
    desc.num_packets = 64/2; // _SAUDIO_DEFAULT_NUM_PACKETS(64);
    desc.num_channels = 1;
    saudio_setup(&desc);

    printf("%d audio rate\n", saudio_sample_rate());;
    printf("%d audio frames\n", saudio_buffer_frames());;
    printf("%d audio channels\n", saudio_channels());;

    atexit(audio_quit);
}



//#include "res/audio/insert"  // S16 C1 22050Hz zxsp
//#include "res/audio/eject"   // S16 C1 22050Hz zxsp

//#include "res/audio/motor"   // S16 C1 22050Hz cap32
#include "res/audio/motor2"  // S16 C1 22050Hz rvm
//#include "res/audio/running" // S16 C1 22050Hz zxsp

#include "res/audio/seek"    // S16 C1 22050Hz cap32
//#include "res/audio/seek2"   // S16 C1 22050Hz 

#include "res/audio/read"    // S16 C1 22050Hz
//#include "res/audio/step"    // S16 C1 22050Hz too fast

#include "res/audio/camera"    // S16 C1 22050Hz

typedef struct voice_t {
    int id;
    unsigned ampl;  // amplification
    int16_t *samples;
    unsigned len;   // number of samples
    unsigned count; // number of loops (0=stop, ~0u=inf)
    double pos;     // position within samples (seek)
} voice_t;

enum { voices_max = 4 };

voice_t voice[voices_max];

char *voice_info(int i) {
    int id = voice[i].id;
    return va("play %c%c%c%c x%d %f/%u %p", 
        (voice[i].id>>24)&255,(voice[i].id>>16)&255,(voice[i].id>>8)&255,(voice[i].id>>0)&255,
        voice[i].count,
        voice[i].pos, voice[i].len,
        voice[i].samples );
}

float mix(float dt) {
    float accum = 0, voices = 0;
    for( int i = 0; i < voices_max; ++i ) {
        voice_t *v = voice + i;
        if( !v->count ) continue;

        v->pos += dt;

        if( v->pos >= v->len ) {
            v->pos -= v->len;
            v->count--;
        }

        if( v->count ) {
            // 16-bit mono [-32768..32767] to float [-1..1]
            accum += (v->samples[(unsigned)v->pos] / 32768.f) * v->ampl;
            ++voices;
        }
    }

    return accum / (voices+!voices);
}

int play(int sample, unsigned count) {
    static voice_t motors[] = {
        {'moto', 5, (int16_t*)(44+wavmotor2), (sizeof(wavmotor2) - 44) / 2},
    };
    static voice_t seeks[] = {
        {'seek', 5, (int16_t*)(44+wavseek), (sizeof(wavseek) - 44) / 2},
    };
    static voice_t reads[] = {
        {'read', 5, (int16_t*)(44+wavread), (sizeof(wavread) - 44) / 2},
    };
    static voice_t cameras[] = {
        {'cam', 1, (int16_t*)(44+wavcamera), (sizeof(wavcamera) - 44) / 2},
    };

    voice_t *v = 0;
    // find current slot
    if( !v ) for( int i = 0; i < voices_max; ++i ) { 
        if( voice[i].id == sample ) {
            // already playing? update & exit
            voice[i].count = count;
            return 1;
        }
    }
    // else find free slot
    if( !v ) for( int i = 0; i < voices_max; ++i ) {
        if( voice[i].count ) continue;
        v = voice + i;
        break;
    }
    // else abort
    if( !v ) return 0;

    // load known samples
    /**/ if( sample == 'moto' ) *v = motors[0];
    else if( sample == 'seek' ) *v = seeks[0];
    else if( sample == 'read' ) *v = reads[0];
    else if( sample == 'cam' )  *v = cameras[0];
    else return 0;

    // update markers
    v->id = sample;
    v->pos = 0;
    v->count = count;
    return 1;
}

void mixer_reset() {
    memset(voice, 0, sizeof(voice));
}
