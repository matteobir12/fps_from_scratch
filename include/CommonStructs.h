#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>

struct Face {
    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> textureIndices;
    std::vector<unsigned int> normalIndices;
    Face() = default;

    Face(unsigned int x, unsigned int y, unsigned int z) {
        vertexIndices.push_back(x);
        vertexIndices.push_back(y);
        vertexIndices.push_back(z);
    };
    Face(unsigned int x, unsigned int y, unsigned int z, unsigned int xn, unsigned int yn, unsigned int zn) {
        vertexIndices.push_back(x);
        vertexIndices.push_back(y);
        vertexIndices.push_back(z);
        normalIndices.push_back(xn);
        normalIndices.push_back(yn);
        normalIndices.push_back(zn);
    };
    Face(unsigned int x, unsigned int y, unsigned int z, unsigned int xn, unsigned int yn, unsigned int zn, unsigned int xt, unsigned int yt, unsigned int zt) {
        vertexIndices.push_back(x);
        vertexIndices.push_back(y);
        vertexIndices.push_back(z);
        normalIndices.push_back(xn);
        normalIndices.push_back(yn);
        normalIndices.push_back(zn);
        textureIndices.push_back(xt);
        textureIndices.push_back(yt);
        textureIndices.push_back(zt);
    };
};

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    float specularEx;
    GLuint texture;
    bool useTexture = false;
    Material() = default;
};

struct FaceMaterial {
    std::vector<Face> faces;
    std::string material;
    FaceMaterial() = default;
};

struct Vertex {
    float x, y, z;
    Vertex() = default;
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}; 
};

struct VertexNormal {
    float xn, yn, zn;
};

struct TextureCoord {
    float u, v;
};

struct CpuGeometry {
    std::string objectId;
    struct Data {
        std::vector<Vertex> vertices;
        std::vector<TextureCoord> textures;
        std::vector<VertexNormal> normals;
        std::vector<FaceMaterial> FaceMaterials;
        bool hasNormals = false;
        bool hasTexcoords = false;
    } data;
    CpuGeometry() = default;
    CpuGeometry(std::string objectId) : objectId(objectId) {};
};

// maybe change to represent each sub geo so default case is handled better. (remove offset)
struct GpuGeometry {
    unsigned int size;
    unsigned int offset;
    Material* material = nullptr;
    GpuGeometry() = default;
};

struct GpuObject {
    std::string objectId;
    
    GLuint VAO;
    GLuint VBO;

    bool hasNormals = false;
    bool hasTexcoords = false;

    std::vector<GpuGeometry> gpuGeometries;
    
    GpuObject() = default;
    GpuObject(GLuint VAO, GLuint VBO, std::vector<GpuGeometry> gpuGeometries)
        : VAO(VAO), VBO(VBO), gpuGeometries(gpuGeometries) {}

    // use only when you will be defining each subgeo
    GpuObject(GLuint VAO, GLuint VBO)
        : VAO(VAO), VBO(VBO) {
    }
    // use only when one large geo
    GpuObject(GLuint VAO, GLuint VBO, unsigned int size, unsigned int offset): VAO(VAO), VBO(VBO) {
        GpuGeometry geo = GpuGeometry();
        geo.size = size;
        geo.offset = offset;
        gpuGeometries.push_back(geo);
    }
};

struct Point3D {
    float x, y, z;

    Point3D() : x(0), y(0), z(0) {}
    Point3D(float xCoord, float yCoord, float zCoord) : x(xCoord), y(yCoord), z(zCoord) {}
};

struct AABB {
    Point3D min;
    Point3D max;

    AABB() {}
    AABB(const Point3D& minPoint, const Point3D& maxPoint) : min(minPoint), max(maxPoint) {}
    AABB(const Point3D& startPoint, float height, float width, float length) 
        : min(startPoint), max(Point3D(startPoint.x + width, startPoint.y + height, startPoint.z + length)) {}

    bool intersects(const AABB& other) const {
        if (max.x < other.min.x || min.x > other.max.x) return false;
        if (max.y < other.min.y || min.y > other.max.y) return false;
        if (max.z < other.min.z || min.z > other.max.z) return false;
        return true;
    }

    // todo
    void draw() const {
        std::cout << "AABB Min: (" << min.x << ", " << min.y << ", " << min.z << "), "
                  << "Max: (" << max.x << ", " << max.y << ", " << max.z << ")\n";
    }
};

#endif