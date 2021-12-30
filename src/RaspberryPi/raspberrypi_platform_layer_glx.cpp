#include "raspberrypi_platform_layer.h"
#include <common.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/glxew.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <utils/log.h>
#include <utils/value_modifier.h>

namespace MESHAPI
{
typedef struct
{
   uint32 screen_width;
   uint32 screen_height;
   Display* display;
   Screen* screen;
   int32 screenId;
   Window window;
   GLXContext context;
   XVisualInfo* visualInfo;
   UserInput input;
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

    GLint glxMajor = 0;
    GLint glxMinor = 0;
    glXQueryVersion(state->display, &glxMajor, &glxMinor);
    LOG("GLX VERSION: %d.%d\n", glxMajor, glxMinor);

    GLint attriblist[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_SAMPLE_BUFFERS, 0,
        GLX_SAMPLES, 0,
        None
    };

    state->visualInfo = glXChooseVisual(state->display, state->screenId, attriblist);
    if(state->visualInfo == 0)
    {
        LOG("Unable to create window!\n");
    }

    // Open the window
	
    constexpr long KEYINPUT   = KeymapStateMask | KeyPressMask	| KeyReleaseMask;
    constexpr long MOUSEINPUT = PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask;
    constexpr long WINDOWRESIZE = ExposureMask;
    constexpr long INPUTMASK  = KEYINPUT | MOUSEINPUT | WINDOWRESIZE;

    XSetWindowAttributes windowAttributes{};
    windowAttributes.border_pixel = BlackPixel(state->display, state->screenId);
    windowAttributes.background_pixel = WhitePixel(state->display, state->screenId);
    windowAttributes.override_redirect = True;
    windowAttributes.colormap = XCreateColormap(state->display, RootWindow(state->display, state->screenId), state->visualInfo->visual, AllocNone);
    windowAttributes.event_mask = INPUTMASK;

    state->window = XCreateWindow(state->display, RootWindow(state->display, state->screenId), 0, 0, 480, 340, 1, state->visualInfo->depth, InputOutput, state->visualInfo->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttributes);
    state->context = glXCreateContext(state->display, state->visualInfo, NULL, GL_TRUE);
    glXMakeCurrent(state->display, state->window, state->context);
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        ERRORLOG("Error: %s\n", glewGetErrorString(err));
        return;
    }

    LOG("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    if(GLEW_VERSION_3_3)
    {
        LOG("GL 3.3 Supported.\n");
    }
    else if(GLEW_VERSION_3_2)
    {
        LOG("GL 3.2 Supported.\n");
    }
    else if(GLEW_VERSION_3_1)
    {
        LOG("GL 3.1 Supported.\n");
    }
    else if(GLEW_VERSION_3_0)
    {
        LOG("GL 3.0 Supported.\n");
    }
    else if(GLEW_VERSION_2_1)
    {
        LOG("GL 2.1 Supported.\n");
    }
    else if(GLEW_VERSION_2_0)
    {
        LOG("GL 2.0 Supported.\n");
    }
    glXSwapIntervalMESA(1); // Activate V-sync
    
    
    XClearWindow(state->display, state->window);
	XMapRaised(state->display, state->window);
    
    XKeyboardControl keyboardSettings{};
    keyboardSettings.auto_repeat_mode = AutoRepeatModeOff;
    XChangeKeyboardControl(state->display, KBAutoRepeatMode, &keyboardSettings);
    XAutoRepeatOff(state->display);

    XStoreName(state->display, state->window, RASPBERRY_PI_APPLICATION_NAME);

    XWindowAttributes winAttributes;
    XGetWindowAttributes(state->display, state->window, &winAttributes);
    state->screen_width = winAttributes.width;
    state->screen_height = winAttributes.height;
}

void ProcessMessages(XEvent& e)
{
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
            state->input.MouseX = e.xmotion.x;
            state->input.MouseY = e.xmotion.y;
        }break;
        case ButtonPress:
        {
            unsigned int& mouseButton = e.xbutton.button;
            LOG("Mouse button pressed: %u\n", mouseButton);
            if (mouseButton == 1) {
                state->input.Fire2 += 1;
            }
            else if (mouseButton == 2) {
                state->input.Submit += 1;
            }
            else if (mouseButton == 3) {
                state->input.Fire3 += 1;
            }
            else if (mouseButton == 4) {
                state->input.MouseScrollWheel = 1;
            } 
            else if (mouseButton == 5) {
                state->input.MouseScrollWheel = -1;
            }
        }break;
        case ButtonRelease:
        {
            unsigned int& mouseButton = e.xbutton.button;
            LOG("Mouse button released: %u\n", mouseButton);
            if (mouseButton == 1) {
                state->input.Fire2 -= 1;
            }
            else if (mouseButton == 2) {
                state->input.Submit -= 1;
            }
            else if (mouseButton == 3) {
                state->input.Fire3 -= 1;
            }
        }break;
        case KeymapNotify:
        {
            XRefreshKeyboardMapping(&e.xmapping);
        }break;
        case KeyPress:
        {
            XLookupString(&e.xkey, 0, 0, &keysym, 0);
            LOG("Key pressed: %lu\n", keysym);
            constexpr int keyValue = 1;
            if (keysym == 'w')
            {
                state->input.Vertical += keyValue;
            }
            else if (keysym == 'a')
            {
                state->input.Horizontal += -keyValue;
            }
            else if (keysym == 's')
            {
                state->input.Vertical += -keyValue;
            }
            else if (keysym == 'd')
            {
                state->input.Horizontal += keyValue;
            }
            else if (keysym == 'q')
            {
                state->input.Fire1 += -keyValue;
            }
            else if (keysym == 'e')
            {
                state->input.Fire1 += keyValue;
            }
            else if (keysym == XK_Up)
            {
                state->input.Vertical += keyValue;
            }
            else if (keysym == XK_Down)
            {
                state->input.Vertical += -keyValue;
            }
            else if (keysym == XK_Left)
            {
                state->input.Horizontal += -keyValue;
            }
            else if (keysym == XK_Right)
            {
                state->input.Horizontal += keyValue;
            }
            else if (keysym == XK_Escape)
            {
                state->input.Cancel += keyValue;
            }
            else if (keysym == XK_space)
            {
                state->input.Jump += keyValue;
            }
            else if (keysym == XK_Return)
            {
                state->input.Submit += keyValue;
            }
        }break;
        case KeyRelease:
        {
            XLookupString(&e.xkey, 0, 0, &keysym, 0);
            LOG("Key released: %lu\n", keysym);
            constexpr int keyValue = -1;
            if (keysym == 'w')
            {
                state->input.Vertical += keyValue;
            }
            else if (keysym == 'a')
            {
                state->input.Horizontal += -keyValue;
            }
            else if (keysym == 's')
            {
                state->input.Vertical += -keyValue;
            }
            else if (keysym == 'd')
            {
                state->input.Horizontal += keyValue;
            }
            else if (keysym == 'q')
            {
                state->input.Fire1 += -keyValue;
            }
            else if (keysym == 'e')
            {
                state->input.Fire1 += keyValue;
            }
            else if (keysym == XK_Up)
            {
                state->input.Vertical += keyValue;
            }
            else if (keysym == XK_Down)
            {
                state->input.Vertical += -keyValue;
            }
            else if (keysym == XK_Left)
            {
                state->input.Horizontal += -keyValue;
            }
            else if (keysym == XK_Right)
            {
                state->input.Horizontal += keyValue;
            }
            else if (keysym == XK_Escape)
            {
                state->input.Cancel += keyValue;
            }
            else if (keysym == XK_space)
            {
                state->input.Jump += keyValue;
            }
            else if (keysym == XK_Return)
            {
                state->input.Submit += keyValue;
            }
        }break;
        case Expose:
        {
            XWindowAttributes winAttributes;
            XGetWindowAttributes(state->display, state->window, &winAttributes);
            if(state->screen_width != winAttributes.width || state->screen_height != winAttributes.height)
            {
                state->screen_width = winAttributes.width;
                state->screen_height = winAttributes.height;
                LOG("Window resized: width:%d height:%d\n", state->screen_width, state->screen_height);
            }
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
    input = state->input;
    state->input.MouseScrollWheel = 0;
    clamp(input.Horizontal, -1.0f, 1.0f);
    clamp(input.Vertical, -1.0f, 1.0f);
    clamp(input.Fire1, -1.0f, 1.0f);
    clamp(input.Fire2, -1.0f, 1.0f);
    clamp(input.Fire3, -1.0f, 1.0f);
    clamp(input.Jump, -1.0f, 1.0f);
    clamp(input.MouseScrollWheel, -1.0f, 1.0f);
    clamp(input.Submit, -1.0f, 1.0f);
    clamp(input.Cancel, -1.0f, 1.0f);
    return true;
}

int startGameloop(UpdateAndRenderFunc* UpdateAndRender)
{
    bool GlobalRunning = true;
    while (GlobalRunning)
    {
        while(XPending(state->display))
        {
            XEvent e;
            XNextEvent(state->display, &e);
            ProcessMessages(e);
        }

        GlobalRunning = UpdateAndRender( 1.0f / 60.0f );
        glXSwapBuffers(state->display, state->window);
    }

	// Cleanup
    if(state->context)
    {
        glXDestroyContext(state->display, state->context);
    }
	XDestroyWindow(state->display, state->window);
	XFree(state->screen);
	XCloseDisplay(state->display);

   return 0;
}

}