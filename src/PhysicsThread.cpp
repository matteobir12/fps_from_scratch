#include "PhysicsThread.h"
#include <chrono>
#include <thread>
#include <iostream>

std::atomic<bool> keepRunning(true);

void physicsUpdate() {

}

void runAtFrequency(int n) {
    using namespace std::chrono;

    int targetTime = 1000 / n;  // For n updates per second
    steady_clock::time_point lastUpdateTime = steady_clock::now();
    steady_clock::time_point lastWarningTime = steady_clock::now() - seconds(1); // Initialize to ensure it's ready to print a warning immediately if needed

    // Counters
    int lateCounter = 0; // Counts how many times the loop was late
    int loopCounter = 0; // Counts how many loops in the current second

    while (true) {  // Replace with a more suitable condition for stopping the thread
        auto startTime = steady_clock::now();
        
        physicsUpdate(); // Perform the physics update

        auto endTime = steady_clock::now();
        auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);

        auto sleepTime = targetTime - elapsedTime.count();

        // Increment loop counter
        loopCounter++;

        if (sleepTime <= 0) {
            // Increment late counter if the update took longer than the target time
            lateCounter++;

            // Check if it's been at least 1 second since the last warning
            if(duration_cast<seconds>(steady_clock::now() - lastWarningTime).count() >= 1) {
                std::cout << "Warning: Running behind! ";
                std::cout << "Loop was late " << lateCounter << " times out of " << loopCounter << " in the last second." << std::endl;
                lastWarningTime = steady_clock::now(); // Reset the last warning time

                // Reset counters for the next second
                lateCounter = 0;
                loopCounter = 0;
            }
        } else {
            // Sleep for the remaining duration
            std::this_thread::sleep_for(milliseconds(sleepTime));
        }

        // Additional code for thread termination, signaling, etc., might be needed
    }
}