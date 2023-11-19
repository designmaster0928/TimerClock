#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

// Define your network settings
#define IP_ADDRESS      "192.168.0.100"
#define SUBNET_MASK     "255.255.255.0"
#define GATEWAY         "192.168.0.1"
#define NTP_SERVER      "time.nist.gov"
#define NTP_PORT        123

// NTP packet structure
typedef struct {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;
    uint32_t refTm_s;
    uint32_t refTm_f;
    uint32_t origTm_s;
    uint32_t origTm_f;
    uint32_t rxTm_s;
    uint32_t rxTm_f;
    uint32_t txTm_s;
    uint32_t txTm_f;
} NTPPacket;

// Function to send an NTP request packet
void sendNTPRequest(int socket) {
    NTPPacket packet;

    memset(&packet, 0, sizeof(NTPPacket));
    packet.li_vn_mode = 0x1B;

    send(socket, (uint8_t *)&packet, sizeof(NTPPacket));
}

// Function to receive an NTP response packet
bool receiveNTPResponse(int socket, NTPPacket *packet) {
    if (recv(socket, (uint8_t *)packet, sizeof(NTPPacket)) > 0) {
        return true;
    }
    return false;
}

// Function to convert NTP timestamp to UNIX timestamp
uint32_t convertNTPtoUNIX(uint32_t ntpTime_s) {
    return ntpTime_s - 2208988800UL;
}

// Function to get time from NTP server
bool getTimeFromNTPServer(uint32_t *unixTime) {
    int socket = socket_open(SOCK_UDP, 0, 0);

    if (socket != -1) {
        if (socket_connect(socket, NTP_SERVER, NTP_PORT)) {
            sendNTPRequest(socket);

            NTPPacket response;
            if (receiveNTPResponse(socket, &response)) {
                *unixTime = convertNTPtoUNIX(response.txTm_s);
                socket_close(socket);
                return true;
            }
        }
        socket_close(socket);
    }
    return false;
}

void main() {
    // Set up your network settings
    // Configure the W5500 module with IP_ADDRESS, SUBNET_MASK, GATEWAY, etc.

    // Get the time from the NTP server
    uint32_t unixTime;
    if (getTimeFromNTPServer(&unixTime)) {
        // Use the received time as needed
        printf("Time received from NTP server: %lu\r\n", unixTime);
    } else {
        printf("Failed to get time from NTP server\r\n");
    }

    while (1) {
        // Your main application code here
    }
}