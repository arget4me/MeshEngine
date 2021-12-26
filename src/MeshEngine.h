#ifndef MESHENGINE_HEADER
#define MESHENGINE_HEADER
#include <common.h>

namespace MESHAPI
{

typedef bool (UpdateAndRenderFunc)(real32 dt);

bool InitGLTest();

}

#endif