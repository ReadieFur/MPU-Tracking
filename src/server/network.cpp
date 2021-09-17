#include "network.h"

int Network::discoveredNetworks = 0;
NetworkConfig Network::config = NetworkConfig
{
    false,
    wifi_mode_t::WIFI_MODE_NULL,
    "",
    false,
    IPAddress(0, 0, 0, 0),
    -1,
    -1,
    false
};

std::vector<VoidFunc> Network::onDisconnect = {};

void Network::ScanNetworks()
{
    discoveredNetworks = WiFi.scanNetworks();
}

int Network::GetDiscoveredNetworks()
{
    return discoveredNetworks;
}

NetworkConfig Network::GetNetworkConfig()
{
    return config;
}

NetworkConfig Network::SetupAPNetwork(const char *_ssid, const char *_password, int _channel, int _maxConnections, bool _hidden, bool _save)
{
    if (WiFi.getMode() != WIFI_MODE_APSTA) { WiFi.mode(WIFI_MODE_APSTA); }
    if (config.networkMode == wifi_mode_t::WIFI_MODE_AP) { return NetworkConfig { true }; }

    String ssid = _ssid;
    if (ssid == NULL)
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
        ssid = "esp32_" + String(ssidRandom);
    }

    Serial.println("Attempting to setup AP network with the following details - SSID: '" + ssid + "', Password: '" + _password + "', Channel: " + String(_channel) + ", Max Connections: " + String(_maxConnections) + ", Hidden: " + String(_hidden));

    // WiFi.mode(WIFI_AP);
    bool apSetupError = !WiFi.softAP(ssid.c_str(), _password == NULL || _password == "" ? (const char *) NULL : _password, _channel, _hidden, _maxConnections);
    if (apSetupError)
    {
        // WiFi.mode(WIFI_OFF);
        return NetworkConfig { true };
    }

    Serial.println("AP network setup successful. SSID: '" + ssid + "' IP: '" + WiFi.softAPIP().toString() + "'");

    config = NetworkConfig
    {
        false,
        wifi_mode_t::WIFI_MODE_AP,
        ssid,
        _password != NULL && _password != "",
        WiFi.softAPIP(),
        _channel,
        _maxConnections,
        _hidden
    };

    if (_save)
    {
        Storage::PutInt("network", "networkMode", wifi_mode_t::WIFI_MODE_AP);
        Storage::PutString("network", "ssid", ssid.c_str());
        Storage::PutString("network", "password", _password);
        Storage::PutInt("network", "channel", _channel);
        Storage::PutInt("network", "maxConnections", _maxConnections);
        Storage::PutInt("network", "hidden", _hidden);
    }

    return config;
}

NetworkConfig Network::SetupSTANetwork(const char *ssid, const char *password, bool save)
{
    if (WiFi.getMode() != WIFI_MODE_APSTA) { WiFi.mode(WIFI_MODE_APSTA); }
    if (config.networkMode == wifi_mode_t::WIFI_MODE_STA) { return NetworkConfig { true }; }

    Serial.println("Attempting to setup STA network on the following network - SSID: '" + String(ssid) + "'");

    // WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    if (password == NULL || password == "") { WiFi.begin(ssid); }
    else { WiFi.begin(ssid, password); }
    int connectionAttempts = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
        feedLoopWDT();
        if (connectionAttempts++ > 10)
        {
            // WiFi.mode(WIFI_OFF);
            return NetworkConfig { true };
        }
    }

    Serial.println("STA network setup successful. SSID: '" + String(ssid) + "' IP: '" + WiFi.localIP().toString() + "'");

    config = NetworkConfig
    {
        false,
        wifi_mode_t::WIFI_MODE_STA,
        ssid,
        password != NULL && password != "",
        WiFi.localIP(),
        -1,
        -1,
        false
    };

    if (save)
    {
        Storage::PutInt("network", "networkMode", wifi_mode_t::WIFI_MODE_STA);
        Storage::PutString("network", "ssid", ssid);
        if (password != NULL && password != "") { Storage::PutString("network", "password", password); }
    }

    return config;
}

NetworkConfig Network::LoadConfig()
{
    if (WiFi.getMode() != WIFI_MODE_APSTA) { WiFi.mode(WIFI_MODE_APSTA); }
    // if (config.networkMode != wifi_mode_t::WIFI_MODE_NULL) { return NetworkConfig { true }; }

    String ssid = Storage::GetString("network", "ssid", "");
    String password = Storage::GetString("network", "password", "");
    int networkMode = Storage::GetInt("network", "networkMode", wifi_mode_t::WIFI_MODE_NULL);

    if (networkMode == wifi_mode_t::WIFI_MODE_AP && ssid != NULL && ssid != "")
    {
        int channel = Storage::GetInt("network", "channel", 1);
        int maxConnections = Storage::GetInt("network", "maxConnections", 4);
        int hidden = Storage::GetInt("network", "hidden", 0);
        return SetupAPNetwork(ssid.c_str(), password != "" ? password.c_str() : NULL, channel, maxConnections, hidden, false);
    }
    else if (networkMode == wifi_mode_t::WIFI_MODE_STA && ssid != NULL && ssid != "")
    {
        return SetupSTANetwork(ssid.c_str(), password != "" ? password.c_str() : NULL, false);
    }
    else
    {
        // WiFi.mode(WIFI_MODE_NULL);
        return NetworkConfig
        {
            false,
            wifi_mode_t::WIFI_MODE_NULL,
            "",
            IPAddress(0, 0, 0, 0),
            -1,
            -1,
            -1
        };
    }
}

void Network::Disconnect()
{
    for (int i = 0; i < onDisconnect.size(); i++)
    {
        onDisconnect[i]();
    }
    if (config.networkMode == wifi_mode_t::WIFI_MODE_STA)
    {
        WiFi.disconnect();
    }
    WiFi.mode(WIFI_OFF);
    config = NetworkConfig
    {
        false,
        wifi_mode_t::WIFI_MODE_NULL,
        "",
        false,
        IPAddress(0, 0, 0, 0),
        -1,
        -1,
        false
    };
}