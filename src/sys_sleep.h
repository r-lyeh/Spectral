// high-perf functions

#include <stdint.h>

// exports

uint64_t time_ns();

#ifdef _WIN32
#define sys_sleep(ms) timerSleep((ms)/1000.0) // SleepEx((ms), FALSE)
#define sys_yield()   SwitchToThread()
#else
#define sys_sleep(ms) usleep((ms)*1000)
#define sys_yield()   usleep(0)
#endif


// impl

#ifdef _WIN32
#include <winsock2.h>

static uint64_t nanotimer(uint64_t *out_freq) {
    if( out_freq ) {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        *out_freq = li.QuadPart;
    }
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (uint64_t)li.QuadPart;
}

void timerSleep(double seconds) {
    // src: https://blog.bearcats.nl/accurate-sleep-function/

    static HANDLE timer = 0; do_once timer = CreateWaitableTimerA(NULL, FALSE, NULL);
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;
    
    while (seconds - estimate > 1e-7) {
        double toWait = seconds - estimate;
        LARGE_INTEGER due = {0};
        due.QuadPart = -(int64_t)(toWait * 1e7);
        SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
        uint64_t start = time_ns();
        WaitForSingleObject(timer, INFINITE);
        uint64_t end = time_ns();

        double observed = (end - start) / 1e9;
        seconds -= observed;

        ++count;
        double error = observed - toWait;
        double delta = error - mean;
        mean += delta / count;
        m2   += delta * (error - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    uint64_t start = time_ns();
    while ((time_ns() - start) < (seconds * 1e9));
}

#else

#include <sys/time.h>

#define TIMER_E3 1000ULL
#define TIMER_E6 1000000ULL
#define TIMER_E9 1000000000ULL

#ifdef CLOCK_MONOTONIC_RAW
#define TIME_MONOTONIC CLOCK_MONOTONIC_RAW
#elif defined CLOCK_MONOTONIC
#define TIME_MONOTONIC CLOCK_MONOTONIC
#else
// #define TIME_MONOTONIC CLOCK_REALTIME // untested
#endif

static uint64_t nanotimer(uint64_t *out_freq) {
    if( out_freq ) {
#if defined TIME_MONOTONIC
        *out_freq = TIMER_E9;
#else
        *out_freq = TIMER_E6;
#endif
    }
#if defined TIME_MONOTONIC
    struct timespec ts;
    clock_gettime(TIME_MONOTONIC, &ts);
    return (TIMER_E9 * (uint64_t)ts.tv_sec) + ts.tv_nsec;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (TIMER_E6 * (uint64_t)tv.tv_sec) + tv.tv_usec;
#endif
}

#endif



uint64_t time_ns() {
    static __thread uint64_t epoch = 0;
    static __thread uint64_t freq = 0;
    if( !freq ) {
        epoch = nanotimer(&freq);
    }

    uint64_t a = nanotimer(NULL) - epoch;
    uint64_t b = 1000000000ULL;
    uint64_t c = freq;

    // Computes (a*b)/c without overflow, as long as both (a*b) and the overall result fit into 64-bits.
    // [ref] https://github.com/rust-lang/rust/blob/3809bbf47c8557bd149b3e52ceb47434ca8378d5/src/libstd/sys_common/mod.rs#L124
    uint64_t q = a / c;
    uint64_t r = a % c;
    return q * b + r * b / c;
}
