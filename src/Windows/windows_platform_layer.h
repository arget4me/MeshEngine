#ifndef WINDOWS_PLATFORM_LAYER_HEADER
#define WINDOWS_PLATFORM_LAYER_HEADER
#include <common.h>
#include <Windows.h>
#include <MeshEngine.h>

namespace MESHAPI
{

int initPlatformLayer();
int startGameloop(UpdateAndRenderFunc* UpdateAndRender);

}

#endif