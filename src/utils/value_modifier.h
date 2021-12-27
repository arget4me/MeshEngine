#ifndef VALUEMODIFIER_HEADER
#define VALUEMODIFIER_HEADER
#include <common.h>

inline void clamp(int32& value, const int32& min, const int32& max);
inline void clamp(real32& value, const real32& min, const real32& max);

inline void loop(int32& value, const int32& min, const int32& max);
inline void loop(real32& value, const real32& min, const real32& max);


#ifdef VALUEMODIFIER_IMPLEMENTATION
inline void clamp(int32& value, const int32& min, const int32& max)
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

inline void clamp(real32& value, const real32& min, const real32& max)
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

inline void loop(int32& value, const int32& min, const int32& max)
{
    if(min == max)
    {
        value = max;
    }

    if(value < min)
    {
        const int32 range = max - min;
        const int32 offset = min - value;
        const int32 append = min + (((offset-min) / range) * range);
        value += append;
    }

    if(value > max)
    {
        const int32 range = max - min;
        const int32 offset = value - max;
        const int32 append = (((offset-max) / range) * range);
        value -= append;
    }


}

inline void loop(real32& value, const real32& min, const real32& max)
{
    // 0.3f -> 0.743f

    // 82.843f
    // 0.3004f
}



#endif

#endif