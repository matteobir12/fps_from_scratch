#include "VAOFactory.h"

VAOFactory::Attribute::Attribute(GLuint idx, GLint sz, GLenum tp, GLsizei str, size_t off)
    : index(idx), size(sz), type(tp), stride(str), offset(off) {}

std::vector<GLuint> VAOFactory::createVAO(const std::vector<float>& vertices, const char * vaoId, const std::vector<unsigned int>* indices) {
    Attribute tempAttribute = Attribute();
    return createVAO(vertices, vaoId, tempAttribute, indices);
}

std::vector<GLuint> VAOFactory::createVAO(const std::vector<float>& vertices, const char * vaoId, const Attribute& attributes, const std::vector<unsigned int>* indices) {
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    if (indices) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);
    }
    glVertexAttribPointer(attributes.index, attributes.size, attributes.type, GL_FALSE, attributes.stride, (void*)attributes.offset);
    glEnableVertexAttribArray(attributes.index);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return std::vector<GLuint> {VAO, VBO, EBO};
}

std::vector<GLuint> VAOFactory::createVAO(const std::vector<float>& vertices, const char * vaoId, const std::vector<float>& normals, const std::vector<unsigned int>* indices) {
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

    if (indices) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);
    }

    for (const auto& attribute : attributes) {
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, GL_FALSE, attribute.stride, (void*)attribute.offset);
        glEnableVertexAttribArray(attribute.index);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return std::vector<GLuint> {VAO, VBO, EBO};
}