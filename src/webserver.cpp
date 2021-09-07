#include "webserver.h"

WebServer::WebServer(int port, const char *websocketPath)
{
    // Initialize the server
    server = new AsyncWebServer(port);
    websocket = new AsyncWebSocket(websocketPath);
}

WebServer::~WebServer()
{
    websocket->closeAll();
    server->end();
    delete websocket;
    delete server;
}