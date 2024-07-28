#ifndef BVH_H
#define BVH_H

#include <vector>

#include "CommonStructs.h"

#define THRESHOLD 1;

using namespace Rendering;

// Node structure for BVH
struct BVHNode {
    AABB box;
    BVHNode* left;
    BVHNode* right;
    // Additional data for leaf nodes, like pointers to geometry
    // ...
};

class BVH {
public:
    BVH(const std::vector<Geometry>& objects){
        root = buildBVH(objects, 0, objects.size());
    };
    ~BVH();

    // Public methods for working with BVH
    // ...

private:
    BVHNode* root;

    BVHNode* buildBVH(const std::vector<Geometry>& objects, int start, int end) {
        // Base case: If the range is small, create a leaf node
        if (end - start <= THRESHOLD) {
            BVHNode* leafNode = new BVHNode();
            leafNode->left = leafNode->right = nullptr;
            leafNode->box = computeBoundingBox(objects, start, end);
            // Assign or reference the objects to this leaf node
            // ...
            return leafNode;
        }

        // Choose a splitting axis and position
        int splitAxis = chooseSplitAxis(objects, start, end);
        int splitPosition = partitionObjects(objects, start, end, splitAxis);

        // Recursive construction of child nodes
        BVHNode* leftChild = buildBVH(objects, start, splitPosition);
        BVHNode* rightChild = buildBVH(objects, splitPosition, end);

        // Create a new internal node
        BVHNode* node = new BVHNode();
        node->left = leftChild;
        node->right = rightChild;
        node->box = combineBoundingBoxes(leftChild->box, rightChild->box);

        return node;
    }
    AABB computeBoundingBox(const std::vector<Geometry>& objects, int start, int end);
    int partitionObjects(std::vector<Geometry>& objects, int start, int end, int axis);
    int chooseSplitAxis(const std::vector<Geometry>& objects, int start, int end);
};

#endif