#ifndef PHYSICS_LOOP_H
#define PHYSICS_LOOP_H

#include <atomic>

// Function to start physics loop
void runAtFrequency(int n);

// Global flag to control the thread's execution (extern keyword for global variable)
extern std::atomic<bool> keepRunning;

#endif // PHYSICS_LOOP_H