#include <string>
#include <unordered_map>
#include <map>
#include <any>

#include "ShaderProgram.h"
#include "Scene.h"
#include "GlobalSettings.h"

using namespace Rendering;

class SceneLoader {

    private:
        // std::map<std::string, std::function<int(const std::string&)>> conversionMap = {
        //     {"int", [](const std::string& x) -> int { return std::atoi(x.c_str()); }}
        // };

        // no capitals
        static Scene parseFile(const std::string& filePath, std::map<std::string, ShaderProgram*> &programs, ShaderProgram* backgoundPro);
    public:
        // kinda temp
        static Scene loadFromFile(std::string filePath, ShaderProgram *backgoundPro, ShaderProgram *scenePro, ShaderProgram *noLightPro);
};