#ifndef GPU_MVP_BUILDER_HPP
# define GPU_MVP_BUILDER_HPP

#include "../../src/camera/Camera.hpp"
#include <cmath>
#include <cstring>

class GpuMvpBuilder
{
    public:
        GpuMvpBuilder();
        GpuMvpBuilder(const GpuMvpBuilder &other);
        ~GpuMvpBuilder();
        GpuMvpBuilder &operator=(const GpuMvpBuilder &other);

        static void build(float out[16], const Camera &camera,
            int width, int height,
            float near_z, float far_z) noexcept;
};

#endif
