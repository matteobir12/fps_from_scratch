#include <glm/glm.hpp>
#include "ShaderProgram.h"

class GameObject {
private:
    ShaderProgram* shader;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    // Add other necessary properties like VAO, VBO, texture IDs, etc.

    // Method to check if the object is within the field of view
    bool isInFieldOfView(const glm::mat4& viewProjectionMatrix) {
        // Simple example: Check if the position is within a certain range
        // In a real application, you'd use more complex frustum culling
        glm::vec4 pos = viewProjectionMatrix * glm::vec4(position, 1.0f);
        return pos.x > -1.0f && pos.x < 1.0f && pos.y > -1.0f && pos.y < 1.0f;
    }

public:
    GameObject(ShaderProgram* shaderProgram) : shader(shaderProgram) {
        // Initialize position, scale, rotation, etc.
    }

    void draw(const glm::mat4& viewProjectionMatrix) {
        if (!isInFieldOfView(viewProjectionMatrix)) {
            return; // Skip drawing if not in FOV
        }

        shader->use();
        // Set uniforms (like model matrix, view-projection matrix, etc.)
        // Bind VAO, textures, etc.
        // Perform draw call (glDrawArrays or glDrawElements)
    }

    // Setters and getters for position, scale, rotation, etc.
};
