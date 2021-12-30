#ifndef PLATFORM_LAYER_HEADER
#define PLATFORM_LAYER_HEADER

#include <common.h>
#include <Input/user_input.h>

namespace MESHAPI
{

void InitPlatformLayer();
void CleanupPlatformLayer();
void QueryUserInput(UserInput& input);
void SwapBuffers();
bool ShouldWindowClose();

}

#endif