#pragma once

#include "loopFunction.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "eventDispatcher.h"

struct Vector3_f { float v[3]; };
struct Vector3_l { long v[3]; };

struct MotionData
{
    Vector3_f gyroscope;
    Vector3_f accelerometer;
};

// typedef void(*MotionData_Func)(MotionData motionData);

class Motion
{
    static TwoWire wire2;
    static Adafruit_MPU6050 mpu;
    static int mpuFound;
    static TaskHandle_t calibrationTask;
    //'temperatureEvent' is unused in this program.
    static Adafruit_Sensor *accelerometerSensor, *gyroscopeSensor;
    
    //Not doing this in quaternion because I haven't a clue how to use them, I know euler angles can give unexpected results but oh well.
    // static Vector3_f gyroscopeJitter;
    static Vector3_f gyroscope;
    // static Vector3_f rotation;
    // static Vector3_l lastRotationTimes;
    //In the future I could calculate the position on the client side as the client would do this much faster and it would allow for easier updates.
    // static Vector3_f accelerometerDrift;
    static Vector3_f accelerometer;
    // static Vector3_f position;
    // static Vector3_l lastPositionTimes;

    static LoopFunctionData loopFunctionData;

    Motion();
    ~Motion();
    // static void _calibrationTask(void *sampleCount);

    public:
        // static bool isCalibrating;
        static EventDispatcher<MotionData> motionUpdated;

        static bool MPUFound();
        // static void Calibrate(int pvParameters[2] = new int[2]{90, 1000 / 90});
        // void Calibrate(int sampleCount = 90, int delayBetweenSamplesMS = 1000 / 90);
        static bool Enable();
        static void Disable();
        static Vector3_f GetGyroscope();
        // static Vector3_f GetRotation();
        static Vector3_f GetAcclerometer();
        // static Vector3_f GetPosition();
        static void TimedGetter(long currentLoopTime, long lastLoopTime);
};