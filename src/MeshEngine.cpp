#include "MeshEngine.h"
#include <common.h>
#include <stdio.h>
#include <stdlib.h>

#if _WIN64

#include <GLEW/glew.h>

#elif __linux__
#ifdef RASPBERRY_PI

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#endif
#endif

#include <cmath>

namespace MESHAPI
{

internal GLuint programObject = 0;

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
    printf("Error compiling shader:\n%s\n", infoLog);
    free(infoLog);
    }
    glDeleteShader(shader);
    return 0;
    }
    return shader;
}


//
// Initialize the shader and program object
//
bool InitGLTest()
{
   GLbyte vShaderStr[] =
      "attribute vec4 vPosition; \n"
      "uniform mat4 rot; \n"
      "uniform mat4 ratio; \n"
      "void main() \n"
      "{ \n"
      " gl_Position = ratio * rot * vec4(vPosition.xyz, 1.0); \n"
      "} \n";

   GLbyte fShaderStr[] =
      "precision mediump float; \n"
      "uniform vec3 color; \n"
      "void main() \n"
      "{ \n"
      " gl_FragColor = vec4(color, 1.0); \n"
      "} \n";
   GLuint vertexShader;
   GLuint fragmentShader;
   GLint linked;

   vertexShader = LoadShader( (char*) vShaderStr, GL_VERTEX_SHADER );
   fragmentShader = LoadShader( (char*) fShaderStr, GL_FRAGMENT_SHADER);

   programObject = glCreateProgram();
   if(programObject == 0)     return false;

   glAttachShader(programObject, vertexShader);
   glAttachShader(programObject, fragmentShader);

   glBindAttribLocation(programObject, 0, "vPosition");

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
         printf("Error linking program:\n%s\n", infoLog);

         free(infoLog);
      }
      glDeleteProgram(programObject);
      return false;
   }

   return true;
}




internal GLfloat rot[] = { // Column Major
   1.f, 0.f, 0.f, 0.f, //Column 0
   0.f, 1.f, 0.f, 0.f, //Column 1
   0.f, 0.f, 1.f, 0.f, //Column 2
   0.f, 0.f, 0.f, 1.f, //Column 3
};
internal GLfloat ratio[] = { // Column Major
   1.f, 0.f, 0.f, 0.f, //Column 0
   0.f, 1.f, 0.f, 0.f, //Column 1
   0.f, 0.f, 1.f, 0.f, //Column 2
   0.f, 0.f, 0.f, 1.f, //Column 3
};

void update(GLfloat* rot, float dt)
{
   rot[0 + 0 * 4] =  cos(dt);
   rot[1 + 0 * 4] = -sin(dt);

   rot[0 + 1 * 4] =  sin(dt);
   rot[1 + 1 * 4] =  cos(dt);

}

internal void Draw( GLfloat aspectRatio)
{
   // GLfloat vVertices[] = {
   //     0.0f,  0.5f, 0.0f, 
   //    -0.5f, -0.5f, 0.0f, 
   //     0.5f, -0.5f, 0.0f
   // };
   
   GLfloat vVertices[] = {
      -0.5f, -0.5f, 0.0f, 
      +0.5f, -0.5f, 0.0f, 
      +0.5f, +0.5f, 0.0f,

      -0.5f, -0.5f, 0.0f, 
      +0.5f, +0.5f, 0.0f, 
      -0.5f, +0.5f, 0.0f
   };

   glUseProgram(programObject);

   static GLuint rot_pos = glGetUniformLocation(programObject, "rot");
   glUniformMatrix4fv(	rot_pos, 1, GL_FALSE, &rot[0]);

   ratio[0 + 0 * 4] = aspectRatio; // Squash the x-axis
   
   static GLuint ratio_pos = glGetUniformLocation(programObject, "ratio");
   glUniformMatrix4fv(	ratio_pos, 1, GL_FALSE, &ratio[0]);

   static GLuint color_pos = glGetUniformLocation(programObject, "color");
   glUniform3f(color_pos, 1.0f, 1.0f, 0.f);

   // Load the vertex data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
   glEnableVertexAttribArray(0);

   glDrawArrays(GL_TRIANGLES, 0, 6);
}

internal real32 elapsed;
void UpdateAndRender(real32 dt)
{
    constexpr GLfloat aspectRatio = (GLfloat) 9 / (GLfloat) 16;

    // clear screen
    glClearColor( 1.f, 0.f, 1.f, 1.f );
    glClear( GL_COLOR_BUFFER_BIT );
    elapsed += dt * 60;
    update(&rot[0], 0.01f * elapsed);
    Draw(aspectRatio);
    //
}

}