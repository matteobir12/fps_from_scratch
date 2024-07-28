#include "PhysicsObject.h"

PhysicsObject::PhysicsObject() {
    staticMesh = false;
}

PhysicsObject::~PhysicsObject() {

}

void PhysicsObject::applyForce(glm::vec3& force) {
    if (staticMesh) return;
    acceleration += force / mass;
}

void PhysicsObject::setVelocity(glm::vec3& vel) {
    if (staticMesh) return;
    velocity = vel;
}

void PhysicsObject::addVelocity(glm::vec3& vel) {
    if (staticMesh) return;
    velocity += vel;
}

void PhysicsObject::setAcceleration(glm::vec3& acc) {
    if (staticMesh) return;
    acceleration = acc;
}

void PhysicsObject::addAcceleration(glm::vec3& acc) {
    if (staticMesh) return;
    acceleration += acc;
}

void PhysicsObject::collisionWith(PhysicsObject* other) {
    if (staticMesh) return;
    if (other->staticMesh) {
        // probably will be able to get stuck in object with this code
        velocity = -velocity * elasticityConstant; // change to equal angle on both sides of normal
        
    } else {
        
    }
    
    
}