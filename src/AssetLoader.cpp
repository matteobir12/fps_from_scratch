#include "AssetLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// add if for production
// also make more robust for executing in diff FPs
std::string AssetLoader::texFolder = "./textures/";
std::string AssetLoader::objFolder = "./objects/";

bool AssetLoader::finished = false;
bool AssetLoader::isWorkerRunning = false;
std::queue<AssetLoader::TextureLoadTask> AssetLoader::tasks;
std::mutex AssetLoader::queueMutex;
std::condition_variable AssetLoader::cv;
std::unordered_map<std::string, GLuint> AssetLoader::loadedTextures;
std::thread AssetLoader::worker;

GLuint AssetLoader::loadTexture(const std::string& filename) {
    int width, height, nChannels;
    std::string fullPath = texFolder + filename;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);
    if (!data) {
       std::cout << "faild to load texture file" << std::endl;
       return 0;
    } 
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format;
    if (nChannels == 1)
        format = GL_RED;
    else if (nChannels == 3)
        format = GL_RGB;
    else if (nChannels == 4)
        format = GL_RGBA;
    else {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    stbi_image_free(data);
    return textureID;
}

GLuint AssetLoader::createCubeMap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (GLuint i = 0; i < faces.size(); i++) {
        std::string path = texFolder+faces[i];
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i].c_str() << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return textureID;
}

// probably should async
// add map for reusing already loaded objects
// models smaller than unsigned int verts
GpuObject* AssetLoader::loadObject(const std::string& name) {
    std::vector<std::string> mtlFiles;
    CpuGeometry* geo = parseOBJ(objFolder + name + "/" + name + ".obj", mtlFiles);

    std::cout << geo->data.vertices.size() << " " << geo->data.normals.size() << " " << geo->data.textures.size() << std::endl;

    std::unordered_map<std::string, Material*> materialLibs;
    for (std::string file : mtlFiles){
        parseMTL(objFolder + name + "/", file, materialLibs);
    }
    
    GpuObject* outGeo = VAOFactory::createVAO(geo, materialLibs);
    outGeo->objectId = name;
    free(geo);
    return outGeo;
}

CpuGeometry* AssetLoader::parseOBJ(const std::string& filePath, std::vector<std::string>& mtlFilesToRead) {
    CpuGeometry* geometry = new CpuGeometry();
    FaceMaterial currentFaceMaterial;
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filePath << std::endl;
        return nullptr;
    }
    std::string line;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        if (line.empty() || line[0] == '#') continue;
        std::istringstream linestream(line);
        std::string prefix;
        linestream >> prefix;
        if (prefix == "v") {
            Vertex v;
            if (linestream >> v.x >> v.y >> v.z) {
                geometry->data.vertices.push_back(v);
            } else {
                std::cout << "Failed to read 3 floats." << std::endl;
            }
        } else if (prefix == "vt") {
            geometry->data.hasTexcoords = true;
            TextureCoord vt;
            if (linestream >> vt.u >> vt.v) {
                geometry->data.textures.push_back(vt);
            } else {
                std::cout << "Failed to read 2 floats." << std::endl;
            }
        } else if (prefix == "vn") {
            geometry->data.hasNormals = true;
            VertexNormal vn;
            if (linestream >> vn.xn >> vn.yn >> vn.zn) {
                geometry->data.normals.push_back(vn);
            } else {
                std::cout << "Failed to read 3 floats." << std::endl;
            }
        } else if (prefix == "f") {
            Face f = Face();
            f.vertexIndices.clear();
            f.textureIndices.clear();
            f.normalIndices.clear();
            std::string vertex;
            while (linestream >> vertex) {
                 std::istringstream vertexStream(vertex);
                std::string v, vt, vn;

                std::getline(vertexStream, v, '/');  // Read the vertex index
                std::getline(vertexStream, vt, '/'); // Read the texture index (if any)
                std::getline(vertexStream, vn);      // Read the normal index (if any)

                if (!v.empty()) {
                    int vIndex = std::stoi(v) - 1; // OBJ indices are 1-based
                    f.vertexIndices.push_back(vIndex);
                }

                if (!vt.empty()) {
                    int tIndex = std::stoi(vt) - 1;
                    f.textureIndices.push_back(tIndex);
                }

                if (!vn.empty()) {
                    int nIndex = std::stoi(vn) - 1;
                    f.normalIndices.push_back(nIndex);
                }
            }
            if (f.vertexIndices.size() != 3 && f.vertexIndices.size() != 4) {
                std::cerr << "unexpected read size: " << f.vertexIndices.size() << std::endl;
            }
            if (f.vertexIndices.size() == 4) {
                Face f1 = Face(f.vertexIndices[0], f.vertexIndices[1], f.vertexIndices[2],f.normalIndices[0], f.normalIndices[1], f.normalIndices[2],f.textureIndices[0], f.textureIndices[1], f.textureIndices[2]);
                Face f2  = Face(f.vertexIndices[0], f.vertexIndices[2], f.vertexIndices[3],f.normalIndices[0], f.normalIndices[2], f.normalIndices[3],f.textureIndices[0], f.textureIndices[2], f.textureIndices[3]);
                currentFaceMaterial.faces.push_back(f1);
                currentFaceMaterial.faces.push_back(f2);
            } else {
                currentFaceMaterial.faces.push_back(f);
            }

        } else if (prefix == "usemtl") {
            
            if (!currentFaceMaterial.material.empty()) {
                std::cout << "non empty:" << currentFaceMaterial.material << ":" <<  std::endl;
                geometry->data.FaceMaterials.push_back(currentFaceMaterial);
                currentFaceMaterial = FaceMaterial();
            }
            std::string mtlName;
            linestream >> mtlName;
            currentFaceMaterial.material = mtlName;

        } else if (prefix == "mtllib") {
            std::string mtlFileName;
            linestream >> mtlFileName;
            mtlFilesToRead.push_back(mtlFileName);
        } else if (prefix == "s") {
            // smoothing
        } else if (prefix == "o") {
            // geometry->data.FaceMaterials.push_back(currentFaceMaterial);
            // currentFaceMaterial = FaceMaterial();
        }
    }

    if (!currentFaceMaterial.material.empty()) {
        geometry->data.FaceMaterials.push_back(currentFaceMaterial);
    }

    return geometry;
}


void AssetLoader::parseMTL(const std::string& folderPath, const std::string& fileName, std::unordered_map<std::string, Material*>& materialLibs) {
    Material* currentMaterial = nullptr;
    std::ifstream file(folderPath + fileName);
    if (!file) {
        std::cerr << "Error: Cannot open file " << folderPath + fileName << std::endl;
        return;
    }
    std::string line;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        if (line.empty() || line[0] == '#') continue;
        std::istringstream linestream(line);
        std::string prefix;
        linestream >> prefix;
        bool skip_mtl = false;

        if (prefix == "newmtl") {

            if (skip_mtl)
                skip_mtl = false;

            std::string mtlName;
            linestream >> mtlName;

            if (materialLibs.count(mtlName) != 0) {
                skip_mtl = true;
                std::cout << "multiple materials with the same name, skipping" << std::endl;
            } else {
                currentMaterial = new Material();
                materialLibs[mtlName] = currentMaterial;
            }
        } else if (skip_mtl) {
            continue;
        } else if (!currentMaterial) {
            std::cerr << "Material not defined" << std::endl;
        } else if (prefix == "map_Kd") {
            std::string textureName;
            linestream >> textureName;
            currentMaterial->useTexture = true;

            auto fndTex = loadedTextures.find(textureName);
            
            if (fndTex != loadedTextures.end()) {
                currentMaterial->texture = fndTex->second;
            } else {
                GLuint texID = 0;
                std::string texPath = texFolder + fileName.substr(0, fileName.size() - 4) + "/" + textureName;
                loadTextureAsync(texID, texPath,false);
                currentMaterial->texture = texID;
            }
            
        } else if (prefix == "Kd") {
            glm::vec3 d;
            linestream >> d.x >> d.y >> d.z;
            currentMaterial->diffuse = d;
        } else if (prefix == "Ka") {
            // Ambient reflectivity using RGB values. This sets how the material reflects ambient light, usually used to simulate global illumination effects. "0.00 0.00 0.00" indicates no ambient reflectivity.
        }
        else if (prefix == "Tf") {
            // Transmission filter. This determines the color of the light that passes through the material. "1.00 1.00 1.00" typically means it doesn't change the color of the transmitted light (i.e., white light would pass through unchanged).
        }
        else if (prefix == "Ni") {
            //  Optical density, also known as index of refraction. "1.00" is typically the value for air, meaning no refraction.
        }
        else if (prefix == "Ks") {
            //  Specular reflectivity using RGB values
            glm::vec3 s;
            linestream >> s.x >> s.y >> s.z;
            currentMaterial->specular = s;
        }
        else if (prefix == "Ns") {
            float e;
            linestream >> e;
            currentMaterial->specularEx = e;
        }
    }

}

// multi thread texture stuff
void AssetLoader::textureLoaderWorker() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, []{ return !tasks.empty() || finished; });

        if (finished && tasks.empty()) {
            break;
        }

        TextureLoadTask task = tasks.front();
        tasks.pop();

        lock.unlock();

        // Load the texture data (OpenGL context-sensitive operations here)
        int width, height, channels;
        unsigned char* data = stbi_load(task.filename.c_str(), &width, &height, &channels, 0);

        if (data) {
            glBindTexture(GL_TEXTURE_2D, task.textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << task.filename << std::endl;
        }
    }
}


// might need worker mutex if loadtext goes async
void AssetLoader::loadTextureAsync(GLuint textureID, const std::string& fileLocation, bool async){
    if (finished) {
        finished = false;
    }
    if (async && !isWorkerRunning) {
        worker = std::thread(textureLoaderWorker);
        isWorkerRunning = true;
    }
    if (async){
        queueTextureLoad(textureID, fileLocation);
    } else {
        queueTextureLoad(textureID, fileLocation);
        finished = true;
        textureLoaderWorker();
    }
    
}

void AssetLoader::finishedLoading() {
    finished = true;
}

void AssetLoader::queueTextureLoad(GLuint textureID, const std::string& fileLocation) {
    std::lock_guard<std::mutex> lock(queueMutex);
    tasks.push(TextureLoadTask{textureID, fileLocation});
    cv.notify_one();
}