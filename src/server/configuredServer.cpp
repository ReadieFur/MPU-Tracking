#include "configuredServer.h"
#include <functional>

EDisplayMode ConfiguredServer::displayMode = EDisplayMode::DISPLAY_ENABLED;
EventData<MotionData> ConfiguredServer::updateDisplayEvent(ConfiguredServer::UpdateDisplay);
long ConfiguredServer::lastDisplayUpdate = 0;

ConfiguredServer::ConfiguredServer(WebServer **webServer)
{
    this->webServer = webServer;

    motionUpdatedEvent = new EventData<MotionData>(
        [](void* context, MotionData data)
        {
            static_cast<ConfiguredServer*>(context)->MotionUpdated(data);
        },
        this
    );
    Motion::motionUpdated.Register(*motionUpdatedEvent);

    //Shared resources
    (**this->webServer).server->on("/assets/css/main.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(200, "text/css", WebPages::main_css); });
    (**this->webServer).server->on("/assets/js/main.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(200, "application/javascript", WebPages::main_js); });

    //From SetupServer
    (**this->webServer).server->on("/network/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(200, "text/html", WebPages::network_html); });
    (**this->webServer).server->on("/network/network.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(200, "application/javascript", WebPages::network_js); });
    (**this->webServer).server->on("/network/get", HTTP_GET, SetupServer::GETNetworks);
    //Couldnt get 'AsyncCallbackJsonWebHandler' to work here.
    (**this->webServer).server->on("/network/post", HTTP_POST, SetupServer::POSTNetwork);
    (**this->webServer).server->on("/sleep", HTTP_POST, SetupServer::POSTSleep);
    (**this->webServer).server->on("/reboot", HTTP_POST, SetupServer::POSTReboot);
    (**this->webServer).server->on("/resetESP", HTTP_POST, SetupServer::POSTResetESP);

    //From ConfiguredServer
    (**this->webServer).server->on("/display", HTTP_POST, ConfiguredServer::POSTDisplay);

    (**this->webServer).server->begin();

    NetworkConfig networkConfig = Network::GetNetworkConfig();

    Display::Clear();
    Display::DrawString(0, 0, (String(networkConfig.networkMode == wifi_mode_t::WIFI_MODE_STA ? "STA" : "AP") + " server.").c_str(), u8x8_font_amstrad_cpc_extended_f);
    Display::DrawTwoPartString(0, 1, "SSID: ", networkConfig.ssid.c_str());
    String ip = networkConfig.ip.toString();
    Display::DrawString(0, 2, "IP Address:", u8x8_font_amstrad_cpc_extended_f);
    Display::DrawString(0, 3, ip.c_str());

    Serial.println((EDisplayMode)Storage::GetInt("configuredServer", "displayMode", EDisplayMode::DISPLAY_ENABLED));

    ConfiguredServer::SetDisplayMode((EDisplayMode)Storage::GetInt("configuredServer", "displayMode", EDisplayMode::DISPLAY_ENABLED));

    Motion::Enable();
}

void ConfiguredServer::MotionUpdated(MotionData motionData)
{
    /*
    {
        "gyroscope":
        [
            0.0,
            0.0,
            0.0
        ],
        // "rotation":
        // [
        //     0.0,
        //     0.0,
        //     0.0
        // ],
        "accelerometer":
        [
            0.0,
            0.0,
            0.0
        ],
        // "position":
        // [
        //     0.0,
        //     0.0,
        //     0.0
        // ]
    }
    */
    String json;
    DynamicJsonDocument doc(motionJsonSize);
    JsonArray gyroscope = doc.createNestedArray("gyroscope");
    gyroscope.add(motionData.gyroscope.v[0]);
    gyroscope.add(motionData.gyroscope.v[1]);
    gyroscope.add(motionData.gyroscope.v[2]);
    // JsonArray rotation = doc.createNestedArray("rotation");
    // rotation.add(motionData.rotation.v[0]);
    // rotation.add(motionData.rotation.v[1]);
    // rotation.add(motionData.rotation.v[2]);
    JsonArray accelerometer = doc.createNestedArray("accelerometer");
    accelerometer.add(motionData.accelerometer.v[0]);
    accelerometer.add(motionData.accelerometer.v[1]);
    accelerometer.add(motionData.accelerometer.v[2]);
    // JsonArray position = doc.createNestedArray("position");
    // position.add(motionData.position.v[0]);
    // position.add(motionData.position.v[1]);
    // position.add(motionData.position.v[2]);
    serializeJson(doc, json);
    (**this->webServer).websocket->textAll(json);
    // doc.~DynamicJsonDocument();

    //Create a serial log toggle.
    // Serial.println(json);
}

//This dramatically reduces the clock speed of the microcontroller. It is reccomended to not use this function.
//This is becasuse writing to the display is very slow.
void ConfiguredServer::UpdateDisplay(void* context, MotionData motionData)
{
    long currentTime = millis();
    if (currentTime - lastDisplayUpdate > 1000)
    {
        const float devisionFactor = 10.0f;

        //It seems that the String here always keeps two decimals, so I shall substring the first three characters (four if it is negative).
        String a0 = String((float)((int)(motionData.accelerometer.v[0] * devisionFactor)) / devisionFactor);
        String a1 = String((float)((int)(motionData.accelerometer.v[1] * devisionFactor)) / devisionFactor);
        String a2 = String((float)((int)(motionData.accelerometer.v[2] * devisionFactor)) / devisionFactor);
        if (a0.startsWith("-")) { a0 = a0.substring(0, 4); } else { a0 = a0.substring(0, 3); }
        if (a1.startsWith("-")) { a1 = a1.substring(0, 4); } else { a1 = a1.substring(0, 3); }
        if (a2.startsWith("-")) { a2 = a2.substring(0, 4); } else { a2 = a2.substring(0, 3); }
        Display::DrawTwoPartString(0, 4, "A ", (a0 + " " + a1 + " " + a2).c_str());

        String g0 = String((float)((int)(motionData.gyroscope.v[0] * devisionFactor)) / devisionFactor);
        String g1 = String((float)((int)(motionData.gyroscope.v[1] * devisionFactor)) / devisionFactor);
        String g2 = String((float)((int)(motionData.gyroscope.v[2] * devisionFactor)) / devisionFactor);
        if (g0.startsWith("-")) { g0 = g0.substring(0, 4); } else { g0 = g0.substring(0, 3); }
        if (g1.startsWith("-")) { g1 = g1.substring(0, 4); } else { g1 = g1.substring(0, 3); }
        if (g2.startsWith("-")) { g2 = g2.substring(0, 4); } else { g2 = g2.substring(0, 3); }
        Display::DrawTwoPartString(0, 5, "G ", (g0 + " " + g1 + " " + g2).c_str());

        lastDisplayUpdate = currentTime;
    }
}

void ConfiguredServer::POSTDisplay(AsyncWebServerRequest *request)
{
    if (!request->hasArg("method"))
    {
        request->send(200, "application/json", "{\"error\":true,\"data\":\"NO_METHOD_FOUND\"}");
    }
    else if (request->arg("method") == "enable")
    {
        switch (displayMode)
        {
            case EDisplayMode::DISPLAY_DISABLED:
                SetDisplayMode(EDisplayMode::DISPLAY_ENABLED);
                break;
            case EDisplayMode::UPDATES_ENABLED_DISPLAY_DISABLED:
                SetDisplayMode(EDisplayMode::UPDATES_ENABLED_DISPLAY_ENABLED);
                break;
        }
        request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    }
    else if (request->arg("method") == "enableUpdates")
    {
        switch (displayMode)
        {
            case EDisplayMode::DISPLAY_DISABLED:
                SetDisplayMode(EDisplayMode::UPDATES_ENABLED_DISPLAY_DISABLED);
                break;
            case EDisplayMode::DISPLAY_ENABLED:
                SetDisplayMode(EDisplayMode::UPDATES_ENABLED_DISPLAY_ENABLED);
                break;
        }
        request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    }
    else if (request->arg("method") == "disableUpdates")
    {
        switch (displayMode)
        {
            case EDisplayMode::UPDATES_ENABLED_DISPLAY_ENABLED:
                SetDisplayMode(EDisplayMode::DISPLAY_ENABLED);
                break;
            case EDisplayMode::UPDATES_ENABLED_DISPLAY_DISABLED:
                SetDisplayMode(EDisplayMode::DISPLAY_DISABLED);
                break;
        }
        request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    }
    else if (request->arg("method") == "disable")
    {
        switch (displayMode)
        {
            case EDisplayMode::DISPLAY_ENABLED:
                SetDisplayMode(EDisplayMode::DISPLAY_DISABLED);
                break;
            case EDisplayMode::UPDATES_ENABLED_DISPLAY_ENABLED:
                SetDisplayMode(EDisplayMode::UPDATES_ENABLED_DISPLAY_DISABLED);
                break;
        }
        request->send(200, "application/json", "{\"error\":false,\"data\":{}}");
    }
    else
    {
        request->send(200, "application/json", "{\"error\":true,\"data\":\"INVALID_METHOD\"}");
    }
}

void ConfiguredServer::SetDisplayMode(EDisplayMode _displayMode)
{
    switch (_displayMode)
    {
        case EDisplayMode::DISPLAY_DISABLED:
            Motion::motionUpdated.Unregister(updateDisplayEvent);
            Display::U8X8().clearLine(4);
            Display::U8X8().clearLine(5);
            Display::Disable();
            break;
        case EDisplayMode::UPDATES_ENABLED_DISPLAY_ENABLED:
            Display::Enable();
            Motion::motionUpdated.Register(updateDisplayEvent);
            break;
        case EDisplayMode::UPDATES_ENABLED_DISPLAY_DISABLED:
            Motion::motionUpdated.Unregister(updateDisplayEvent);
            Display::U8X8().clearLine(4);
            Display::U8X8().clearLine(5);
            Display::Disable();
            break;
        default: //EDisplayMode::DISPLAY_ENABLED
            Display::Enable();
            Motion::motionUpdated.Unregister(updateDisplayEvent);
            Display::U8X8().clearLine(4);
            Display::U8X8().clearLine(5);
            break;
    }
    Storage::PutInt("configuredServer", "displayMode", (int)_displayMode);
    displayMode = _displayMode;
}