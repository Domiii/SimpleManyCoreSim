#include "SimConfig.hpp"

int SimConfig::CoreGridSize()
{
    return CoreGridLen * CoreGridLen;
}

int SimConfig::CoreBlockSize()
{
    return CoreBlockLen * CoreBlockLen;
}

int2 SimConfig::ComputeInCoreBlockIdx2(int inCoreBlockIdx1)
{
    div_t d = div(inCoreBlockIdx1, CoreBlockLen);
    return int2(d.quot, d.rem);
}

int SimConfig::GetTotalL2CacheSize()
{
    return CoreBlockSize() * CacheL2Size;
}
