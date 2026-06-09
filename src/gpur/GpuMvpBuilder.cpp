#include "../../src/gpur/GpuMvpBuilder.hpp"

GpuMvpBuilder::GpuMvpBuilder() {}
GpuMvpBuilder::GpuMvpBuilder(const GpuMvpBuilder &other) { (void)other; }
GpuMvpBuilder::~GpuMvpBuilder() {}
GpuMvpBuilder &GpuMvpBuilder::operator=(const GpuMvpBuilder &other)
{ (void)other; return *this; }

void GpuMvpBuilder::build(float out[16], const Camera &camera,
    int width, int height, float near_z, float far_z) noexcept
{
    double cy = std::cos(camera.yaw);
    double sy = std::sin(camera.yaw);
    double cp = std::cos(camera.pitch);
    double sp = std::sin(camera.pitch);

    double tx = -camera.x, ty = -camera.y, tz = -camera.z;
    double vx  =  tx * cy - tz * sy;
    double vy  =  ty;
    double vz  =  tx * sy + tz * cy;
    double vy2 =  vy * cp - vz * sp;
    double vz2 =  vy * sp + vz * cp;

    float view[16];
    std::memset(view, 0, sizeof(view));
    view[0]  = static_cast<float>(cy);
    view[2]  = static_cast<float>(-sy);
    view[4]  = static_cast<float>(-sy * sp);
    view[5]  = static_cast<float>(cp);
    view[6]  = static_cast<float>(-cy * sp);
    view[8]  = static_cast<float>(sy * cp);
    view[9]  = static_cast<float>(sp);
    view[10] = static_cast<float>(cy * cp);
    view[12] = static_cast<float>(vx);
    view[13] = static_cast<float>(vy2);
    view[14] = static_cast<float>(vz2);
    view[15] = 1.0f;

    static const float FOV_TAN = 0.83909963118f;
    float fx = 1.0f / FOV_TAN;
    float fy = fx * (static_cast<float>(width)  * 0.5f)
                   / (static_cast<float>(height) * 0.52f);
    float fn = far_z + near_z;
    float fd = far_z - near_z;
    float A  = fn / fd;
    float B  = -2.0f * near_z * far_z / fd;

    float proj[16];
    std::memset(proj, 0, sizeof(proj));
    proj[0] = fx; proj[5] = fy;
    proj[10] = A; proj[11] = 1.0f;
    proj[14] = B;

    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
        {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k)
                s += proj[k * 4 + row] * view[col * 4 + k];
            out[col * 4 + row] = s;
        }
}
