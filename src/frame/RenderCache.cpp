#include "../../src/frame/RenderCache.hpp"

RenderCache::RenderCache()
{
    this->yaw_cos = 1.0;
    this->yaw_sin = 0.0;
    this->pitch_cos = 1.0;
    this->pitch_sin = 0.0;
    this->fov_tangent = 1.0;
    this->aspect_ratio = 1.0;
    this->focal_length = 1.0;
    this->half_width = 0.0;
    this->half_height = 0.0;
    this->render_distance = 0.0;
}

RenderCache::RenderCache(const RenderCache &other)
{
    *this = other;
}

RenderCache::~RenderCache()
{
}

RenderCache &RenderCache::operator=(const RenderCache &other)
{
    if (this != &other)
    {
        this->yaw_cos = other.yaw_cos;
        this->yaw_sin = other.yaw_sin;
        this->pitch_cos = other.pitch_cos;
        this->pitch_sin = other.pitch_sin;
        this->fov_tangent = other.fov_tangent;
        this->aspect_ratio = other.aspect_ratio;
        this->focal_length = other.focal_length;
        this->half_width = other.half_width;
        this->half_height = other.half_height;
        this->render_distance = other.render_distance;
    }
    return (*this);
}

void RenderCache::configure(const Camera &camera, int32_t width, int32_t height,
    int32_t active_render_distance)
{
    this->yaw_cos = std::cos(camera.yaw);
    this->yaw_sin = std::sin(camera.yaw);
    this->pitch_cos = std::cos(camera.pitch);
    this->pitch_sin = std::sin(camera.pitch);
    this->fov_tangent = std::tan(40.0 * 0.017453292519943295);
    this->aspect_ratio = (static_cast<double>(width) * 0.5)
        / (static_cast<double>(height) * 0.52);
    this->focal_length = (static_cast<double>(width) * 0.5)
        / this->fov_tangent;
    this->half_width = static_cast<double>(width) * 0.5;
    this->half_height = static_cast<double>(height) * 0.52;
    this->render_distance = static_cast<double>(active_render_distance);
}
