#include "motion.h"

//https://randomnerdtutorials.com/esp32-mpu-6050-web-server/
//To calculate the distance moved and not overload the CPU, take a reading every x ms and then calculate the distance moved from the previous reading AND timestamp.

Adafruit_MPU6050 Motion::mpu;
int Motion::mpuFound = -1;
bool Motion::isCalibrating = false;
TaskHandle_t Motion::calibrationTask;
sensors_event_t Motion::accelerometerEvent;
sensors_event_t Motion::gyroscopeEvent;
sensors_event_t Motion::temperatureEvent;

//Should some of these default as 1?
//Or should their last position times be set when enabled?
Vector3_f Motion::gyroscopeJitter = Vector3_f { { 0.0f, 0.0f, 0.0f } };
Vector3_f Motion::gyroscope = Vector3_f { { 0.0f, 0.0f, 0.0f } };
Vector3_f Motion::rotation = Vector3_f { { 0.0f, 0.0f, 0.0f } };
Vector3_l Motion::lastRotationTimes = Vector3_l { { 0, 0, 0 } };

Vector3_f Motion::accelerometerDrift = Vector3_f { { 0.0f, 0.0f, 0.0f } };
Vector3_f Motion::accelerometer = Vector3_f { { 0.0f, 0.0f, 0.0f } };
Vector3_f Motion::position = Vector3_f { { 0.0f, 0.0f, 0.0f } };
Vector3_l Motion::lastPositionTimes = Vector3_l { { 0, 0, 0 } };

LoopFunctionData Motion::loopFunctionData = LoopFunctionData { 0, 90, Motion::TimedGetter };


bool Motion::MPUFound()
{
    if (mpuFound == -1)
    {
        bool _mpuBegin = mpu.begin();
        mpuFound = _mpuBegin ? 1 : 0;
        return _mpuBegin;
    }
    else
    {
        return mpuFound == 1 ? true : false;
    }
}

void Motion::_calibrationTask(void *pvParameters)
{
    isCalibrating = true;

    //Params size should be 2.
    int* params = (int*)pvParameters;
    int sampleCount = params[0];
    int delayBetweenSamples = params[1];

    if (mpuFound)
    {
        float gyroXSum = 0.0f;
        float gyroYSum = 0.0f;
        float gyroZSum = 0.0f;
        
        float accelXSum = 0.0f;
        float accelYSum = 0.0f;
        float accelZSum = 0.0f;

        for (int i = 0; i < sampleCount; i++)
        {
            mpu.getEvent(&accelerometerEvent, &gyroscopeEvent, &temperatureEvent);

            gyroXSum += gyroscopeEvent.gyro.x;
            gyroYSum += gyroscopeEvent.gyro.y;
            gyroZSum += gyroscopeEvent.gyro.z;

            accelXSum += accelerometerEvent.acceleration.x;
            accelYSum += accelerometerEvent.acceleration.y;
            accelZSum += accelerometerEvent.acceleration.z;

            vTaskDelay(delayBetweenSamples / portTICK_PERIOD_MS);
            feedLoopWDT();
        }

        //Should I ABS() these?
        //By ABS() checking the values in the LoopEvent does that also apply the drift to the wrong direction? If so I probably shouldn't ABS() them.
        gyroscopeJitter.v[0] = gyroXSum / sampleCount;
        gyroscopeJitter.v[1] = gyroYSum / sampleCount;
        gyroscopeJitter.v[2] = gyroZSum / sampleCount;

        accelerometerDrift.v[0] = accelXSum / sampleCount;
        accelerometerDrift.v[1] = accelYSum / sampleCount;
        accelerometerDrift.v[2] = accelZSum / sampleCount;

        gyroscope = Vector3_f { { 0.0f, 0.0f, 0.0f } };
        rotation = Vector3_f { { 0.0f, 0.0f, 0.0f } };
        lastRotationTimes = Vector3_l { { 0, 0, 0 } };

        accelerometer = Vector3_f { { 0.0f, 0.0f, 0.0f } };
        position = Vector3_f { { 0.0f, 0.0f, 0.0f } };
        lastPositionTimes = Vector3_l { { 0, 0, 0 } };
    }

    isCalibrating = false;
    vTaskDelete(calibrationTask);
}

// void Motion::Calibrate(int sampleCount, int delayBetweenSamplesMS)
void Motion::Calibrate(int pvParameters[2])
{
    xTaskCreatePinnedToCore(
        Motion::_calibrationTask,
        "CalibrationTask",
        2048,
        &pvParameters,
        1,
        &Motion::calibrationTask,
        0
    );
}

bool Motion::Enable()
{
    if (loopFunctionData.lastLoopTime == 0)
    {
        if (!mpuFound) { return false; }
        LoopFunction::AddFunction(loopFunctionData);
        return true;
    }
}

void Motion::Disable()
{
    if (loopFunctionData.lastLoopTime != 0)
    {
        LoopFunction::RemoveFunction(loopFunctionData);
        loopFunctionData.lastLoopTime = 0;
        loopFunctionData.frequency = 90;
    }
}

Vector3_f Motion::GetRotation()
{
    return rotation;
}

Vector3_f Motion::GetPosition()
{
    return position;
}

//Think about moving this to its own task thread for the least time between readings.
void Motion::TimedGetter(long currentLoopTime, long lastLoopTime)
{
    mpu.getEvent(&accelerometerEvent, &gyroscopeEvent, &temperatureEvent);

    #pragma region Gyroscope
    //I am going to change the deviation on the client side later on.
    //If I do that then I may as well also calculate the position change on the client side as well, for now I will do it on the server side during tests and then maybe move it to the client side at a later stage.
    //The calibration for this should be done with the sensor on a FLAT surface. Then take the readings and use that as the offest.
    //I could also do the calibration over x seconds and then use the average as the offset
    //A similar thing could be used to smooth the movment and prevent some of the position jitter.
    // static const float gyroXoffset = ?;
    //Then use this calculation to check if the movement is within the deviation.
    // if(abs(gyroX) > gyroXoffset)
    //If the reading change is greater than the offset then the reading is valid.
    //If it is valid, devide the reading by the time in MS since the last reading.
    //More info here https://randomnerdtutorials.com/esp32-mpu-6050-web-server/

    if (abs(gyroscopeEvent.gyro.x) > gyroscopeJitter.v[0])
    {
        long gyroXNow = millis();
        long gyroXDiff = gyroXNow - lastRotationTimes.v[0];
        rotation.v[0] = rotation.v[0] + gyroscopeEvent.gyro.x * gyroXDiff;
        gyroscope.v[0] = gyroscopeEvent.gyro.x * gyroXDiff;
        lastRotationTimes.v[0] = gyroXNow;
    }
    if (abs(gyroscopeEvent.gyro.y) > gyroscopeJitter.v[1])
    {
        long gyroYNow = millis();
        long gyroYDiff = gyroYNow - lastRotationTimes.v[1];
        rotation.v[1] = rotation.v[1] + gyroscopeEvent.gyro.y * gyroYDiff;
        gyroscope.v[1] = gyroscopeEvent.gyro.y * gyroYDiff;
        lastRotationTimes.v[1] = gyroYNow;
    }
    if (abs(gyroscopeEvent.gyro.z) > gyroscopeJitter.v[2])
    {
        long gyroZNow = millis();
        long gyroZDiff = gyroZNow - lastRotationTimes.v[2];
        rotation.v[2] = rotation.v[2] + gyroscopeEvent.gyro.z * gyroZDiff;
        gyroscope.v[2] = gyroscopeEvent.gyro.z * gyroZDiff;
        lastRotationTimes.v[2] = gyroZNow;
    }
    #pragma endregion

    #pragma region Accelerometer
    //https://www.youtube.com/watch?v=EiOl7eH_fzM
    if (abs(accelerometerEvent.acceleration.x) > accelerometerDrift.v[0])
    {
        long accelXNow = millis();
        long accelXDiff = accelXNow - lastPositionTimes.v[0];
        position.v[0] = accelerometerEvent.acceleration.x * accelXDiff;
        accelerometer.v[0] = accelerometerEvent.acceleration.x * accelXDiff;
        lastPositionTimes.v[0] = accelXNow;
    }
    if (abs(accelerometerEvent.acceleration.y) > accelerometerDrift.v[1])
    {
        long accelYNow = millis();
        long accelYDiff = accelYNow - lastPositionTimes.v[1];
        position.v[1] = accelerometerEvent.acceleration.y * accelYDiff;
        accelerometer.v[1] = accelerometerEvent.acceleration.y * accelYDiff;
        lastPositionTimes.v[1] = accelYNow;
    }
    if (abs(accelerometerEvent.acceleration.z) > accelerometerDrift.v[2])
    {
        long accelZNow = millis();
        long accelZDiff = accelZNow - lastPositionTimes.v[2];
        position.v[2] = accelerometerEvent.acceleration.z * accelZDiff;
        accelerometer.v[2] = accelerometerEvent.acceleration.z * accelZDiff;
        lastPositionTimes.v[2] = accelZNow;
    }
    #pragma endregion

    Serial.println(
        "Rotation: x:" + String(rotation.v[0]) + " y:" + String(rotation.v[1]) + " z:" + String(rotation.v[2]) +
        " Position: x:" + String(position.v[0]) + " y:" + String(position.v[1]) + " z:" + String(position.v[2]) +
        "\n"
    );
}