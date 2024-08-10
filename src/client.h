#pragma once
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include "message.h"
#include "logger.h"
#include "../sockets/Isocket.h"
#include "../sockets/real_socket.h"
#include "../sockets/mock_socket.h"
#include <string>

#define PORT 8080
#define IP "127.0.0.1"

class Client
{
private:
    uint32_t id;
    int clientSocket;
    sockaddr_in servAddress;
    bool connected;
    std::function<void(Packet &)> passPacketCom;
    ISocket* socketInterface; 

public:
    // Constructor
    Client(uint32_t id, std::function<void(Packet &)> callback, ISocket* socketInterface=new RealSocket());

    // Requesting a connection to the server
    int connectToServer();

    // Sends the packet to the manager-sync
    int sendPacket(Packet &packet);

    // Sends the packet to the manager-async
    int sendPacketAsync(Packet &packet);

    // Waits for a message and forwards it to Communication
    void receivePacket();

    // Closes the connection
    void closeConnection();

    int getClientSocket();

    int isConnected();
    // Destructor
    //~Client();
};