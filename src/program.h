#pragma once

#include "storage.h"
#include "network.h"
#include "webserver.h"
#include "motion.h"

class Program
{
    static bool initialized;
    static WebServer *webServer;

    Program() {};
    ~Program() {};

    public:
        static void Init();
        static void Dispose();
};