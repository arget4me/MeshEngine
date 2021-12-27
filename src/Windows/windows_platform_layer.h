#ifndef WINDOWS_PLATFORM_LAYER_HEADER
#define WINDOWS_PLATFORM_LAYER_HEADER
#include <common.h>
#include <Windows.h>
#include <MeshEngine.h>
#include <Input/user_input.h>

namespace MESHAPI
{

bool QueryUserInput(UserInput& input);

int initPlatformLayer();
int startGameloop(UpdateAndRenderFunc* UpdateAndRender);

}

#endif