#ifndef SPATIAL_HASH_TABLE_H
#define SPATIAL_HASH_TABLE_H

#include <unordered_map>
#include <vector>
#include <functional>
#include "CommonStructs.h"
#include "PhysicsObject.h"


// does not support objects larger than cell size
class SpatialHashTable {
private:
    struct CellKey {
        int x, y, z;

        // Equality comparison for CellKey
        bool operator==(const CellKey& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    // Custom hash function for CellKey
    struct CellKeyHash {
        std::size_t operator()(const CellKey& key) const {
            std::size_t h1 = std::hash<int>()(key.x);
            std::size_t h2 = std::hash<int>()(key.y);
            std::size_t h3 = std::hash<int>()(key.z);

            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    // Hash table mapping cell keys to a vector of object IDs/pointers
    std::unordered_map<CellKey, std::vector<PhysicsObject*>, CellKeyHash> hashTable;
    std::unordered_map<PhysicsObject*, CellKey> reverseMap;
    float cellSize;
    // larger is more precise. larger has bigger memory footprint.
    int precision;

public:
    SpatialHashTable(float cellSize) : cellSize(cellSize) {
        precision = 10;
    }

    // Convert a point to a CellKey based on cell size
    CellKey getCellKey(const Point3D& point) const {
        return { 
            static_cast<int>(std::floor(point.x * precision / cellSize)),
            static_cast<int>(std::floor(point.y * precision / cellSize)),
            static_cast<int>(std::floor(point.z * precision / cellSize))
        };
    }

    // Insert an object into the hash table
    void insertObject(PhysicsObject* objectId, const Point3D& position) {
        CellKey key = getCellKey(position);
        hashTable[key].push_back(objectId);
        reverseMap[objectId] = key;
    }

    // Remove an object from the hash table
    void removeObject(PhysicsObject* objectId) {
        if (reverseMap.find(objectId) != reverseMap.end()) {
            CellKey key = reverseMap[objectId];
            auto& cell = hashTable[key];
            cell.erase(std::remove(cell.begin(), cell.end(), objectId), cell.end());
            reverseMap.erase(objectId); // Remove the entry from the reverse map
        }
    }

    // Update an object's position in the hash table
    void updateObject(PhysicsObject* objectId, const Point3D& newPosition) {
        removeObject(objectId);
        insertObject(objectId, newPosition);
    }

    // Retrieve potential collisions for a given object
    std::vector<PhysicsObject*> getPotentialCollisions(const Point3D& position) {
        CellKey key = getCellKey(position);
        if (hashTable.find(key) != hashTable.end()) {
            return hashTable[key];
        }
        return {};
    }
};

#endif