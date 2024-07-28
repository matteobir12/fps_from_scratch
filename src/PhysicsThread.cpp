#include "PhysicsThread.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "ThreadAtFreq.h"
#include "SpatialHashTable.h"
#include <chrono>
#include <thread>
#include <iostream>

std::vector<GameObject*> objsInScene;

void physicsUpdate() {
    // first update all physics properties
    //checkDynamicCollisions();
    // then enact their effects on the objects
    for (GameObject* obj : objsInScene) {
        if (obj->physData) {
            obj->move(obj->physData->velocity);
        }
    }
}



void start(int hz, std::vector<GameObject*> &) {
    // immediately goes out of scope lol 
    auto thread = ThreadAtFreq(physicsUpdate, hz, "Physics");
    thread.start();
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
