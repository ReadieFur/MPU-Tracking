#include "webserver.h"

WebServer::WebServer(int port, const char *websocketPath)
{
    // Initialize the server
    server = new AsyncWebServer(port);
    websocket = new AsyncWebSocket(websocketPath);
    server->addHandler(websocket);
    server->onNotFound([](AsyncWebServerRequest *request) { request->send(404, "text/plain", "404"); });
}

WebServer::~WebServer()
{
    websocket->closeAll();
    server->end();
    delete websocket;
    delete server;
}