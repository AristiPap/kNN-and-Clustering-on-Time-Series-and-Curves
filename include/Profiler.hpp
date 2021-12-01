#pragma once

#include <chrono>

// initiate the profiler
void profiler_init();

// begin the profiling sequence
void profiler_start();

// end the profiling sequence
void profiler_stop();

// Get the duration (in ms) from the moment you called profiler_start
// till you called profiler stop.
double profiler_get_duration();
