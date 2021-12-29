#include "MeshEngine.h"
#include <common.h>
#include <stdio.h>
#include <stdlib.h>

#if _WIN64

#include "Windows/windows_platform_layer.h"
#include <GLEW/glew.h>

#elif __linux__
#ifdef RASPBERRY_PI

#include "RaspberryPi/raspberrypi_platform_layer.h"

#ifdef EGL
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

#ifdef GLX
#include <GL/glxew.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#endif

#endif
#endif

#include <cmath>

#include <utils/log.h>
#define VALUEMODIFIER_IMPLEMENTATION
#include <utils/value_modifier.h>

namespace MESHAPI
{

static GLuint programObject = 0;
static real32 angle = 0.0f;

static GLfloat rot[] = { // Column Major
   1.f, 0.f, 0.f, 0.f, //Column 0
   0.f, 1.f, 0.f, 0.f, //Column 1
   0.f, 0.f, 1.f, 0.f, //Column 2
   0.f, 0.f, 0.f, 1.f, //Column 3
};
static GLfloat ratio[] = { // Column Major
   1.f, 0.f, 0.f, 0.f, //Column 0
   0.f, 1.f, 0.f, 0.f, //Column 1
   0.f, 0.f, 1.f, 0.f, //Column 2
   0.f, 0.f, 0.f, 1.f, //Column 3
};

static GLfloat color[] = { 1.0f /*r*/, 0.0f /*g*/, 0.0f /*b*/};

GLuint LoadShader(const char *shaderSrc, GLenum type)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);
    if(shader == 0)
    return 0;
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if(!compiled)
    {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if(infoLen > 1)
    {
    char* infoLog = (char*)malloc(sizeof(char) * infoLen);
    glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
    ERRORLOG("Error compiling shader:\n%s\n", infoLog);
    free(infoLog);
    }
    glDeleteShader(shader);
    return 0;
    }
    return shader;
}

bool InitGLTest()
{
#ifdef EGL
   GLbyte vShaderStr[] =
      "#version 140 \n"
      "attribute vec4 vPosition; \n"
      "uniform mat4 rot; \n"
      "uniform mat4 ratio; \n"
      "void main() \n"
      "{ \n"
      " gl_Position = ratio * rot * vec4(vPosition.xyz, 1.0); \n"
      "} \n";

   GLbyte fShaderStr[] =
      "#version 140 \n"
      "precision mediump float; \n"
      "uniform vec3 color; \n"
      "void main() \n"
      "{ \n"
      " gl_FragColor = vec4(color, 1.0); \n"
      "} \n";
#else
   // GLbyte vShaderStr[] =
   //    "#version 330 core\n"
   //    "in vec4 vPosition; \n"
   //    "uniform mat4 rot; \n"
   //    "uniform mat4 ratio; \n"
   //    "void main() \n"
   //    "{ \n"
   //    " gl_Position = ratio * rot * vec4(vPosition.xyz, 1.0); \n"
   //    "} \n";

   // GLbyte fShaderStr[] =
   //    "#version 330 core \n"
   //    "uniform vec3 color; \n"
   //    "out vec4 FragColor; \n"
   //    "void main() \n"
   //    "{ \n"
   //    " FragColor = vec4(color, 1.0); \n"
   //    "} \n";

   const char *vShaderStr = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
   const char *fShaderStr = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
#endif
   
   GLuint vertexShader;
   GLuint fragmentShader;
   GLint linked;

   vertexShader = LoadShader( (char*) vShaderStr, GL_VERTEX_SHADER );
   fragmentShader = LoadShader( (char*) fShaderStr, GL_FRAGMENT_SHADER);

   programObject = glCreateProgram();
   if(programObject == 0)     return false;

   glAttachShader(programObject, vertexShader);
   glAttachShader(programObject, fragmentShader);

#ifdef EGL
   glBindAttribLocation(programObject, 0, "vPosition");
#endif

   glLinkProgram(programObject);

   glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
   if(!linked)
   {
      GLint infoLen = 0;
      glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

      if(infoLen > 1)
      {
         char* infoLog = (char*) malloc(sizeof(char) * infoLen);
         glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
         ERRORLOG("Error linking program:\n%s\n", infoLog);

         free(infoLog);
      }
      glDeleteProgram(programObject);
      return false;
   }

   return true;
}

void LearnOpenGL()
{
   float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left  
      0.5f, -0.5f, 0.0f, // right 
      0.0f,  0.5f, 0.0f  // top   
   }; 

   unsigned int VBO, VAO;
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0); 

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   glBindVertexArray(0);

   glUseProgram(programObject);
   glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
   glDrawArrays(GL_TRIANGLES, 0, 3);
}



static void setRotation(GLfloat* rot, real32 angle)
{
   rot[0 + 0 * 4] =  cos(angle);
   rot[1 + 0 * 4] = -sin(angle);

   rot[0 + 1 * 4] =  sin(angle);
   rot[1 + 1 * 4] =  cos(angle);

}

static void Draw( GLfloat aspectRatio)
{
   GLfloat vVertices[] = {
      -0.5f, -0.5f, 0.0f, 
      +0.5f, -0.5f, 0.0f, 
      +0.5f, +0.5f, 0.0f,

      -0.5f, -0.5f, 0.0f, 
      +0.5f, +0.5f, 0.0f, 
      -0.5f, +0.5f, 0.0f
   };

   glUseProgram(programObject);
   setRotation(rot, angle);

   static GLuint rot_pos = glGetUniformLocation(programObject, "rot");
   glUniformMatrix4fv(	rot_pos, 1, GL_FALSE, &rot[0]);

   ratio[0 + 0 * 4] = aspectRatio; // Squash the x-axis
   
   static GLuint ratio_pos = glGetUniformLocation(programObject, "ratio");
   glUniformMatrix4fv(	ratio_pos, 1, GL_FALSE, &ratio[0]);

   static GLuint color_pos = glGetUniformLocation(programObject, "color");
   glUniform3f(color_pos, color[0], color[1], color[2]);

   // Load the vertex data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
   glEnableVertexAttribArray(0);

   glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void LogUserInput(const UserInput& input)
{
   LOG("Horizontal: %.1f\n", input.Horizontal);
   LOG("Vertical: %.1f\n", input.Vertical);
   LOG("Cancel: %.1f\n", input.Cancel);
   LOG("Fire1: %.1f\n", input.Fire1);
   LOG("Fire2: %.1f\n", input.Fire2);
   LOG("Fire3: %.1f\n", input.Fire3);
   LOG("Jump: %.1f\n", input.Jump);
   LOG("MouseScrollWheel: %.1f\n", input.MouseScrollWheel);
   LOG("MouseX: %.1f\n", input.MouseX);
   LOG("MouseY: %.1f\n", input.MouseY);
   LOG("Submit: %.1f\n", input.Submit);
}

inline void LogUserMouse(const UserInput& input)
{
   LOG("MouseLeft: %.1f\n", input.Fire2);
   LOG("MouseRight: %.1f\n", input.Fire3);
   LOG("MouseWheel: %.1f\n", input.MouseScrollWheel);
   LOG("MouseX: %.1f\n", input.MouseX);
   LOG("MouseY: %.1f\n", input.MouseY);
}

bool UpdateAndRender(real32 dt)
{
   UserInput input{};
   QueryUserInput(input);
   
   constexpr GLfloat aspectRatio = (GLfloat) 9 / (GLfloat) 16;
   glClearColor( 1.f, 0.f, 1.f, 1.f );
   glClear( GL_COLOR_BUFFER_BIT );

   rot[12] += input.Horizontal * dt;
   rot[13] += input.Vertical * dt;
   clamp(rot[12], -1, 1);
   clamp(rot[13], -1, 1);

   angle += input.Fire1 * dt + 3.4f * input.MouseScrollWheel * dt;
   color[0] += input.Fire2 * dt;
   color[1] += input.Fire3 * dt;
   color[2] += (input.Fire3 > 0 && input.Fire2 > 0) * dt;
   loop(color[0], 0.0f, 1.0f);
   loop(color[1], 0.0f, 1.0f);
   loop(color[2], 0.0f, 1.0f);

   // Draw(aspectRatio);
   LearnOpenGL();

   return true;
}

}

#if _WIN64 && defined(RELEASE)
int __stdcall WinMain(
    HINSTANCE hinstance,  // handle to current instance 
    HINSTANCE hinstPrev,  // handle to previous instance 
    LPSTR lpCmdLine,      // address of command-line string 
    int nCmdShow         // show-window type 
)
#else
int main(int argc, char* argv[])
#endif
{
   using namespace MESHAPI;
   initPlatformLayer();
   
   if(!InitGLTest())
   {
      return -1;
   }

   LOG("Starting game loop\n");

   return startGameloop( &UpdateAndRender );
}