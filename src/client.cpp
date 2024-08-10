#include "client.h"

// Constructor
Client::Client(uint32_t id, std::function<void(Packet &)> callback, ISocket* socketInterface)
    : id(id), passPacketCom(callback), connected(false), socketInterface(socketInterface)
{
    if ((clientSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "failed to create a client socket");
        return;
    }
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "create a client socket");
    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &servAddress.sin_addr);
}

// Requesting a connection to the server
int Client::connectToServer()
{
    if (socketInterface->connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress)) < 0) {
        Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "error to connect to server");
        return -1;
    }

    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "connection succeed to server socket");
    connected = true;

#ifndef BROADCAST_MODE
    Packet packet(id);
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        Logger::packetLogMessage(std::to_string(id), LogLevel::ERROR, "sending id for init failed", packet);
        return -1;
    }
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending id for init succeed", packet);
#endif

    std::thread(&Client::receivePacket, this).detach();
    return 0;
}

// Sends the packet to the manager-sync
int Client::sendPacket(Packet &packet)
{
    if (!connected)
        return -1;

    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        Logger::packetLogMessage(std::to_string(id), LogLevel::ERROR, "sending failed", packet);
        return -1;
    }
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending succeed", packet);
    return 0;
}

// Sends the packet to the manager-async
int Client::sendPacketAsync(Packet &packet)
{
    // std::thread(&Client::sendPacket, this, packet).detach();
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending succeed", packet);
    return 0;
}

// Waits for a message and forwards it to Communication
void Client::receivePacket()
{
    Packet packet;
    while (connected) {
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0) {
            Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "Connection closed or error occurred in client socket number: " + std::to_string(clientSocket));
            break;
        }
        Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "received packet", packet);
        passPacketCom(packet);
    }
}

// Closes the connection
void Client::closeConnection()
{
    socketInterface->close(clientSocket);
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "close client socket number: " + std::to_string(clientSocket));
    connected = false;
}

int Client::getClientSocket()
{
    return clientSocket;
}

int Client::isConnected()
{
    return connected;
}

// Destructor
//Client::~Client() {
    //closeConnection();
   // delete socketInterface; 
//}