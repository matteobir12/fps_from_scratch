#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Face {
        std::vector<int> vertexIndices;
        std::vector<int> textureIndices;
        std::vector<int> normalIndices;
    };

    struct Material {
        glm::vec3 diffuse;
        glm::vec3 specular;
        float specularEx;
        GLuint texture;
        Material() = default;
    };

    struct FaceMaterial {
        std::vector<Face> faces;
        std::string material;
        FaceMaterial() = default;
    };

    struct CpuGeometry {
        std::string objectId;
        struct Data {
            std::vector<float> vertices;
            std::vector<float> textures;
            std::vector<float> normals;
            std::vector<FaceMaterial> FaceMaterials;
        } data;
    };

    struct GpuGeometry {
        GLuint EBO;
        Material* material;
        unsigned int triangleCnt = 0;
        GpuGeometry() = default;
    };

    struct GpuObject {
        std::string objectId;
        
        GLuint VAO;
        GLuint VBO;
        std::vector<GpuGeometry> gpuGeometries;

        bool useFaces = true;
        
        GpuObject() = default;
        GpuObject(GLuint VAO, GLuint VBO, std::vector<GpuGeometry> gpuGeometries)
            : VAO(VAO), VBO(VBO), gpuGeometries(gpuGeometries) {}

        GpuObject(GLuint VAO, GLuint VBO)
            : VAO(VAO), VBO(VBO) {
                useFaces = false;
        }
        GpuObject(GLuint VAO, GLuint VBO, GLuint EBO, unsigned int triangleCnt): VAO(VAO), VBO(VBO) {
            useFaces = true;
            GpuGeometry geo = GpuGeometry();
            geo.EBO = EBO;
            geo.triangleCnt = triangleCnt;
            gpuGeometries.push_back(geo);
        }
    };

#endif