
#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <functional>
#include <map>
#include "message.h"
#include "logger.h"
#include "../sockets/Isocket.h"
#include "../sockets/real_socket.h"

class Server
{
private:
    int serverSocket;
    sockaddr_in address;
    int port;
    bool running;
    std::vector<std::thread> clientThreads;
    std::vector<int> sockets;
    std::mutex socketMutex;
    std::function<void(void *)> receiveData;
    std::map<int, uint32_t> clientIDMap;
    ISocket* socketInterface; // השתמש ב-ISocket במקום ב-socket ישירות

public:
    // Constructor
    Server(int port, std::function<void(void *)> callback, ISocket* socketInterface = new RealSocket());
    
    // Starts listening for connection requests
    int start();
    
    // Closes the sockets and the threads
    void stop();
    
    // Runs in a thread for each process - waits for a message and forwards it to the manager
    void handleClient(int clientSocket);
    
    // Returns the sockets ID
    int getClientSocketByID(uint32_t destID);
    
    // Sends the message to all connected processes - broadcast
    int sendToClients(const Packet &packet);

    int getServerSocket();

    // Destructor
    //~Server();
};