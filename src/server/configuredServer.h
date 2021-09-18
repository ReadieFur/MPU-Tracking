#include "webserver.h"
#include "setupServer.h"
#include "webPages.h"
#include "network.h"
#include "motion.h"
#include "eventData.h"
#include "storage.h"
#include <math.h>

#ifndef CONFIGUREDSERVER_H
#define CONFIGUREDSERVER_H
enum EDisplayMode
{
    DISPLAY_DISABLED = 0,
    DISPLAY_ENABLED,
    UPDATES_ENABLED_DISPLAY_ENABLED,
    UPDATES_ENABLED_DISPLAY_DISABLED
};

class ConfiguredServer
{
    WebServer **webServer;
    EventData<MotionData> *motionUpdatedEvent;
    static EDisplayMode displayMode;
    static EventData<MotionData> updateDisplayEvent;
    static long lastDisplayUpdate;
    // const size_t motionJsonSize = JSON_OBJECT_SIZE(4) * JSON_ARRAY_SIZE(3);
    const size_t motionJsonSize = JSON_OBJECT_SIZE(2) * JSON_ARRAY_SIZE(3);

    public:
        //I think i should be using & here instead of * but the code dosen't like that because the value I try to pass is *.
        ConfiguredServer(WebServer **webServer);
        ~ConfiguredServer() {};
        void MotionUpdated(MotionData motionData);
        static void UpdateDisplay(void* context, MotionData motionData);
        static void GETDisplay(AsyncWebServerRequest *request);
        static void POSTDisplay(AsyncWebServerRequest *request);
        static void SetDisplayMode(EDisplayMode displayMode);
};
#endif