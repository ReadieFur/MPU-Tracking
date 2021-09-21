#pragma once

#include "storage.h"
#include "program.h"
#include <WiFi.h>
#include <vector>

#ifndef VOIDFUNC_T
#define VOIDFUNC_T
typedef void(*VoidFunc)();
#endif

struct NetworkConfig
{
    bool error;
    wifi_mode_t networkMode;
    String ssid;
    bool password;
    IPAddress ip;
    int channel;
    int maxConnections;
    bool hidden;
};

class Network
{
    static int discoveredNetworks;
    static NetworkConfig config;

    Network() {};
    ~Network() {};

    public:
        static std::vector<VoidFunc> onDisconnect;

        static void ScanNetworks();
        static int GetDiscoveredNetworks();
        static NetworkConfig GetNetworkConfig();
        static NetworkConfig LoadConfig();
        static NetworkConfig SetupAPNetwork(const char *_ssid = NULL, const char *_password = NULL, int _channel = 1, int _maxConnections = 4, bool _hidden = false, bool _save = true);
        static NetworkConfig SetupSTANetwork(const char *ssid, const char *password = NULL, bool save = true);
        static void Disconnect();
};