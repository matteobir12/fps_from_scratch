#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <atomic>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <optional>

#include "CommonStructs.h"
#include "GameObject.h"

#pragma comment(lib, "ws2_32.lib")

class ServerThread {
public:
ServerThread(const u_short udp_port_num, const u_short tcp_port_num);
~ServerThread();

template<class Func>
void FuncOnObjects(Func func) {
    std::mutex latestValidDataMutex;
    for (const auto& player_data : latestValidData)
        func(player_data.second);
}

private:
SOCKET tcpSocket;
SOCKET udpSocket;
const u_short UDPport;
const u_short TCPport;
std::thread TcpThread;
std::thread UdpThread;
std::atomic<bool> terminateAll_;
std::mutex udpClientsMutex;
std::unordered_map<u_long, Networking::RebuildingStruct> udpClients;
std::mutex latestValidDataMutex;
std::unordered_map<u_long, Networking::ClientToServerNetworkData> latestValidData;
void ListenAndProcessConnections();
void handleClient(const SOCKET clientSocket);
void SendFile(SOCKET sock, const std::string& filename);
void UDPListen();
void SetupUdpForClient(const sockaddr_in& clientAddr);

};

#endif