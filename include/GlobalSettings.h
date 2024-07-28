#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <filesystem>
#include <Winsock2.h>

namespace fs = std::filesystem;

namespace GlobalSettings {

struct WindowSettings {
    static int windowWidth;
    static int windowHeight;
    static float aspectRatio;
    static void setWindowSize(int width, int height);
};

struct NetworkSettings {
    static int serverTickRate;
    static int clientTickRate;
    static WSADATA wsaData;
    static void startNetworkService();
};

struct PhysicsSettings {
  static int physicsTickRate;
};

struct PlayerSetting {
  static int fov;
};

struct AssetSettings {
  static fs::path curDir;
  static fs::path mapDir;
};

}  // GlobalSettings


#endif