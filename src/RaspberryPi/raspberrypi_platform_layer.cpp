#include <common.h>
#include <stdio.h>
#include "bcm_host.h"

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include <assert.h>

#include <MeshEngine.h>

namespace MESHAPI
{
typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
   // OpenGL|ES objects
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
} CUBE_STATE_T;

static volatile int terminate;
static CUBE_STATE_T _state, *state=&_state;

internal void init_ogl(CUBE_STATE_T *state)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      // EGL_RED_SIZE, 8,
      // EGL_GREEN_SIZE, 8,
      // EGL_BLUE_SIZE, 8,
      // EGL_ALPHA_SIZE, 8,
      // EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
      EGL_NONE
   };
   
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   const EGLint context_attrib_list[] = { 
        // request a context using Open GL ES 2.0
        EGL_CONTEXT_CLIENT_VERSION, 2, 
        EGL_NONE 
   };

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attrib_list);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );

#if FULLSCREEN
   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
#else 
   dst_rect.x = state->screen_width/2;
   dst_rect.y = state->screen_height/2;
   dst_rect.width = state->screen_width/2;
   dst_rect.height = state->screen_height/2;
#endif
   dst_rect.x += SCREEN_PADDING;
   dst_rect.y += SCREEN_PADDING;
   dst_rect.width -= SCREEN_PADDING * 2;
   dst_rect.height -= SCREEN_PADDING * 2;


   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screen_width << 16;
   src_rect.height = state->screen_height << 16;        

   state->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   state->dispman_element = vc_dispmanx_element_add ( dispman_update, state->dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);
      
   nativewindow.element = state->dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);
}

int main()
{
   using namespace MESHAPI;
   bcm_host_init();

   // Clear application state
   memset( state, 0, sizeof( *state ) );
      
   // Start OGLES
   init_ogl(state);

   printf("OpenGL_ES initialized");

   const char* egl_client_apis = eglQueryString(state->display, EGL_CLIENT_APIS);
   const char* egl_extensions = eglQueryString(state->display, EGL_EXTENSIONS);
   const char* egl_vendor = eglQueryString(state->display, EGL_VENDOR);
   const char* egl_version = eglQueryString(state->display, EGL_VERSION);

   printf("eglQueryString:\n");
   printf("\n\t %s\n", egl_client_apis);
   printf("\n\t %s\n", egl_extensions);
   printf("\n\t %s\n", egl_vendor);
   printf("\n\t %s\n", egl_version);

   printf("glGetString:\n");
   printf("\n\t %s\n", glGetString( GL_RENDERER ));
   printf("\n\t %s\n", glGetString( GL_VENDOR ));
   printf("\n\t %s\n", glGetString( GL_VERSION ));
   printf("\n\t %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ));
   printf("\n\t %s\n", glGetString( GL_EXTENSIONS ));

   InitGLTest();

   while (!terminate)
   {
      UpdateAndRender(1.0f / 60.0f);

      eglSwapBuffers(state->display, state->surface);
   }

   return 0;
}

