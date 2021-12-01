#include "Profiler.hpp"

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

static std::chrono::_V2::system_clock::time_point t_start;
static std::chrono::_V2::system_clock::time_point t_end;

void profiler_init() {
    // nothing to do yet
    t_start = std::chrono::_V2::system_clock::time_point();
    t_end = std::chrono::_V2::system_clock::time_point();
}

void profiler_start() {
    t_start = high_resolution_clock::now();
}

void profiler_stop() {
    t_end = high_resolution_clock::now();
}

double profiler_get_duration () {
    duration<double, std::milli> ms_double = t_end - t_start;
    return ms_double.count();
}