#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <iostream>
#include <GL/glew.h>
#include <vector>


class AssetLoader {
private:
       static std::string texFolder;
       static std::string objFolder;
public:
    static void LoadFile(const std::string& filepath);
    
    static GLuint loadTexture(const std::string& filename);
    static GLuint createCubeMap(const std::vector<std::string>& faces);

};

#endif