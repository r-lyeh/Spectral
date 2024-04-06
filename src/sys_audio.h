#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     1 // 2
#define DEVICE_SAMPLE_RATE  44100 // 22050 // 48000
#define AUDIO_LATENCY       100 // ms
#define AUDIO_BUFFER        (2*(DEVICE_SAMPLE_RATE*DEVICE_CHANNELS)/(1000/AUDIO_LATENCY))
// audiobuffer produce
int audio_pos = 0;
float audio_buffer[AUDIO_BUFFER] = {0};
// audiobuffer consume
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
#if 0
    static int last_pos = AUDIO_BUFFER/2;
    if( (last_pos+frameCount) >= AUDIO_BUFFER ) frameCount = AUDIO_BUFFER - last_pos;
    memcpy(pOutput, &audio_buffer[last_pos /*audio_pos > frameCount ? (audio_pos-frameCount) : 0*/], frameCount * sizeof(float));
    last_pos += frameCount;
    if( last_pos >= AUDIO_BUFFER ) last_pos = 0;
#endif
#if 0
    int last_sample = audio_pos - frameCount;
    if( last_sample < 0 ) {
        memcpy(pOutput, &audio_buffer[last_sample + AUDIO_BUFFER], (-last_sample) * sizeof(float));
        frameCount += last_sample;
        last_sample = 0;
    }
    memcpy(pOutput, &audio_buffer[last_sample], frameCount * sizeof(float));
#endif
#if 0
    int last_sample = (int)(audio_pos + AUDIO_BUFFER * AUDIO_LATENCY) % AUDIO_BUFFER;
    if( (last_sample+frameCount) >= AUDIO_BUFFER ) {
        memcpy(pOutput, &audio_buffer[last_sample], (AUDIO_BUFFER-last_sample) * sizeof(float));
        pOutput = &((float*)pOutput)[ AUDIO_BUFFER-last_sample ];
        frameCount -= (AUDIO_BUFFER-last_sample);
        last_sample = 0;
    }
    memcpy(pOutput, &audio_buffer[last_sample], frameCount * sizeof(float));
#endif
    static int last_pos = AUDIO_BUFFER / 2;
    if( (last_pos+frameCount) >= AUDIO_BUFFER ) {
        memcpy(pOutput, &audio_buffer[last_pos], (AUDIO_BUFFER-last_pos) * sizeof(float));
        pOutput = (float*)pOutput + (AUDIO_BUFFER-last_pos);
        frameCount -= (AUDIO_BUFFER-last_pos);
        last_pos = 0;
    }
    memcpy(pOutput, &audio_buffer[last_pos], frameCount * sizeof(float));
    last_pos += frameCount;
}
ma_device device;
void audio_quit(void) {
    ma_device_uninit(&device);
}
void audio_init() {
    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = NULL;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        return; // -4;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return; // -5;
    }

    atexit(audio_quit);
}
