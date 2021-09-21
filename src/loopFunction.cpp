#include <Arduino.h>
#include "loopFunction.h"

std::vector<LoopFunctionData> LoopFunction::loopFunctions;

void LoopFunction::AddFunction(LoopFunctionData &data)
{
    if (data.frequency > 1000) { data.frequency = 0; }
    else
    {
        data.frequency = 1000 /*1 second*/ / /*x times per second*/ data.frequency;
    }
    loopFunctions.push_back(data);
}

void LoopFunction::RemoveFunction(LoopFunctionData &data)
{
    for (int i = 0; i < loopFunctions.size(); i++)
    {
        if (loopFunctions[i].function == data.function)
        {
            loopFunctions.erase(loopFunctions.begin() + i);
            // data.frequency *= 1000; //Revert to original frequency.
            return;
        }
    }
}

void LoopFunction::RunAll()
{
    for (int i = 0; i < loopFunctions.size(); i++)
    {
        long currentLoopTime = millis();
        if (currentLoopTime - loopFunctions[i].lastLoopTime >= loopFunctions[i].frequency)
        {
            loopFunctions[i].function(currentLoopTime, loopFunctions[i].frequency);
            loopFunctions[i].lastLoopTime = currentLoopTime;
        }
    }
}