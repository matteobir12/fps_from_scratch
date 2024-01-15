#include "PhysicsThread.h"
#include "PhysicsObject.h"
#include "SpatialHashTable.h"
#include <chrono>
#include <thread>
#include <iostream>

std::atomic<bool> keepRunning(true);
void physicsUpdate() {
    //checkDynamicCollisions();
}

void runAtFrequency(int n) {
    using namespace std::chrono;

    int targetTime = 1000 / n;
    steady_clock::time_point lastUpdateTime = steady_clock::now();
    steady_clock::time_point lastWarningTime = steady_clock::now() - seconds(1);

    int lateCounter = 0; // Counts how many times the loop was late
    int loopCounter = 0; // Counts how many loops in the current second

    while (keepRunning) {
        auto startTime = steady_clock::now();
        
        physicsUpdate();

        auto endTime = steady_clock::now();
        auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);

        auto sleepTime = targetTime - elapsedTime.count();

        loopCounter++;

        if (sleepTime <= 0) {
            lateCounter++;

            // Check if it's been at least 1 second since the last warning
            if(duration_cast<seconds>(steady_clock::now() - lastWarningTime).count() >= 1) {
                std::cout << "Warning: Running behind! ";
                std::cout << "Loop was late " << lateCounter << " times out of " << loopCounter << " in the last second." << std::endl;
                lastWarningTime = steady_clock::now();
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

SpatialHashTable hashTable(4); // maybe dynamically calc cell size, or at the very least figure out a good number.

void addDynamicObject(PhysicsObject* object) {
    hashTable.insertObject(object, {});
}

void updateDynamicObject(PhysicsObject* object) {
    hashTable.updateObject(object, {});
}

void checkDynamicCollisions() {
    // For each object in the hash table:
    // 1. Get potential collisions from hash table
    // 2. Perform AABB check
    // 3. If AABBs intersect, perform detailed collision check
    // 4. Handle any detected collisions
}

void clearDynamicObjects() {
    hashTable = SpatialHashTable(4);
}
