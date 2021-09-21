#include "storage.h"
#include <ESPAsyncWebServer.h>

//https://stackoverflow.com/questions/13060699/arduino-class-redefinition-error
#ifndef WEBSERVER_H
#define WEBSERVER_H

class WebServer
{
    public:
        AsyncWebServer *server;
        AsyncWebSocket *websocket;

        WebServer(int port = 80, const char *websocketPath = "/websocket");
        ~WebServer();
};
#endif