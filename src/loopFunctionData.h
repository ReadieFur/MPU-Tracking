typedef void(*LoopFunc)(long currentLoopTime, long lastLoopTime);

struct LoopFunctionData
{
    long lastLoopTime;
    double frequency;
    LoopFunc function;
};