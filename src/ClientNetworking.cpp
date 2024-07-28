#include "Networking/ClientNetworking.h"

#include <algorithm>
#include <filesystem>

#include "GlobalSettings.h"

namespace {
void receiveFile(SOCKET sock, const std::string& filename) {
    std::ofstream file(GlobalSettings::AssetSettings::mapDir / filename, std::ios::binary);
    size_t fileSize;
    recv(sock, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

    char buffer[1024];
    size_t totalBytesReceived = 0;
    std::cout << "recieving file" << std::endl;
    while (totalBytesReceived < fileSize) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        file.write(buffer, bytesReceived);
        totalBytesReceived += bytesReceived;
        // progress bar based on totalBytesReceived / fileSize
    }

    file.close();
}

}

ClientNetworking::ClientNetworking(char* const ip, const u_short udp_port_num, const u_short tcp_port_num) 
    : serverIP(ip), UDPport(udp_port_num), TCPport(tcp_port_num), inUdpMode(false), terminateAll_({false}) {
    // Pre allocate
    // 5 = 1 for checksum + 1 for delimiter + 3 for expected escaped characters
    encodedData.reserve(sizeof(Networking::ClientToServerNetworkData) + 5);
    tcpSyncAndSetupThread = std::thread(&ClientNetworking::tcpSyncAndSetup, this);
}

ClientNetworking::~ClientNetworking() {
    terminateAll_.store(true, std::memory_order_release);
    if (tcpSyncAndSetupThread.joinable()) {
        tcpSyncAndSetupThread.join();
    }

    if (sendThread)
        sendThread.value().stop();

    if (recvThread.joinable()) {
        // stop recv block
        recvThread.join();
    }
}

void ClientNetworking::UpdateOutGoingData(Rendering::Point3D p, Rendering::Orientation o){
    dataToSend.outGoingPoint = p.ToNetworkPoint();
    dataToSend.outGoingOrientation = o.ToNetworkOrientation();
}

void ClientNetworking::tcpSyncAndSetup() {
    if (!tcpSync()) {
        inUdpMode = true;
        udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (udpSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
            
            throw std::exception();
        }

        udp_serverAddr.sin_family = AF_INET;
        udp_serverAddr.sin_port = htons(UDPport);
        inet_pton(AF_INET, serverIP, &udp_serverAddr.sin_addr); 
        if (bind(udpSocket, (struct sockaddr*)&udp_serverAddr, sizeof(udp_serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error code: " << WSAGetLastError() << std::endl;
            closesocket(udpSocket);
            
            throw std::exception();
        }
        sendThread.emplace(std::bind(&ClientNetworking::SendUdp, this), GlobalSettings::NetworkSettings::clientTickRate, "send thread");
    } else {
        return;
    }
}

int ClientNetworking::tcpSync() {
    try {
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        inet_pton(AF_INET, serverIP, &address.sin_addr);
        address.sin_port = htons(TCPport);
        tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
        connect(tcpSocket, (struct sockaddr *)&address, sizeof(address));
        // Send hello/sync packet
        char* message = "Hello, server!";
        send(tcpSocket, message, strlen(message), 0);

        char server_reply[2000];
        recv(tcpSocket, server_reply, 2000, 0);
        std::string filename(server_reply);

        // Check if file exists
        char fileCheck = std::filesystem::exists(GlobalSettings::AssetSettings::mapDir / filename) ? '1' : '0';
        send(tcpSocket, &fileCheck, sizeof(fileCheck), 0);

        if (fileCheck == '0') {
            receiveFile(tcpSocket, filename);
        }

        closesocket(tcpSocket);
        

        return 0;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
}

void ClientNetworking::SendUdp() {

    // Serialize data
    char* rawData = (char*)&dataToSend;
    uint8_t checksum = 0;
    // no packet order guarentee. Might need packet numbers, but for now everything should fit in one packet
    for (size_t i = 0; i < sizeof(Networking::ClientToServerNetworkData); ++i) {
        char currentByte = rawData[i];
        if (currentByte == Networking::kDelimiter || currentByte == Networking::kEscape) {
            encodedData.push_back(Networking::kEscape);
            checksum ^= (uint8_t)Networking::kEscape;
        }
        encodedData.push_back(currentByte);
        checksum ^= (uint8_t)currentByte;
    }
    // Stop Char
    encodedData.push_back(Networking::kDelimiter);
    // * sizeof(char) for more obvious code
    encodedData.push_back(static_cast<char>(checksum));
    // Max packet size - check sum
    for (size_t i = 0; i < encodedData.size(); i += Networking::kSinglePacketSize) {
        // * sizeof(char) for more obvious code
        const size_t remaining_size = (encodedData.size() - i) * sizeof(char);
        const size_t chunkSize = std::min(Networking::kSinglePacketSize, remaining_size);
        if (sendto(udpSocket, &encodedData[i], chunkSize, 0, (struct sockaddr*)&udp_serverAddr, sizeof(udp_serverAddr)) == SOCKET_ERROR) {
            std::cout << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
        }
    }
    encodedData.clear();
}
