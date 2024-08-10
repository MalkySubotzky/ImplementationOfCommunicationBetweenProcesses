#pragma once
#include <vector>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <iostream>

// Packet definition
struct Packet
{
    // Packet header - metadata according to CAN bus
    struct Header
    {
        uint32_t PSN;          // Packet Sequence Number
        uint32_t TPS;          // Total Packet Sum
        uint32_t SrcID;        // Source ID
        uint32_t DestID;       // Destination ID
        int DLC;               // Data Length Code (0-8 bits)
        uint16_t CRC;          // Cyclic Redundancy Check
        bool ackSlot;          // Acknowledgment slot
        bool errorFrame;       // Error signaling
        std::time_t timestamp; // Timestamp
    } header;

    // Maximum data size per packet is 8 bytes.
    uint8_t data[8]; 

    // Default constructor 
    Packet() = default;

    // Constructor to initialize Packet for sending
    Packet(uint32_t psn, uint32_t tps, uint32_t srcID, uint32_t destID, uint8_t *data, int dlc);
    
    // Constructor to initialize receiving Packet ID for init
    Packet(uint32_t id);
    
    // Calculate CRC for the given data and length
    uint16_t calculateCRC(const uint8_t *data, size_t length);
    
};

// Message definition
class Message
{
private:
    std::vector<Packet> packets; // List of packets in the message.
    uint32_t tps;                // Total Packet Sum

public:

    // Default constructor 
    Message() = default;

    // Constructor for sending message
    Message(uint32_t srcID, uint32_t destID, void *data, size_t size);
    
    // Constructor for receiving message
    Message(uint32_t tps);
    
    // Add a packet to the received message
    bool addPacket(const Packet &p);
    
    // Check if the message is complete
    bool isComplete() const;
    
    // Get the complete data of the message
    void *completeData() const;
    
    // Get the packets of the message
    std::vector<Packet> &getPackets();
    
};
