#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>
#include <future>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include "VAOFactory.h"
#include "CommonStructs.h"

class AssetLoader {
private:
       static std::string texFolder;
       static std::string objFolder;
       struct TextureLoadTask {
            GLuint textureID;
            std::string filename;
        };

        static std::queue<TextureLoadTask> tasks;
        static std::mutex queueMutex;
        static std::condition_variable cv;
        static bool finished;
        static std::unordered_map<std::string, GLuint> loadedTextures;

        static void textureLoaderWorker();

        static std::thread worker;
        static bool isWorkerRunning;
public:

    /*
    if (geometryHasBeenTransferred) {
    geometry.data.vertices.clear(); // Clear vertices
    geometry.data.textures.clear(); // Clear texture coordinates
    geometry.data.normals.clear();  // Clear normals
    geometry.data.faces.clear();    // Clear faces

    geometry.data.vertices.shrink_to_fit();
    geometry.data.textures.shrink_to_fit();
    geometry.data.normals.shrink_to_fit();
    geometry.data.faces.shrink_to_fit();
}
    */

    static void LoadFile(const std::string& filepath);
    
    static GLuint loadTexture(const std::string& filename);
    static void loadTextureAsync(GLuint textureID, const std::string& fileLocation, bool async);
    static GLuint createCubeMap(const std::vector<std::string>& faces);
    static void finishedLoading();
    static void queueTextureLoad(GLuint textureID, const std::string& fileLocation);
    static GpuObject* loadObject(const std::string& objLoc);

    static CpuGeometry* parseOBJ(const std::string& filePath, std::vector<std::string>& mtlFilesToRead);
    static void parseMTL(const std::string& folderPath, const std::string& fileName, std::unordered_map<std::string, Material*>& materialLibs);
};

#endif