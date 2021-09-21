#pragma once

#include "splash.h"
#include "storage.h"
#include "network.h"
#include "webserver.h"
#include "setupServer.h"
#include "configuredServer.h"
#include "motion.h"
#include "display.h"
#include <driver/rtc_io.h>
#include <soc/rtc.h>
extern "C"
{
    #include <esp_clk.h>
}


//Move this along with other common files into its own header file.
#ifndef VOIDFUNC_T
#define VOIDFUNC_T
typedef void(*VoidFunc)();
#endif

class Program
{
    static bool initialized;
    static bool networkPreConfigured;
    static WebServer *webServer;
    static SetupServer *setupServer;
    static ConfiguredServer *configuredServer;
    static SemaphoreHandle_t semaphore;
    static RTC_DATA_ATTR bool wasAsleep;
    static RTC_DATA_ATTR long PRGSleepTimes[3];

    Program() {};
    ~Program() {};

    static void LogWakeupReason();
    static void SetupSleepButton();
    static void EnterDeepSleep();

    public:
        static std::vector<VoidFunc> onDeepSleep;

        static void Init();
        static uint64_t GetRTCMS();
        static void DeepSleep();
        static void ResetESP();
        static void Restart();

        //These have to be public because it is shared to another class.
        static void SemaphoreHandler(void *arg);
        static void PRGStateDownAwake(void *arg);
};