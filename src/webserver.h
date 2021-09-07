#include "storage.h"
#include <ESPAsyncWebServer.h>

class WebServer
{
    public:
        AsyncWebServer *server;
        AsyncWebSocket *websocket;

        WebServer(int port = 80, const char *websocketPath = "/websocket");
        ~WebServer();
};