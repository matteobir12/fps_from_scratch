#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <string>
#include <iostream>

namespace Networking {
constexpr std::size_t kSinglePacketSize = 1472;
constexpr char kDelimiter = 0xFF;
constexpr char kEscape = 0xFE;

struct PacketHeader {
    uint16_t type;
    uint32_t length;
};

// Need to remain as POD
struct NetworkPoint3D {
    float x, y, z;
};

// Need to remain as POD
struct NetworkOrientation {
    float roll;
    float pitch;
    float yaw;
};

// All need to be POD. Can only Default init in C++20
struct ClientToServerNetworkData {
    NetworkPoint3D outGoingPoint;
    NetworkOrientation outGoingOrientation;
    bool shoot;
};

constexpr int kPlayerPacketDataSize = sizeof(ClientToServerNetworkData);

struct RebuildingStruct {
    char buffer[kPlayerPacketDataSize];
    int valid_bits = 0;
    uint8_t checksum = 0;
};

}

namespace Rendering {

struct Face {
    std::array<unsigned int, 3> vertexIndices;
    std::array<unsigned int, 3> textureIndices;
    std::array<unsigned int, 3> normalIndices;
    bool validVt = false;
    bool validTx = false;
    bool validNm = false;
    Face() = default;

    Face(unsigned int x, unsigned int y, unsigned int z) {
        vertexIndices[0] = x;
        vertexIndices[1] = y;
        vertexIndices[2] = z;
        validVt = true;
    };

    Face(unsigned int x, unsigned int y, unsigned int z, unsigned int xn, unsigned int yn, unsigned int zn) {
        vertexIndices[0] = x;
        vertexIndices[1] = y;
        vertexIndices[2] = z;
        normalIndices[0] = xn;
        normalIndices[1] = yn;
        normalIndices[2] = zn;
        validVt = true;
        validNm = true;
    };

    Face(unsigned int x, unsigned int y, unsigned int z, unsigned int xn, unsigned int yn, unsigned int zn, unsigned int xt, unsigned int yt, unsigned int zt) {
        vertexIndices[0] = x;
        vertexIndices[1] = y;
        vertexIndices[2] = z;
        normalIndices[0] = xn;
        normalIndices[1] = yn;
        normalIndices[2] = zn;
        textureIndices[0] = xt;
        textureIndices[1] = yt;
        textureIndices[2] = zt;
        validVt = true;
        validTx = true;
        validNm = true;
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

struct GameObjectFlags {
    bool usesLight = true;
};

struct PositionRotationScale {
    glm::vec3 objectPosition = glm::vec3();
    glm::vec3 objectRotation = glm::vec3();
    glm::vec3 objectScale = glm::vec3(1.0f);
};

struct Point3D {
    float x, y, z;

    Point3D() : x(0), y(0), z(0) {}
    Point3D(float xCoord, float yCoord, float zCoord) : x(xCoord), y(yCoord), z(zCoord) {}
    Networking::NetworkPoint3D ToNetworkPoint() { return {x, y, z}; }
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

struct Light {
    glm::vec3 position;
    glm::vec3 lightDirection;
    glm::vec3 lightColor;
    bool lightIsDirectional;
    bool lightIsOn;
    Light(glm::vec3 position, glm::vec3 lightDirection, glm::vec3 lightColor, bool lightIsDirectional, bool lightIsOn) 
    : position(position), lightDirection(lightDirection), lightColor(lightColor), lightIsDirectional(lightIsDirectional), lightIsOn(lightIsOn)
    {};
};

struct Orientation {
    float roll;
    float pitch;
    float yaw;

    Networking::NetworkOrientation ToNetworkOrientation() { return {roll, pitch, yaw}; }
};

}

#endif