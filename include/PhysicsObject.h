#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <glm/glm.hpp>
#include <vector>
#include "CommonStructs.h"
class PhysicsObject {
    private:
        bool staticMesh;
        
    public:
        glm::vec3 velocity{};
        // should not be cleared, for one time accel bust (like force) use addVelocity might rename to gravity force or smthin
        // probably wont really be used except for gravity
        glm::vec3 acceleration{};
        float mass;
        // between 0 and 1, will be multiply by velo to see how much is retained on collison
        float elasticityConstant;
        //std::vector<AABB> hitBox; 
        bool gravity;

        PhysicsObject();
        PhysicsObject(bool staticMesh) : staticMesh(staticMesh) {
            mass = 0;
            gravity = false;
            elasticityConstant = 0;

        }
        PhysicsObject(float mass, float elasticityConstant, bool gravity) : mass(mass), elasticityConstant(elasticityConstant), gravity(gravity) { 
            staticMesh = false;
            velocity = glm::vec3{};
        }
        ~PhysicsObject();

        void applyForce(glm::vec3& force);

        void setVelocity(glm::vec3& vel);

        void addVelocity(glm::vec3& vel);

        void setAcceleration(glm::vec3& acc);
        
        void addAcceleration(glm::vec3& acc);

        void collisionWith(PhysicsObject* other);
    };

#endif