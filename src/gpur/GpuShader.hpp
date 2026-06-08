#ifndef GPUR_SHADER_HPP
# define GPUR_SHADER_HPP

#include "../../src/gpur/gl_funcs.hpp"
#include <cstdint>

class GpuShader
{
    GLuint _program;

    public:
        GpuShader() noexcept : _program(0) {}
        ~GpuShader() noexcept { destroy(); }

        GpuShader(const GpuShader &)            = delete;
        GpuShader &operator=(const GpuShader &) = delete;

        bool   compile(const char *vert_src, const char *frag_src) noexcept;
        void   destroy() noexcept;
        void   use() const noexcept;
        GLint  uniform(const char *name) const noexcept;
        bool   ready() const noexcept { return _program != 0; }
};

#endif
