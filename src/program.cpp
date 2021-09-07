#include "program.h"

bool Program::initialized = false;
WebServer *Program::webServer;

void Program::Init()
{
    if (Program::initialized != true)
    {
    	Serial.begin(115200);

        NetworkConfig networkConfig = Network::LoadConfig();
        webServer = new WebServer();

        if (networkConfig.networkMode == wifi_mode_t::WIFI_AP && Storage::GetBool("network", "configured", false))
        {
            //Self hosted network.
        }
        else if (networkConfig.networkMode == wifi_mode_t::WIFI_STA)
        {
            //Run on existing network.
        }
        else
        {
            //Configure new network.
        }

        Motion::Calibrate();
        Motion::Enable();

        initialized = true;
    }
}

void Program::Dispose()
{
    Network::Disconnect();
	ESP.restart();
}