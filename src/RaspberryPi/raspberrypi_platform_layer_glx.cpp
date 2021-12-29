#include "raspberrypi_platform_layer.h"
#include <common.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <utils/log.h>

namespace MESHAPI
{
typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
   Display* display;
   Screen* screen;
   int32 screenId;
   Window window;
   GLXContext context;
   XVisualInfo visualInfo;
   XEvent event;
} OPENGL_STATE;
static OPENGL_STATE _state, *state=&_state;

void CreateGLXWindow()
{
    // Open the display
	state->display = XOpenDisplay(NULL);

    if (state->display == NULL) {
		ERRORLOG("%s\n", "Could not open display");
		return;
	}

    state->screen = DefaultScreenOfDisplay(state->display);
	state->screenId = DefaultScreen(state->display);

    // Open the window
	state->window = XCreateSimpleWindow(state->display, RootWindowOfScreen(state->screen), 0, 0, 480, 340, 1, BlackPixel(state->display, state->screenId), WhitePixel(state->display, state->screenId));

    XClearWindow(state->display, state->window);
	XMapRaised(state->display, state->window);

    constexpr long KEYINPUT   = KeymapStateMask | KeyPressMask	| KeyReleaseMask;
    constexpr long MOUSEINPUT = PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask;
    constexpr long INPUTMASK  = KEYINPUT | MOUSEINPUT;
    XSelectInput(state->display, state->window, INPUTMASK);

    XStoreName(state->display, state->window, RASPBERRY_PI_APPLICATION_NAME);
}

void ProcessMessages()
{
    XNextEvent(state->display, &state->event);
    XEvent& e = state->event;
    KeySym keysym = 0;

    switch(e.type)
    {
        case EnterNotify:
        {
            LOG("Mouse entered window\n")
        }break;
        case LeaveNotify:
        {
            LOG("Mouse left window\n")
        }break;
        case MotionNotify:
        {
            LOG("Mouse moved: (%d, %d) Window coordinates\n", e.xmotion.x, e.xmotion.y);
        }break;
        case ButtonPress:
        {
            LOG("Mouse button pressed: %u\n", e.xbutton.button);
        }break;
        case ButtonRelease:
        {
            LOG("Mouse button released: %u\n", e.xbutton.button);
        }break;
        case KeymapNotify:
        {
            XRefreshKeyboardMapping(&e.xmapping);
        }break;
        case KeyPress:
        {
            XLookupString(&e.xkey, 0, 0, &keysym, 0);
            LOG("Key pressed: %lu\n", keysym);
        }break;
        case KeyRelease:
        {
            XLookupString(&e.xkey, 0, 0, &keysym, 0);
            LOG("Key released: %lu\n", keysym);
        }break;
    }
}

void initPlatformLayer()
{
   using namespace MESHAPI;

    CreateGLXWindow();




    LOG("OpenGL MESA initialized.\n");
    LOG("GLX_EXTENSIONS              = %s\n", (char *) glXGetClientString(state->display, GLX_EXTENSIONS));
    LOG("GLX_VENDOR                  = %s\n", (char *) glXGetClientString(state->display, GLX_VENDOR));    
    LOG("GLX_VERSION                 = %s\n", (char *) glXGetClientString(state->display, GLX_VERSION));
    LOG("GL_RENDERER                 = %s\n", (char *) glGetString(GL_RENDERER));
    LOG("GL_VERSION                  = %s\n", (char *) glGetString(GL_VERSION));
    LOG("GL_VENDOR                   = %s\n", (char *) glGetString(GL_VENDOR));
    LOG("GL_EXTENSIONS               = %s\n", (char *) glGetString(GL_EXTENSIONS));
    LOG("GL_SHADING_LANGUAGE_VERSION = %s\n", (char *) glGetString(GL_SHADING_LANGUAGE_VERSION));
}

bool QueryUserInput(UserInput& input)
{

    
    return true;
}

int startGameloop(UpdateAndRenderFunc* UpdateAndRender)
{
    bool GlobalRunning = true;
    while (GlobalRunning)
    {
        ProcessMessages();


        //GlobalRunning = UpdateAndRender( 1.0f / 60.0f );
        //glXSwapBuffers(state->display, state->window);
    }

	// Cleanup
	XDestroyWindow(state->display, state->window);
	XFree(state->screen);
	XCloseDisplay(state->display);

   return 0;
}

}