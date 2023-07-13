#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// CAN interface name (e.g., can0, can1, etc.)
const char* canInterface = "can0";

// Function to initialize the CAN socket
int initCANSocket() {
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        std::cerr << "Failed to create CAN socket." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    struct sockaddr_can addr;
    struct ifreq ifr;

    std::strcpy(ifr.ifr_name, canInterface);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "Failed to retrieve CAN interface index." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind CAN socket to interface." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return s;
}

// Function to send a CAN message
void sendCANMessage(int s, uint32_t id, uint8_t* data, size_t len) {
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = len;
    std::memcpy(frame.data, data, len);

    if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
        std::cerr << "Failed to send CAN message." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Function to receive a CAN message
void receiveCANMessage(int s) {
    struct can_frame frame;
    if (read(s, &frame, sizeof(frame)) < 0) {
        std::cerr << "Failed to receive CAN message." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Received CAN message - ID: " << std::hex << frame.can_id
              << ", Data: ";
    for (int i = 0; i < frame.can_dlc; i++) {
        std::cout << std::hex << static_cast<int>(frame.data[i]) << " ";
    }
    std::cout << std::endl;
}

int main() {
    int s = initCANSocket();

    // Example CAN message
    uint32_t messageId = 0x123;
    uint8_t messageData[] = {0x01, 0x02, 0x03};
    size_t messageLen = sizeof(messageData);

    // Send the CAN message
    sendCANMessage(s, messageId, messageData, messageLen);

    // Receive a CAN message
    receiveCANMessage(s);

    close(s);
    return 0;
}
