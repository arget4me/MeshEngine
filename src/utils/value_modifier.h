#ifndef VALUEMODIFIER_HEADER
#define VALUEMODIFIER_HEADER
#include <common.h>

inline void clamp(int32& value, const int32 min, const int32 max);
inline void clamp(real32& value, const real32 min, const real32 max);

void loop(int32& value, const int32 min, const int32 max);
void loop(real32& value, const real32 min, const real32 max);

// #define VALUEMODIFIER_IMPLEMENTATION
#ifdef VALUEMODIFIER_IMPLEMENTATION
inline void clamp(int32& value, const int32 min, const int32 max)
{
    if(value < min)
    {
        value = min;
    }
    else if(value > max)
    {
        value = max;
    }
}

inline void clamp(real32& value, const real32 min, const real32 max)
{
    if(value < min)
    {
        value = min;
    }
    else if(value > max)
    {
        value = max;
    }
}

#include <cmath>

void loop(int32& value, const int32 min, const int32 max)
{
    if(min == max)
    {
        value = max;
        return;
    }

    int32 range = (max - min);
    range = range >= 0 ? range + 1 : 1 - range;

    int32 diff = value - min;
    int32 offset = 0;
    if(diff < 0)
    {
        diff = -diff;
        offset = (diff / range + 1) * range;
    }
    int32 newValue = ((value - min) + offset) % range;
    value = newValue + min;
}

void loop(real32& value, const real32 min, const real32 max)
{
    constexpr real32 shiftFraction = 1000.f;
    constexpr real32 inverseShiftFraction = 1.f / shiftFraction;

    int32 newValue = (int32)(value * shiftFraction);
    int32 newMin   = (int32)(min   * shiftFraction);
    int32 newMax   = (int32)(max   * shiftFraction);
    loop(newValue, newMin, newMax);
    value = (real32)newValue * inverseShiftFraction;
}



#endif

#endif