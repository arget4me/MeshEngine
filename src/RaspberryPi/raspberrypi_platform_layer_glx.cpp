#include "raspberrypi_platform_layer.h"
#include <common.h>
#include <X11/Xlib.h>
#include <GL/glxew.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <utils/log.h>

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
    XClearWindow(state->display, state->window);
	XMapRaised(state->display, state->window);

    /* XCreateSimpleWindow: does NOT support opengl. Use XCreateWindow instead.
     *   // state->window = XCreateSimpleWindow(state->display, RootWindowOfScreen(state->screen), 0, 0, 480, 340, 1, BlackPixel(state->display, state->screenId), WhitePixel(state->display, state->screenId));
     *   // XSelectInput(state->display, state->window, INPUTMASK);
    */

    XStoreName(state->display, state->window, RASPBERRY_PI_APPLICATION_NAME);

    XWindowAttributes winAttributes;
    XGetWindowAttributes(state->display, state->window, &winAttributes);
    state->screen_width = winAttributes.width;
    state->screen_height = winAttributes.height;

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        ERRORLOG("Error: %s\n", glewGetErrorString(err));
        return;
    }

    LOG("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
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


    return true;
}

int startGameloop(UpdateAndRenderFunc* UpdateAndRender)
{
    bool GlobalRunning = true;
    while (GlobalRunning)
    {
        ProcessMessages();


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