#pragma once

#include "storage.h"
#include <WiFi.h>
#include <vector>

typedef void(*VoidFunc)();

struct NetworkConfig
{
    bool error;
    wifi_mode_t networkMode;
    String ssid;
    IPAddress ip;
    int channel;
    int maxConnections;
    int hidden;
} ;

class Network
{
    static wifi_mode_t networkMode;
    static int discoveredNetworks;
    static IPAddress ipAddress;
    static int channel;
    static int maxConnections;
    static int hidden;

    Network() {};
    ~Network() {};

    public:
        static std::vector<VoidFunc> onDisconnect;

        static NetworkConfig GetNetworkConfig();
        static NetworkConfig LoadConfig();
        static NetworkConfig SetupAPNetwork(const char *ssid = NULL, const char *password = NULL, int channel = 1, int maxConnections = 4, int hidden = 0);
        static NetworkConfig SetupSTANetwork(const char *ssid, const char *password = NULL);
        static void Disconnect();
};