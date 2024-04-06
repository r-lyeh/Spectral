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
