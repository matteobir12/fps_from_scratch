#include "VAOFactory.h"

VAOFactory::Attribute::Attribute(GLuint idx, GLint sz, GLenum tp, GLsizei str, size_t off)
    : index(idx), size(sz), type(tp), stride(str), offset(off) {}

GpuObject* VAOFactory::createVAO(const std::vector<float>& vertices, const std::string vaoId, const std::vector<unsigned int>* indices) {
    Attribute tempAttribute = Attribute();
    return createVAO(vertices, vaoId, tempAttribute, indices);
}

GpuObject* VAOFactory::createVAO(const std::vector<float>& vertices, const std::string vaoId, const Attribute& attributes, const std::vector<unsigned int>* indices) {
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    GpuObject* meshPtr = nullptr;

    if (indices) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);
        meshPtr = new GpuObject(VAO, VBO, EBO, indices->size() / 3);
    } else {
        meshPtr = new GpuObject(VAO,VBO);
    }
    meshPtr->objectId = vaoId;
    glVertexAttribPointer(attributes.index, attributes.size, attributes.type, GL_FALSE, attributes.stride, (void*)attributes.offset);
    glEnableVertexAttribArray(attributes.index);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return meshPtr;
}

GpuObject* VAOFactory::createVAO(const std::vector<float>& vertices, const std::string vaoId, const std::vector<float>& normals, const std::vector<unsigned int>* indices) {
    std::vector<Attribute> attributes;
    attributes.emplace_back(0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    attributes.emplace_back(1, 3, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float)); 
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::vector<float> interleavedData;
    size_t numVertices = vertices.size() / 3;
    for (size_t i = 0; i < numVertices; ++i) {
        interleavedData.insert(interleavedData.end(), vertices.begin() + i * 3, vertices.begin() + (i + 1) * 3);
        interleavedData.insert(interleavedData.end(), normals.begin() + i * 3, normals.begin() + (i + 1) * 3);
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    GpuObject* meshPtr = nullptr;
    if (indices) {
        std::cerr << "do not use" << std::endl;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);
        meshPtr = new GpuObject(VAO, VBO, EBO, indices->size() / 3);
    } else {
        meshPtr = new GpuObject(VAO,VBO);
    }

    meshPtr->objectId = vaoId;
    for (const auto& attribute : attributes) {
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, GL_FALSE, attribute.stride, (void*)attribute.offset);
        glEnableVertexAttribArray(attribute.index);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return meshPtr;
}

GpuObject* VAOFactory::createVAO(CpuGeometry* cpuGeo, std::unordered_map<std::string, Material*>& materialLibs) {
    std::vector<Attribute> attributes;
    attributes.emplace_back(0, 3, GL_FLOAT, 8 * sizeof(float), 0); // Vertex Positions
    attributes.emplace_back(1, 3, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float)); // Normals
    attributes.emplace_back(2, 2, GL_FLOAT, 8 * sizeof(float), 6 * sizeof(float)); // Texture Coordinates

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::vector<float> interleavedData;
    size_t numVertices = cpuGeo->data.vertices.size() / 3;
    for (size_t i = 0; i < numVertices; ++i) {
        interleavedData.insert(interleavedData.end(), cpuGeo->data.vertices.begin() + i * 3, cpuGeo->data.vertices.begin() + (i + 1) * 3);
        interleavedData.insert(interleavedData.end(), cpuGeo->data.normals.begin() + i * 3, cpuGeo->data.normals.begin() + (i + 1) * 3);
        interleavedData.insert(interleavedData.end(), cpuGeo->data.textures.begin() + i * 2, cpuGeo->data.textures.begin() + (i + 1) * 2); // Adding texture coords
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    GpuObject* gpuObjectPtr = new GpuObject{VAO, VBO};

    for (const auto& indices : cpuGeo->data.FaceMaterials) {
        std::vector<unsigned int> allIndices; // Store all indices for EBO

        for (const Face& face : indices.faces) {
            for (int index : face.vertexIndices) {
                allIndices.push_back(index);
            }
        }

        gpuObjectPtr->useFaces = true;
        GpuGeometry EBO;
        glGenBuffers(1, &EBO.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndices.size() * sizeof(unsigned int), allIndices.data(), GL_STATIC_DRAW);
        EBO.triangleCnt = indices.faces.size() / 3;
        auto iter = materialLibs.find(indices.material);

        if (iter != materialLibs.end()) {
            EBO.material = iter->second;
        } else {
            std::cerr << "Error: Material key '" << indices.material << "' not found." << std::endl;
        }
        gpuObjectPtr->gpuGeometries.push_back(EBO);
    }

    for (const auto& attribute : attributes) {
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, GL_FALSE, attribute.stride, (void*)attribute.offset);
        glEnableVertexAttribArray(attribute.index);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    gpuObjectPtr->objectId = cpuGeo->objectId;
    return gpuObjectPtr;
}