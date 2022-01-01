#ifndef USERINPUT_HEADER
#define USERINPUT_HEADER
#include <common.h>

namespace MESHAPI
{
struct UserInput
{
    real32 Horizontal;
    real32 Vertical;
    real32 Fire1;
    real32 Fire2;
    real32 Fire3;
    real32 Jump;
    real32 MouseX;
    real32 MouseY;
    real32 MouseScrollWheel;
    real32 Submit;
    real32 Cancel;
};
}

#endif