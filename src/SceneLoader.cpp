#include "SceneLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <functional>

#include "AssetLoader.h"
#include "GameObject.h"

        // std::map<std::string, std::function<int(const std::string&)>> conversionMap = {
        //     {"int", [](const std::string& x) -> int { return std::atoi(x.c_str()); }}
        // };

        // no capitals
Scene SceneLoader::parseFile(const std::string& filePath, std::map<std::string, ShaderProgram*> &programs, ShaderProgram* backgoundPro) {
    std::unordered_map<std::string, std::any> dataContainer = {};
    std::vector<GameObject*> objs;
    GameObject* playerModel = nullptr;
    bool hasBackground = false;
    GLuint cubemapTexture = 0;
    Camera* c = nullptr;
    
    std::ifstream file(filePath);
    if (!file.good())
        throw std::runtime_error("bad stream for file " + filePath);
    std::string line;
    int lineNum = 0;
    while (getline(file, line)) {
        lineNum += 1;
        try {
            if (line.empty() || line[0] != '<' || line.back() != '>') continue;

            line = line.substr(1, line.size() - 2);
            size_t pos = line.find(':');
            if (pos == std::string::npos) continue;

            std::string type = line.substr(0, pos);
            std::cout << type << std::endl;
            std::string parameters = line.substr(pos + 1);
            std::cout << parameters << std::endl;
            std::istringstream paramStream(parameters);
            std::string param;

            while (getline(paramStream, param, ';')) {
                size_t eqPos = param.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = param.substr(0, eqPos);
                    std::string value = param.substr(eqPos + 1);

                    if (value.size() > 1 && value.front() == '"' && value.back() == '"') {
                        dataContainer[key] = value.substr(1, value.size() - 2);
                    }  
                    else if (value.front() == '(' && value.back() == ')') {
                        value = value.substr(1, value.length() - 2);
                        std::istringstream vecStream(value);
                        std::string component;
                        std::vector<float> components;
                        while (getline(vecStream, component, ',')) {
                            if (component.back() == 'f') {
                                std::string number = component.substr(0, component.size() - 1);
                                components.push_back(std::stof(number));
                            } else {
                                components.push_back(std::stoi(component));
                            }
                        }

                        if (components.size() == 3) {
                            dataContainer[key] = glm::vec3(components[0], components[1], components[2]);
                        } else if (components.size() == 4) {
                            dataContainer[key] = glm::vec4(components[0], components[1], components[2], components[3]);
                        } else {
                            throw std::runtime_error("vec bad len");
                        }

                        } else if (!value.empty() && value.back() == 'f') {
                            // Handle float value, remove trailing 'f' before conversion
                            std::string number = value.substr(0, value.size() - 1);
                            dataContainer[key] = std::stof(number);
                        } else {
                            dataContainer[key] = std::stoi(value);
                        }
                    }
            }

            // switch to switch
            if (type == "object" || type == "player") {
                auto it = dataContainer.find("meshname");
                std::string meshName;
                if (it != dataContainer.end()) {
                    meshName = std::any_cast<std::string>(it->second);
                } else {
                    throw std::runtime_error("object needs meshpath");
                }

                GpuObject* objMesh = AssetLoader::loadObject(meshName);
                it = dataContainer.find("position");
                glm::vec3 position;
                if (it != dataContainer.end()) {
                    position = std::any_cast<glm::vec3>(it->second);
                } else {
                    position = glm::vec3();
                }

                it = dataContainer.find("scale");
                glm::vec3 scale;
                if (it != dataContainer.end()) {
                    scale = std::any_cast<glm::vec3>(it->second);
                } else {
                    scale = glm::vec3(1);
                }

                it = dataContainer.find("rotation");
                glm::vec3 rotation;
                if (it != dataContainer.end()) {
                    rotation = std::any_cast<glm::vec3>(it->second);
                } else {
                    rotation = glm::vec3();
                }

                it = dataContainer.find("physics");
                bool physicsEnable;
                if (it != dataContainer.end()) {
                    if (it->second.type() == typeid(std::string)) {
                        physicsEnable = (std::any_cast<std::string>(it->second)) == "true";
                    } else if (it->second.type() == typeid(int)) {
                        physicsEnable = (std::any_cast<int>(it->second)) != 0;
                    }
                } else {
                    physicsEnable = false;
                }

                it = dataContainer.find("fixed");
                bool isFixed;
                if (it != dataContainer.end()) {
                    if (it->second.type() == typeid(std::string)) {
                        isFixed = (std::any_cast<std::string>(it->second)) == "true";
                    } else if (it->second.type() == typeid(int)) {
                        isFixed = (std::any_cast<int>(it->second)) != 0;
                    }
                } else {
                    isFixed = false;
                }

                it = dataContainer.find("program");
                ShaderProgram *program = nullptr;
                bool usesLight = false;
                if (it != dataContainer.end()) {
                    std::string str = std::any_cast<std::string>(it->second);
                    auto proIt = programs.find(str);
                    if (proIt != programs.end()) {
                        program = proIt->second;
                        if (str == "light") {
                            usesLight = true;
                            std::cout << "uses light" << std::endl;
                        } else {
                            program = programs.begin()->second;
                        }
                    } else {
                        std::cout << "invalid program on line: " << lineNum << " using default" << std::endl;
                        program = programs.begin()->second;
                    }
                } else {
                    program = programs.begin()->second;
                }

                PhysicsObject* p = nullptr;
                if (physicsEnable)
                  p = new PhysicsObject(isFixed);

                GameObject* obj = new GameObject(program, objMesh, {position, rotation, scale}, p, {usesLight} );
                if (type == "object")
                    objs.push_back(obj);
                else
                    playerModel = obj;

            } else if (type == "background") {
                hasBackground = true;

                std::vector<std::string> faces;

                auto it = dataContainer.find("face1");
                if (it != dataContainer.end()) {
                    faces.push_back(std::any_cast<std::string>(it->second));
                } else {
                    throw std::runtime_error("background missing a face");
                }

                it = dataContainer.find("face2");
                if (it != dataContainer.end()) {
                    faces.push_back(std::any_cast<std::string>(it->second));
                } else {
                    throw std::runtime_error("background missing a face");
                }

                it = dataContainer.find("face3");
                if (it != dataContainer.end()) {
                    faces.push_back(std::any_cast<std::string>(it->second));
                } else {
                    throw std::runtime_error("background missing a face");
                }

                it = dataContainer.find("face4");
                if (it != dataContainer.end()) {
                    faces.push_back(std::any_cast<std::string>(it->second));
                } else {
                    throw std::runtime_error("background missing a face");
                }

                it = dataContainer.find("face5");

                if (it != dataContainer.end()) {
                    faces.push_back(std::any_cast<std::string>(it->second));
                } else {
                    throw std::runtime_error("background missing a face");
                }

                it = dataContainer.find("face6");
                if (it != dataContainer.end()) {
                    faces.push_back(std::any_cast<std::string>(it->second));
                } else {
                    throw std::runtime_error("background missing a face");
                }

                cubemapTexture = AssetLoader::createCubeMap(faces);

            } else if (type == "") {

            } else if (type == "light") {
                
            } else if (type == "camera") {
                auto it = dataContainer.find("position");
                glm::vec3 position;
                if (it != dataContainer.end()) {
                    position = std::any_cast<glm::vec3>(it->second);
                } else {
                    position = glm::vec3();
                }

                it = dataContainer.find("fov");
                int fov;
                if (it != dataContainer.end()) {
                    fov = std::any_cast<int>(it->second);
                } else {
                    fov = 45;
                }

                it = dataContainer.find("up");
                glm::vec3 startUp;
                if (it != dataContainer.end()) {
                    startUp = std::any_cast<glm::vec3>(it->second);
                } else {
                    startUp = glm::vec3(0,1,0);
                }

                it = dataContainer.find("yaw");
                float startYaw;
                if (it != dataContainer.end()) {
                    startYaw = std::any_cast<float>(it->second);
                } else {
                    startYaw = 0.0f;
                }

                it = dataContainer.find("pitch");
                float startPitch;
                if (it != dataContainer.end()) {
                    startPitch = std::any_cast<float>(it->second);
                } else {
                    startPitch = 0.0f;
                }
                c = new Camera(position, fov, GlobalSettings::WindowSettings::aspectRatio, startUp, startYaw, startPitch);

            }else {
                dataContainer.clear();
                throw std::runtime_error("bad type");
            }
            dataContainer.clear();
        } catch (const std::exception& e) {
            std::cout << "failed on line " << lineNum << " skipping" << std::endl;
            std::cout << e.what() << std::endl;
        }
    }
    if (!c){
        throw std::runtime_error("no camera");
    }

    return { c, std::move(objs), playerModel,  hasBackground ? backgoundPro : nullptr, cubemapTexture };
}

// kinda temp
Scene SceneLoader::loadFromFile(std::string filePath, ShaderProgram *backgoundPro, ShaderProgram *scenePro, ShaderProgram *noLightPro) {
    std::map<std::string, ShaderProgram*> programs = {
        {"light", scenePro},
        {"nolight", noLightPro},
    };
    return parseFile(filePath, programs, backgoundPro);
}
