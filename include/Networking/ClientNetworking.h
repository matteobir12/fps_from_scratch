#ifndef CLIENTNETWORKING_H
#define CLIENTNETWORKING_H

#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <optional>

#include "CommonStructs.h"
#include "ThreadAtFreq.h"

#pragma comment(lib,"ws2_32.lib")

class ClientNetworking {
  public:
    // abstract to string
    /**
     * @brief Construct a new Connection to server at ip
     * Will first tcp handshake and trade assets and validate
     * Then will close tcp and use udp for active game data
     * 
     * @param ip
     */
    ClientNetworking(char* const ip, const u_short udp_port_num, const u_short tcp_port_num);

    ~ClientNetworking();

    void UpdateOutGoingData(Rendering::Point3D p, Rendering::Orientation o);

  private:
    std::thread tcpSyncAndSetupThread;
    std::optional<ThreadAtFreq> sendThread;
    std::thread recvThread;
    char* const serverIP = "127.0.0.1";
    const u_short UDPport;
    const u_short TCPport;
    SOCKET tcpSocket;
    SOCKET udpSocket;
    sockaddr_in udp_serverAddr;
    bool inUdpMode;
    std::atomic<bool> terminateAll_;
    std::vector<char> encodedData;
    Networking::ClientToServerNetworkData dataToSend;
    int tcpSync();
    void tcpSyncAndSetup();
    void SendUdp();

};

#endif
