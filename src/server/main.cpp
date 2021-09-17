/*#include <Arduino.h>
#include <U8x8lib.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>*/

#include "program.h"
#include "loopFunction.h"

void setup()
{
    Program::Init();
}

int i = 0;
void loop()
{
    LoopFunction::RunAll();
}