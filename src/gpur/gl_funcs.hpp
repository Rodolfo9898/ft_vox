#ifndef GPUR_GL_FUNCS_HPP
# define GPUR_GL_FUNCS_HPP

#ifndef GL_SILENCE_DEPRECATION
# define GL_SILENCE_DEPRECATION
#endif

#ifdef __APPLE__
# include <OpenGL/gl3.h>
#else
# include <GL/gl.h>
# include <GL/glext.h>
# include <GL/glx.h>

extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
extern PFNGLVERTEXATTRIBIPOINTERPROC     glVertexAttribIPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORM3FVPROC               glUniform3fv;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLUNIFORM2FPROC                glUniform2f;
extern PFNGLUNIFORM1FPROC                glUniform1f;
extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
extern PFNGLGENERATEMIPMAPPROC           glGenerateMipmap;

bool gpur_load_gl_functions() noexcept;

#endif

#endif
