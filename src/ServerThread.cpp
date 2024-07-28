#include "Networking/ServerThread.h"

ServerThread::ServerThread(const u_short udp_port_num, const u_short tcp_port_num)
    : UDPport(udp_port_num), TCPport(tcp_port_num) {
    struct sockaddr_in address;
    int opt = 1;

    // tcp setup
    if ((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("socket failed");
        throw std::exception();
    }

    // Forcefully attaching socket to the port
    if (setsockopt(tcpSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
        perror("setsockopt");
        closesocket(tcpSocket);
        throw std::exception();
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCPport);

    if (bind(tcpSocket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        perror("bind failed");
        closesocket(tcpSocket);
        throw std::exception();
    }

    if (listen(tcpSocket, 10) == SOCKET_ERROR) {
        perror("listen");
        closesocket(tcpSocket);
        throw std::exception();
    }

    // udp setup
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        throw std::exception();
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDPport);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(udpSocket);
        
        throw std::exception();
    }

    TcpThread = std::thread(&ServerThread::ListenAndProcessConnections, this);
    UdpThread = std::thread(&ServerThread::UDPListen, this);
}

ServerThread::~ServerThread() {
    terminateAll_.store(true, std::memory_order_release);
    if (TcpThread.joinable())
        TcpThread.join();
    
    if (UdpThread.joinable())
        UdpThread.join();

    closesocket(tcpSocket);
    closesocket(udpSocket);
    
}

 void ServerThread::handleClient(SOCKET clientSocket) {
    try {
        char buffer[512];
        int bytesReceived = recv(clientSocket, buffer, 512, 0);
        if (bytesReceived == SOCKET_ERROR || strcmp(buffer, "Hello, server!") != 0) {
            throw std::runtime_error("Received incorrect message or recv error");
        }

        // Send filename to client
        std::string filename = "example.dat";
        send(clientSocket, filename.c_str(), filename.length(), 0);

        // Wait for client response about file existence
        char clientResponse;
        recv(clientSocket, &clientResponse, sizeof(clientResponse), 0);
        if (clientResponse == '0') {
            SendFile(clientSocket, filename);
        }

        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        if (getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrLen) == SOCKET_ERROR) {
            std::cerr << "Failed to get client address: " << WSAGetLastError() << std::endl;
            throw std::exception();
        }
        
        SetupUdpForClient(clientAddr);
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    closesocket(clientSocket);
}

void ServerThread::ListenAndProcessConnections() {
    while (true) {
        SOCKET new_socket = accept(tcpSocket, NULL, NULL);
        if (new_socket == INVALID_SOCKET) {
            perror("accept");
            continue;
        }

        std::thread clientThread(&ServerThread::handleClient, this, new_socket);
        clientThread.detach();
    }
}

void ServerThread::SendFile(SOCKET sock, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    send(sock, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0);

    char buffer[1024];
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        int bytesToSend = file.gcount();
        send(sock, buffer, bytesToSend, 0);
    }

    file.close();
}

void ServerThread::UDPListen() {
    char buffer[Networking::kSinglePacketSize];
    sockaddr_in si_other;
    int si_other_len = sizeof(si_other);
    int recv_len;

    while (true) {
        recv_len = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&si_other, &si_other_len);
        if (recv_len == SOCKET_ERROR) {
            std::cerr << "recvfrom() failed with error code: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::unique_lock<std::mutex> lock(udpClientsMutex);
        const auto it = udpClients.find(si_other.sin_addr.s_addr);
        if (it == udpClients.end()) {
            std::cout << "Received packet from unregistered IP: " << inet_ntoa(si_other.sin_addr) << std::endl;
            continue;
        }
        uint8_t checksum = it->second.checksum;
        
        // no packet order guarentee. Might need packet numbers, but for now everything should fit in one packet
        bool next_is_data = false;
        bool last_packet = false;
        int i = 0;
        for (; i < recv_len; ++i) {
            char currentByte = buffer[i];
            checksum ^= (uint8_t)currentByte;
            if (!next_is_data && currentByte == Networking::kDelimiter) {
                last_packet = true;
                break;
            }

            if (currentByte == Networking::kEscape) {
                if (!next_is_data) {
                    next_is_data = true;
                    continue;
                } else {
                    next_is_data = false;
                }
            }

            if (it->second.valid_bits >= Networking::kPlayerPacketDataSize) {
                std::cout << "Too much udp data" << std::endl;
                it->second.valid_bits = 0;
            }
            it->second.buffer[it->second.valid_bits]  = currentByte;
            it->second.valid_bits += 1;
        }
        if (last_packet) {
            if (i < recv_len) {
                if (i + 1 != recv_len)
                    std::cout << "More data in packet?" << std::endl;

                if (checksum != buffer[recv_len - 1]) {
                    std::cout << "Bad check sum" << std::endl;
                } else {
                    if (it->second.valid_bits != Networking::kPlayerPacketDataSize) {
                        std::cout << "not enough valid bits?" << std::endl;
                    } else {
                        std::unique_lock<std::mutex> lock(latestValidDataMutex);
                        memcpy(&latestValidData[si_other.sin_addr.s_addr], buffer, sizeof(it->second.buffer));
                    }
                }
            }
            it->second.valid_bits = 0;
            it->second.checksum = 0;
        } else {
            it->second.checksum = checksum;
        }
    }
}

void ServerThread::SetupUdpForClient(const sockaddr_in& clientAddr) {
    // Aquire only one lock at a time
    {
        std::unique_lock<std::mutex> lock(udpClientsMutex);
        udpClients[clientAddr.sin_addr.s_addr] = {};
    }
    {
        std::unique_lock<std::mutex> lock(latestValidDataMutex);
        latestValidData[clientAddr.sin_addr.s_addr] = {};
    }

}