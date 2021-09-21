#include "program.h"
#include "loopFunction.h"

void setup()
{
    Program::Init();
}

void loop()
{
    LoopFunction::RunAll();
}