#ifndef RASPBERRYPI_PLATFORM_LAYER_HEADER
#define RASPBERRYPI_PLATFORM_LAYER_HEADER
#include <common.h>
#include <MeshEngine.h>
#include <Input/user_input.h>

namespace MESHAPI
{

bool QueryUserInput(UserInput& input);

void initPlatformLayer();
int startGameloop(UpdateAndRenderFunc* UpdateAndRender);

}

#endif