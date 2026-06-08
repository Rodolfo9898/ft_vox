#include "../../src/gpur/gl_funcs.hpp"

#ifndef __APPLE__

PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays         = nullptr;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray         = nullptr;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays      = nullptr;
PFNGLGENBUFFERSPROC              glGenBuffers              = nullptr;
PFNGLBINDBUFFERPROC              glBindBuffer              = nullptr;
PFNGLBUFFERDATAPROC              glBufferData              = nullptr;
PFNGLDELETEBUFFERSPROC           glDeleteBuffers           = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC    glVertexAttribIPointer    = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLCREATESHADERPROC            glCreateShader            = nullptr;
PFNGLSHADERSOURCEPROC            glShaderSource            = nullptr;
PFNGLCOMPILESHADERPROC           glCompileShader           = nullptr;
PFNGLGETSHADERIVPROC             glGetShaderiv             = nullptr;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog        = nullptr;
PFNGLDELETESHADERPROC            glDeleteShader            = nullptr;
PFNGLCREATEPROGRAMPROC           glCreateProgram           = nullptr;
PFNGLATTACHSHADERPROC            glAttachShader            = nullptr;
PFNGLLINKPROGRAMPROC             glLinkProgram             = nullptr;
PFNGLGETPROGRAMIVPROC            glGetProgramiv            = nullptr;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog       = nullptr;
PFNGLUSEPROGRAMPROC              glUseProgram              = nullptr;
PFNGLDELETEPROGRAMPROC           glDeleteProgram           = nullptr;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation      = nullptr;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv        = nullptr;
PFNGLUNIFORM1IPROC               glUniform1i               = nullptr;
PFNGLUNIFORM3FVPROC              glUniform3fv              = nullptr;
PFNGLUNIFORM4FVPROC              glUniform4fv              = nullptr;
PFNGLUNIFORM2FPROC               glUniform2f               = nullptr;
PFNGLUNIFORM1FPROC               glUniform1f               = nullptr;
PFNGLACTIVETEXTUREPROC           glActiveTexture           = nullptr;
PFNGLGENERATEMIPMAPPROC          glGenerateMipmap          = nullptr;

#define GPUR_LOAD(name) \
    name = reinterpret_cast<decltype(name)>(glXGetProcAddressARB( \
        reinterpret_cast<const GLubyte *>(#name))); \
    if (name == nullptr) return false

bool gpur_load_gl_functions() noexcept
{
    GPUR_LOAD(glGenVertexArrays);
    GPUR_LOAD(glBindVertexArray);
    GPUR_LOAD(glDeleteVertexArrays);
    GPUR_LOAD(glGenBuffers);
    GPUR_LOAD(glBindBuffer);
    GPUR_LOAD(glBufferData);
    GPUR_LOAD(glDeleteBuffers);
    GPUR_LOAD(glVertexAttribIPointer);
    GPUR_LOAD(glEnableVertexAttribArray);
    GPUR_LOAD(glCreateShader);
    GPUR_LOAD(glShaderSource);
    GPUR_LOAD(glCompileShader);
    GPUR_LOAD(glGetShaderiv);
    GPUR_LOAD(glGetShaderInfoLog);
    GPUR_LOAD(glDeleteShader);
    GPUR_LOAD(glCreateProgram);
    GPUR_LOAD(glAttachShader);
    GPUR_LOAD(glLinkProgram);
    GPUR_LOAD(glGetProgramiv);
    GPUR_LOAD(glGetProgramInfoLog);
    GPUR_LOAD(glUseProgram);
    GPUR_LOAD(glDeleteProgram);
    GPUR_LOAD(glGetUniformLocation);
    GPUR_LOAD(glUniformMatrix4fv);
    GPUR_LOAD(glUniform1i);
    GPUR_LOAD(glUniform3fv);
    GPUR_LOAD(glUniform4fv);
    GPUR_LOAD(glUniform2f);
    GPUR_LOAD(glUniform1f);
    GPUR_LOAD(glActiveTexture);
    GPUR_LOAD(glGenerateMipmap);
    return true;
}

#undef GPUR_LOAD

#endif
