#include "network.h"

wifi_mode_t Network::networkMode = wifi_mode_t::WIFI_MODE_NULL;
int Network::discoveredNetworks = WiFi.scanNetworks();
IPAddress Network::ipAddress = IPAddress(0, 0, 0, 0);
int Network::channel = -1;
int Network::maxConnections = -1;
int Network::hidden = -1;

std::vector<VoidFunc> Network::onDisconnect = {};

NetworkConfig Network::GetNetworkConfig()
{
    return NetworkConfig
    {
        false,
        networkMode,
        WiFi.SSID(),
        ipAddress,
        channel,
        maxConnections,
        hidden
    };
}

NetworkConfig Network::SetupAPNetwork(const char *ssid, const char *password, int _channel, int _maxConnections, int _hidden)
{
    if (networkMode != wifi_mode_t::WIFI_MODE_NULL) { return NetworkConfig { true }; }

    if (ssid != NULL)
    {
        for (int i = 0; i < discoveredNetworks; ++i)
        {
            if (WiFi.SSID(i) == ssid)
            {
                break;
            }
        }
    }
    else
    {
        long ssidRandom;
        while (true)
        {
            ssidRandom = random(1000, 9999);
            boolean ssidMatch = false;
            for (int i = 0; i < discoveredNetworks; ++i)
            {
                if ("esp32_" + String(ssidRandom) == WiFi.SSID(i))
                {
                    ssidMatch = true;
                    break;
                }
            }
            if (!ssidMatch) { break; }
        }
        ssid = ("esp32_" + String(ssidRandom)).c_str();
    }

    WiFi.mode(WIFI_AP);
    bool apSetupError = WiFi.softAP(ssid, password, _channel, _maxConnections, _hidden);
    if (apSetupError)
    {
        WiFi.mode(WIFI_OFF);
        return NetworkConfig { true };
    }

    networkMode = wifi_mode_t::WIFI_MODE_AP;
    ipAddress = WiFi.softAPIP();
    channel = _channel;
    maxConnections = _maxConnections;
    hidden = _hidden;

    Storage::PutInt("network", "networkMode", networkMode);
    Storage::PutString("network", "ssid", ssid);
    Storage::PutString("network", "password", password);
    Storage::PutInt("network", "channel", _channel);
    Storage::PutInt("network", "maxConnections", _maxConnections);
    Storage::PutInt("network", "hidden", _hidden);

    return NetworkConfig
    {
        false,
        networkMode,
        ssid,
        ipAddress,
        channel,
        maxConnections,
        hidden
    };
}

NetworkConfig Network::SetupSTANetwork(const char *ssid, const char *password)
{
    if (networkMode != wifi_mode_t::WIFI_MODE_NULL) { return NetworkConfig { true }; }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    int connectionAttempts = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
        feedLoopWDT();
        if (connectionAttempts++ > 10)
        {
            WiFi.mode(WIFI_OFF);
            return NetworkConfig { true };
        }
    }

    networkMode = wifi_mode_t::WIFI_MODE_STA;
    ipAddress = WiFi.localIP();
    Storage::PutInt("network", "networkMode", networkMode);
    Storage::PutString("network", "ssid", ssid);
    Storage::PutString("network", "password", password);

    return NetworkConfig
    {
        false,
        networkMode,
        ssid,
        ipAddress,
        WiFi.channel(),
        -1,
        -1
    };
}

NetworkConfig Network::LoadConfig()
{
    if (networkMode != wifi_mode_t::WIFI_MODE_NULL) { return NetworkConfig { true }; }

    int networkMode = Storage::GetInt("network", "networkMode", wifi_mode_t::WIFI_MODE_NULL);
    const char *ssid = Storage::GetString("network", "ssid", NULL).c_str();
    const char *password = Storage::GetString("network", "password", NULL).c_str();

    if (networkMode == wifi_mode_t::WIFI_MODE_AP)
    {
        int channel = Storage::GetInt("network", "channel", 1);
        int maxConnections = Storage::GetInt("network", "maxConnections", 4);
        int hidden = Storage::GetInt("network", "hidden", 0);
        return SetupAPNetwork(ssid, password, channel, maxConnections, hidden);
    }
    else if (networkMode == wifi_mode_t::WIFI_MODE_STA && ssid != NULL)
    {
        return SetupSTANetwork(ssid, password);
    }
    else
    {
        WiFi.mode(WIFI_MODE_NULL);
        networkMode = wifi_mode_t::WIFI_MODE_NULL;
        return NetworkConfig
        {
            false,
            wifi_mode_t::WIFI_MODE_NULL
        };
    }
}

void Network::Disconnect()
{
    for (int i = 0; i < onDisconnect.size(); i++)
    {
        onDisconnect[i]();
    }
    WiFi.disconnect();
    networkMode = wifi_mode_t::WIFI_MODE_NULL;
    ipAddress = IPAddress(0, 0, 0, 0);
}
