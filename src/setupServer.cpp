#include "setupServer.h"

SetupServer::SetupServer(WebServer **webServer)
{
    this->webServer = webServer;

    if(!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS.");
        Display::Log("SPIFFS Err");
        return;
    }

    const NetworkConfig setupNetworkConfig = Network::SetupAPNetwork(NULL, NULL, 0, 4, 0, false);
    Display::Clear();

    (**this->webServer).server->on("/assets/css/main.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/assets/css/main.css", "text/html"); });
    (**this->webServer).server->on("/assets/js/main.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/assets/js/main.js", "application/javascript"); });

    (**this->webServer).server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) { request->redirect("/network/"); });
    (**this->webServer).server->on("/network/", HTTP_GET, [this](AsyncWebServerRequest *request) { request->send(SPIFFS, "/network/index.html", "text/html"); });
    (**this->webServer).server->on("/network/network.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/network/network.js", "application/javascript"); });
    (**this->webServer).server->on("/network/get", HTTP_GET, SetupServer::GETNetworks);
    //Couldnt get 'AsyncCallbackJsonWebHandler' to work here.
    (**this->webServer).server->on("/network/post", HTTP_POST, SetupServer::POSTNetwork);
    (**this->webServer).server->on("/sleep", HTTP_POST, SetupServer::POSTSleep);
    (**this->webServer).server->on("/reboot", HTTP_POST, SetupServer::POSTReboot);
    (**this->webServer).server->on("/resetESP", HTTP_POST, SetupServer::POSTResetESP);
    (**this->webServer).server->begin();

    Display::Clear();
    Display::DrawString(0, 0, "Setup required.", u8x8_font_amstrad_cpc_extended_f);
    Display::DrawString(0, 1, "Join network");
    Display::DrawString(0, 2, "SSID & visit IP");
    Display::DrawString(0, 3, "in your browser.");
    Display::DrawTwoPartString(0, 5, "SSID: ", setupNetworkConfig.ssid.c_str());
    Display::DrawTwoPartString(0, 6, "IP: ", setupNetworkConfig.ip.toString().c_str());
}

//In the future I would like to rescan for networks but I currently get a watchdog error when doing so.
void SetupServer::GETNetworks(AsyncWebServerRequest *request)
{
    ////JSON_OBJECT_SIZE(2)
    /*
    {
        "error": bool,
        "data":
        {
            //JSON_OBJECT_SIZE(7)
            "config":
            {
                "networkMode": int,
                "ssid": string,
                "password": bool,
                "ip": string,
                "channel": int,
                "maxConnections": int,
                "hidden": int
            },
            //JSON_ARRAY_SIZE(networks) + ((networks) * JSON_OBJECT_SIZE(3))
            "discoveredNetworks":
            [
                {
                    "ssid": string,
                    "rssi": int,
                    "encryptionType": int
                }
            ]
        }
    }
    */

    String response;

    try
    {
        int networks = Network::GetDiscoveredNetworks();

        //I hope what I'm calculating is correct and I'm not assigning way too much memory to the object.
        size_t networksJsonSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7) + JSON_ARRAY_SIZE(networks) + ((networks) * JSON_OBJECT_SIZE(3));
        DynamicJsonDocument doc(networksJsonSize);

        doc.getOrAddMember("error").set(false);
        JsonObject data = doc.createNestedObject("data");

        JsonObject config = data.createNestedObject("config");
        NetworkConfig networkConfig = Network::GetNetworkConfig();
        config["networkMode"] = networkConfig.networkMode;
        config["ssid"] = networkConfig.ssid;
        config["usesPassword"] = networkConfig.password;
        config["ip"] = networkConfig.ip.toString();
        config["channel"] = networkConfig.channel;
        config["maxConnections"] = networkConfig.maxConnections;
        config["hidden"] = networkConfig.hidden;

        JsonArray discoveredNetworks = data.createNestedArray("discoveredNetworks");
        for (int i = 0; i < networks; ++i)
        {
            JsonObject obj = discoveredNetworks.createNestedObject();
            obj["ssid"] = WiFi.SSID(i);
            obj["rssi"] = WiFi.RSSI(i);
            obj["encryptionType"] = WiFi.encryptionType(i);
        }

        serializeJson(doc, response);
        //I believe that arduinoJSON handles the memory unallocation automatically.
    }
    catch (const std::exception &e)
    {
        Display::Log(e.what());
        Serial.println(e.what());
        response = "{\"error\":true,\"data\":" + String(e.what()) + "}";
    }

    request->send(200, "application/json", response);
}

//Check if the network type should be set to STA or AP.
void SetupServer::POSTNetwork(AsyncWebServerRequest *request)
{
    if (!request->hasArg("method"))
    {
        request->send(200, "application/json", "{\"error\":true,\"data\":\"NO_METHOD_FOUND\"}");
        return;
    }
    else if (request->arg("method") == "STA")
    {
        if (!request->hasArg("ssid"))
        {
            request->send(200, "application/json", "{\"error\":true,\"data\":\"INVALID_DATA\"}");
            return;
        }

        String ssid = request->arg("ssid");
        String password = request->hasArg("password") ? request->arg("password") : "";

        NetworkConfig networkConfig = Network::SetupSTANetwork(ssid.c_str(), password.length() != 0 ? password.c_str() : NULL, true);
        if (!networkConfig.error)
        {
            Storage::PutBool("network", "configured", true);
            request->send(
                200,
                "application/json",
                "{\"error\":" + String(networkConfig.error) + ",\"data\":{\"ip\":\"" + networkConfig.ip.toString() + "\"}}"
            );
            Program::Restart();
        }
        else
        {
            request->send(
                200,
                "application/json",
                "{\"error\":" + String(networkConfig.error) + ",\"data\":{}}"
            );
        }
    }
    else if (request->arg("method") == "AP")
    {
        if (!request->hasArg("ssid"))
        {
            request->send(200, "application/json", "{\"error\":true,\"data\":\"INVALID_DATA\"}");
            return;
        }

        //This could fail as the network will be changed so the user will never get the response.
        //And even if the connection is unsuccessful the user will never get the response as the ESP wouldve tried to create a new network.
        //As the connection will be lost if the ESP fails to create the network the device should be restarted.

        String response;
        String password = request->hasArg("password") ? request->arg("password") : "";

        NetworkConfig networkConfig = Network::SetupAPNetwork(
            request->arg("ssid").c_str(), password.length() != 0 ? password.c_str() : NULL,
            request->hasArg("channel") ? request->arg("channel").toInt() : 1,
            request->hasArg("maxConnections") ? request->arg("hidden").toInt() : 4,
            request->hasArg("hidden") ? request->arg("hidden").toInt() : false,
            true
        );

        if (!networkConfig.error)
        {
            Storage::PutBool("network", "configured", true);
            response = "{\"error\":" + String(networkConfig.error) + ",\"data\":{\"ip\":\"" + networkConfig.ip.toString() + "\"}}";
        }
        else
        {
            response = "{\"error\":" + String(networkConfig.error) + ",\"data\":{}}";
        }

        request->send(200, "application/json", response);
        Program::Restart();
    }
    else
    {
        request->send(200, "application/json", "{\"error\":true,\"data\":\"INVALID_METHOD\"}");
    }
}

void SetupServer::POSTSleep(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    Program::DeepSleep();
}

void SetupServer::POSTReboot(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    Program::Restart();
}

void SetupServer::POSTResetESP(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    Program::ResetESP();
}