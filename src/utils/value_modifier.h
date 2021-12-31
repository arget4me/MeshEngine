#ifndef VALUEMODIFIER_HEADER
#define VALUEMODIFIER_HEADER
#include <common.h>

void clamp(int32& value, const int32 min, const int32 max);
void clamp(real32& value, const real32 min, const real32 max);

void loop(int32& value, const int32 min, const int32 max);
void loop(real32& value, const real32 min, const real32 max);

void pulse_float(real32 &value, bool &value_state, real32 speed, real32 min_value, real32 max_value);
void loop_float(real32 &value, real32 speed, real32 min_value, real32 max_value);
void interpolate_float(real32& value, real32 speed, real32 target_value);
void interpolate_float(real32& value, real32 speed, real32 target_value, real32 threshold);

// #define VALUEMODIFIER_IMPLEMENTATION
#ifdef VALUEMODIFIER_IMPLEMENTATION
void clamp(int32& value, const int32 min, const int32 max)
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

void clamp(real32& value, const real32 min, const real32 max)
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

void pulse_float(real32 &value, bool &value_state, real32 speed, real32 min_value, real32 max_value)
{
	if (value_state)
	{
		value += speed;
		if (value >= max_value)
		{
			value = max_value;
			value_state = false;
		}
	}
	else
	{
		value -= speed;
		if (value <= min_value)
		{
			value = min_value;
			value_state = true;
		}
	}

}

void loop_float(real32& value, real32 speed, real32 min_value, real32 max_value)
{
	/*@Todo: Take a look at this behaviour again and explaing, not straight forward just from looking at the code.
		I think doing something similar to fmod but its bit unclear.
	*/
	value += speed;
	if (value > max_value)
	{
		int x = (int) ((value - min_value) / (max_value - min_value));

		value = value - x * (max_value - min_value);
	}

	if (value < min_value)
	{
		int x = (int) ((value - min_value) / (max_value - min_value));

		value = max_value + value - x * (max_value - min_value);
	}
}

void interpolate_float(real32& value, real32 speed, real32 target_value)
{
	if (value == target_value)
	{
		return;
	}else if (value > target_value)
	{
		value -= fabs(speed);
		if (value < target_value)
		{
			value = target_value;
		}
	}
	else
	{
		value += fabs(speed);
		if (value > target_value)
		{
			value = target_value;
		}
	}
}

void interpolate_float(real32& value, real32 speed, real32 target_value, real32 threshold)
{
	if (value == target_value)
	{
		return;
	}
	else if (value > target_value)
	{
		value -= fabs(speed);
	}
	else
	{
		value += fabs(speed);
	}

	if (fabs(value - target_value) <= threshold)
	{
		value = target_value;
	}
}


#endif

#endif