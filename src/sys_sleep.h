// high-perf functions

#include <stdint.h>
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

void timerSleep(double seconds) {
    // src: https://blog.bearcats.nl/accurate-sleep-function/

    static HANDLE timer; do_once timer = CreateWaitableTimer(NULL, FALSE, NULL);
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds - estimate > 1e-7) {
        double toWait = seconds - estimate;
        LARGE_INTEGER due;
        due.QuadPart = -(int64_t)(toWait * 1e7);
        int64_t start = time_ns();
        SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        int64_t end = time_ns();

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
    int64_t start = time_ns();
    int64_t delay = (int64_t)(seconds * 1e9);
    while (time_ns() - start < delay);
}

// exports

#define sys_sleep(ms) timerSleep((ms)/1000.0) // SleepEx((ms), FALSE)
#define sys_yield() SwitchToThread()
