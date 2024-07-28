#include "VAOFactory.h"

VAOFactory::Attribute::Attribute(GLuint idx, GLint sz, GLenum tp, GLsizei str, size_t off)
    : index(idx), size(sz), type(tp), stride(str), offset(off) {}

Rendering::GpuObject* VAOFactory::createVAO(Rendering::CpuGeometry* cpuGeo, std::unordered_map<std::string, Rendering::Material*>& materialLibs) {
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    Rendering::GpuObject* gpuObjectPtr = new Rendering::GpuObject{VAO, VBO};
    unsigned int offset = 0;
    std::vector<float> interleavedData; // potentially preallocate size
    gpuObjectPtr->hasNormals = cpuGeo->data.hasNormals;
    gpuObjectPtr->hasTexcoords = cpuGeo->data.hasTexcoords;
    for (const auto& fm : cpuGeo->data.FaceMaterials) {
        Rendering::GpuGeometry geo{};
        geo.size = fm.faces.size() * 3;
        geo.offset = offset;
        offset += geo.size;
        if (fm.faces.size() <= 0) {
            std::cerr << "unexpected 0 faces in material" << std::endl;
            continue;
        }

        for (const auto& face : fm.faces) {
            // vertex position
            // if (face.vertexIndices.size() != 3) {
            //     std::cerr << "unexpected size: " << face.vertexIndices.size() << std::endl;
            // }
            for (unsigned int i = 0; i < 3; i++) {
                // Access the vertex using the index i
                const Rendering::Vertex& v = cpuGeo->data.vertices[face.vertexIndices[i]];
                interleavedData.push_back(v.x);
                interleavedData.push_back(v.y);
                interleavedData.push_back(v.z);

                
                // normal
                if (gpuObjectPtr->hasNormals) {
                    const Rendering::VertexNormal& vn = cpuGeo->data.normals[face.normalIndices[i]];
                    interleavedData.push_back(vn.xn);
                    interleavedData.push_back(vn.yn);
                    interleavedData.push_back(vn.zn);
                }

                // texture coord
                if (gpuObjectPtr->hasTexcoords) {
                    const Rendering::TextureCoord& vt = cpuGeo->data.textures[face.textureIndices[i]];
                    interleavedData.push_back(vt.u);
                    interleavedData.push_back(vt.v);
                }
            }
        }
        
        auto iter = materialLibs.find(fm.material);
        if (iter != materialLibs.end()) {
            
            geo.material = iter->second;
        } else if (fm.material.empty()){
            geo.material = nullptr;
        } else {
            std::cerr << "Error: Material key '" << fm.material << "' not found." << std::endl;
        }
        gpuObjectPtr->gpuGeometries.push_back(geo);
    }

    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    std::vector<Attribute> attributes;
    
    unsigned short stride_size = 3;
    unsigned short texOffset = 3;
    if (gpuObjectPtr->hasNormals) {
        stride_size += 3;
        texOffset = 6;
    }
    if (gpuObjectPtr->hasTexcoords) {
        stride_size += 2;
    }

    attributes.emplace_back(0, 3, GL_FLOAT, stride_size * sizeof(float), 0); // Vertex Positions

    if (gpuObjectPtr->hasNormals) {
        attributes.emplace_back(1, 3, GL_FLOAT, stride_size * sizeof(float), 3 * sizeof(float)); // Normals
    }
    if (gpuObjectPtr->hasTexcoords) {
        attributes.emplace_back(2, 2, GL_FLOAT, stride_size * sizeof(float), texOffset * sizeof(float)); // Texture Coordinates
    }

    for (const auto& attribute : attributes) {
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, GL_FALSE, attribute.stride, (void*)attribute.offset);
        glEnableVertexAttribArray(attribute.index);
    }

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    gpuObjectPtr->objectId = cpuGeo->objectId;
    return gpuObjectPtr;
}