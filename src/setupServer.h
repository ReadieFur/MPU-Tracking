#include "webserver.h"
#include "network.h"
#include "program.h"
#include <ArduinoJson.h>
// #include <AsyncJson.h>
#include "storage.h"
#include <SPIFFS.h>

#ifndef SETUPSERVER_H
#define SETUPSERVER_H
class SetupServer
{
    //https://stackoverflow.com/questions/16183189/expression-must-have-pointer-to-class-type
    WebServer **webServer;

    public:
        //I think i should be using & here instead of * but the code dosen't like that because the value I try to pass is *.
        SetupServer(WebServer **webServer);
        ~SetupServer() {};
        static void GETIndex(AsyncWebServerRequest *request);
        static void GETNetworks(AsyncWebServerRequest *request);
        static void POSTNetwork(AsyncWebServerRequest *request);
        static void POSTSleep(AsyncWebServerRequest *request);
        static void POSTReboot(AsyncWebServerRequest *request);
        static void POSTResetESP(AsyncWebServerRequest *request);
};
#endif