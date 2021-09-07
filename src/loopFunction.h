#pragma once

#include "loopFunctionData.h"
#include <vector>

//I had a different method for the loopFunction but the compilier didn't like it so I went with this other methid that I don't like as much but it still works alright.
// LoopFunction Motion::loopFunction = LoopFunction(90, Motion::TimedGetter, false);

class LoopFunction
{
    public:
        static std::vector<LoopFunctionData> loopFunctions;

        static void AddFunction(LoopFunctionData &data);
        static void RemoveFunction(LoopFunctionData &data);
        static void RunAll();
};