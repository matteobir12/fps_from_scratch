#include "GlobalSettings.h"

namespace GlobalSettings {

int WindowSettings::windowWidth = 1000;
int WindowSettings::windowHeight = 800;
float WindowSettings::aspectRatio = static_cast<float>(windowWidth) / windowHeight;

int NetworkSettings::serverTickRate = 30;
int NetworkSettings::clientTickRate = 30;
WSADATA NetworkSettings::wsaData;

int PhysicsSettings::physicsTickRate = 60;

int PlayerSetting::fov = 45;

fs::path AssetSettings::curDir = fs::current_path();
fs::path AssetSettings::mapDir = curDir / "maps/";

void NetworkSettings::startNetworkService() {
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void WindowSettings::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

} // GlobalSettings