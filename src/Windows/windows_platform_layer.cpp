#include <Windows.h>
#include "platform_layer.h"
#include <common.h>
#include <windowsx.h>

#include <GLEW/glew.h>
#include <GLEW/wglew.h>
#include <MeshEngine.h>

#include <utils/log.h>
#include <utils/value_modifier.h>

namespace MESHAPI
{

static struct WindowsPlatformInternalData
{
    HDC DeviceContext;
    UserInput UserInput{};
    bool Running;
}winData{};

LRESULT CALLBACK MainWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

inline static bool RegisterWNDClass(HINSTANCE hinstance, const wchar_t* WINDOW_CLASS_NAME)
{
    WNDCLASS wc = {};

    // Register the main window class.
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClass(&wc))
    {
        // TODO: Error logging. To console or to file or both.
        return false;
    }

    return true;
}

inline static bool CreateHWND(HWND& hwnd, HINSTANCE hinstance, const wchar_t* WINDOW_CLASS_NAME)
{
    hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        WINDOW_CLASS_NAME,              // Window class
        WINDOWS_APPLICATION_NAME,       // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window
        NULL,       // Menu
        hinstance,  // Instance handle
        NULL        // Additional application data
    );

    return hwnd != NULL;
}
struct Win32OpenGLDummyWindow
{
    HWND windowHandle;
    HDC deviceContext;
    HGLRC glContext;
    int32 pixel_format_index;
};

static Win32OpenGLDummyWindow Win32CreateOpenGLDummyWindow(HINSTANCE hInstance)
{
    Win32OpenGLDummyWindow result = { nullptr, nullptr, nullptr, {} };

    WNDCLASS WindowClass{};
    WindowClass.style         = CS_OWNDC; // Note: CS_OWNDC for OpenGl context creation
    WindowClass.lpfnWndProc   = DefWindowProc;
    WindowClass.hInstance     = hInstance;
    WindowClass.lpszClassName = TEXT("DUMMY_WINDOW_CLASS");

    if (RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, TEXT("DUMMY_WINDOW_CLASS"), TEXT("DUMMY"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, 0);
        if (WindowHandle)
        {
            HDC DeviceContext = GetDC(WindowHandle);
            PIXELFORMATDESCRIPTOR pixel_format_descriptor = {}; // initialize to zero since there are several unused fields in this struct
            pixel_format_descriptor.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
            pixel_format_descriptor.nVersion     = 1;
            pixel_format_descriptor.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pixel_format_descriptor.iPixelType   = PFD_TYPE_RGBA;
            pixel_format_descriptor.cColorBits   = 32;
            pixel_format_descriptor.cDepthBits   = 24;
            pixel_format_descriptor.cStencilBits = 8;

            int32 pixel_format_index = ChoosePixelFormat(DeviceContext, &pixel_format_descriptor);
            if (pixel_format_index != 0)
            {
                if (SetPixelFormat(DeviceContext, pixel_format_index, &pixel_format_descriptor))
                {
                    HGLRC opengl_context_handle = wglCreateContext(DeviceContext);
                    if (opengl_context_handle)
                    {

                        if (wglMakeCurrent(DeviceContext, opengl_context_handle))
                        {
                            result.windowHandle = WindowHandle;
                            result.deviceContext = DeviceContext;
                            result.glContext = opengl_context_handle;
                            result.pixel_format_index = pixel_format_index;
                        }
                    }
                }
            }
        }
    }

    return result;
}

inline static bool InitOpenGL()
{
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        ERRORLOG("Error: %s\n", glewGetErrorString(err));
        return false;
    }

    LOG("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    return true;
}

HGLRC Win32CreateOpenGLContext(HINSTANCE hInstance, HDC DeviceContext)
{
    HGLRC opengl_context_handle = 0;
    Win32OpenGLDummyWindow dummy = Win32CreateOpenGLDummyWindow(hInstance);
    if (dummy.windowHandle && dummy.deviceContext && dummy.glContext)
    {
        InitOpenGL();

        if( GLEW_ARB_framebuffer_sRGB )
        {
            const int attribList[] =
            {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SAMPLES_ARB, 4,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
                0, // End
            };

            int pixelFormat;
            UINT numFormats;

            if(wglChoosePixelFormatARB(DeviceContext, attribList, NULL, 1, &pixelFormat, &numFormats))
            {
                // Set new pixel format for proper device context
                PIXELFORMATDESCRIPTOR pfd{};
                if(SetPixelFormat(DeviceContext, pixelFormat, &pfd))
                {
                    LOG("Successfully set opengl pixelformat\n");

                    const int attribList[] =
                    {
                        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                        // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                        0, // End
                    };
                    opengl_context_handle = wglCreateContextAttribsARB(DeviceContext, 0, attribList);
                }
            }

            LOG("\n\nExtensions : ");
            LOG((char *)glGetString(GL_EXTENSIONS));
            LOG("\n\nShading Language : ");
            LOG((char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
            LOG("\n\nGL Version : ");
            LOG((char *)glGetString(GL_VERSION));
            LOG("\n\n");

            // Destroy dummy context now that the WGL extensions are loaded.
            wglMakeCurrent(dummy.deviceContext, 0);
            wglDeleteContext(dummy.glContext);
            DestroyWindow(dummy.windowHandle);
        }
        else
        {
            // Fallback to dummy context if sRGB is not supported
            PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
            if (SetPixelFormat(DeviceContext, dummy.pixel_format_index, &pixel_format_descriptor))
            {
                opengl_context_handle = wglCreateContext(DeviceContext);
            }

            // Destroy dummy context
            wglMakeCurrent(dummy.deviceContext, 0);
            wglDeleteContext(dummy.glContext);
            DestroyWindow(dummy.windowHandle);
        }
    }

    return opengl_context_handle;
}

bool ShouldWindowClose()
{
    return !winData.Running;
}

void QueryUserInput(UserInput& input)
{
    MSG msg = { };
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) // Poll all messages belonging to this thread
    {
        if (msg.message == WM_QUIT)
        {
            winData.Running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    input = winData.UserInput;
    winData.UserInput.MouseScrollWheel = 0;
    clamp(input.Horizontal, -1.0f, 1.0f);
    clamp(input.Vertical, -1.0f, 1.0f);
    clamp(input.Fire1, -1.0f, 1.0f);
    clamp(input.Fire2, -1.0f, 1.0f);
    clamp(input.Fire3, -1.0f, 1.0f);
    clamp(input.Jump, -1.0f, 1.0f);
    // clamp(input.MouseX, -1.0f, 1.0f); // NOTE: Should be NDC instead
    // clamp(input.MouseY, -1.0f, 1.0f); // NOTE: Should be NDC instead
    clamp(input.MouseScrollWheel, -1.0f, 1.0f);
    clamp(input.Submit, -1.0f, 1.0f);
    clamp(input.Cancel, -1.0f, 1.0f);
}

void SwapBuffers()
{

}

void CleanupPlatformLayer()
{
    SwapBuffers(winData.DeviceContext);
}

LRESULT CALLBACK MainWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM WParam,    // first message parameter
    LPARAM LParam)    // second message parameter
{
    LRESULT Result = 0;

    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            winData.UserInput.Fire2 += 1.0f;
        }break;
        case WM_RBUTTONDOWN:
        {
            winData.UserInput.Fire3 += 1.0f;
        }break;
        case WM_LBUTTONUP:
        {
            winData.UserInput.Fire2 -= 1.0f;
        }break;
        case WM_RBUTTONUP:
        {
            winData.UserInput.Fire3 -= 1.0f;
        }break;
        case WM_MOUSEMOVE:
        {
            winData.UserInput.MouseX = (real32)GET_X_LPARAM(LParam);
            winData.UserInput.MouseY = (real32)GET_Y_LPARAM(LParam);
        }break;
        case WM_MOUSEWHEEL:
        {
            winData.UserInput.MouseScrollWheel = (real32)GET_WHEEL_DELTA_WPARAM(WParam);
        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool32 WasDown = ((LParam & (1 << 30)) != 0);
            bool32 IsDown = ((LParam & (1 << 31)) == 0);
            bool32 pressed = (WasDown != IsDown);
            real32 keyValue = IsDown * 1.0f + !IsDown * -1.0f;
            if(pressed)
            {
                if (VKCode == 'W')
                {
                    winData.UserInput.Vertical += keyValue;
                }
                else if (VKCode == 'A')
                {
                    winData.UserInput.Horizontal += -keyValue;
                }
                else if (VKCode == 'S')
                {
                    winData.UserInput.Vertical += -keyValue;
                }
                else if (VKCode == 'D')
                {
                    winData.UserInput.Horizontal += keyValue;
                }
                else if (VKCode == 'Q')
                {
                    winData.UserInput.Fire1 += -keyValue;
                }
                else if (VKCode == 'E')
                {
                    winData.UserInput.Fire1 += keyValue;
                }
                else if (VKCode == VK_UP)
                {
                    winData.UserInput.Vertical += keyValue;
                }
                else if (VKCode == VK_DOWN)
                {
                    winData.UserInput.Vertical += -keyValue;
                }
                else if (VKCode == VK_LEFT)
                {
                    winData.UserInput.Horizontal += -keyValue;
                }
                else if (VKCode == VK_RIGHT)
                {
                    winData.UserInput.Horizontal += keyValue;
                }
                else if (VKCode == VK_ESCAPE)
                {
                    winData.UserInput.Cancel += keyValue;
                }
                else if (VKCode == VK_SPACE)
                {
                    winData.UserInput.Jump += keyValue;
                }
                else if (VKCode == VK_RETURN)
                {
                    winData.UserInput.Submit += keyValue;
                }
            }


            bool32 AltKeyDown = (LParam & (1 << 29));
            if (VKCode == VK_F4 && AltKeyDown)
            {
                PostQuitMessage(0);
            }
        }

        case WM_SIZE:
        {

        }break;    // Set the size and position of the window.

        case WM_CLOSE:
        {
            PostQuitMessage(0);
        }break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }break;    // Clean up window-specific data objects.

        default:
            Result =  DefWindowProc(hwnd, uMsg, WParam, LParam);
    }
    return Result;
}


int InitPlatformLayer()
{
    HINSTANCE hinstance = GetModuleHandle(0); // Only works if calling process is not from a dll
    using namespace MESHAPI;
    const wchar_t* WINDOW_CLASS_NAME = L"MESH ENGINE WINDOW CLASS";

    if(!RegisterWNDClass(hinstance, WINDOW_CLASS_NAME))
    {
        return FALSE;
    }

    HWND hwnd;
    if(!CreateHWND(hwnd, hinstance, WINDOW_CLASS_NAME))
    {
        return FALSE;
    }


    HGLRC opengl_context_handle;
    winData.DeviceContext = GetDC(hwnd);
    opengl_context_handle = Win32CreateOpenGLContext(hinstance, winData.DeviceContext);

    if(opengl_context_handle == nullptr)
    {
        return FALSE;
    }

    if (!wglMakeCurrent(winData.DeviceContext, opengl_context_handle))
    {
        return FALSE;
    }

    glEnable(GL_FRAMEBUFFER_SRGB);

    ShowWindow(hwnd, SW_SHOW);
    winData.Running = true;

    return TRUE;
}

}
