#include "../../src/sky/SkyboxRenderer.hpp"

SkyboxRenderer::SkyboxRenderer()
{
}

SkyboxRenderer::SkyboxRenderer(const SkyboxRenderer &other)
{
    *this = other;
}

SkyboxRenderer::~SkyboxRenderer()
{
}

SkyboxRenderer &SkyboxRenderer::operator=(const SkyboxRenderer &other)
{
    (void)other;
    return (*this);
}

uint32_t SkyboxRenderer::skybox_row_color(int32_t y, int32_t height)
{
    double factor;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    if (height <= 1)
        return (RendererColor::rgba(94U, 164U, 225U));
    factor = static_cast<double>(y) / static_cast<double>(height - 1);
    red = static_cast<uint8_t>(82.0 + (62.0 * factor));
    green = static_cast<uint8_t>(154.0 + (44.0 * factor));
    blue = static_cast<uint8_t>(224.0 + (22.0 * factor));
    if (y > (height / 2))
    {
        red = static_cast<uint8_t>(66.0 + (32.0 * factor));
        green = static_cast<uint8_t>(132.0 + (38.0 * factor));
        blue = static_cast<uint8_t>(202.0 + (26.0 * factor));
    }
    return (RendererColor::rgba(red, green, blue));
}

void SkyboxRenderer::draw_skybox_sun(ft_render_framebuffer &framebuffer,
    const Camera &camera)
{
    const double sun_radius = 38.0;
    double sun_x;
    double sun_y;
    double delta_x;
    double delta_y;
    double distance_squared;
    int32_t start_x;
    int32_t end_x;
    int32_t start_y;
    int32_t end_y;
    int32_t x;
    int32_t y;
    size_t pixel_index;

    sun_x = (static_cast<double>(framebuffer.width) * 0.5)
        + (std::sin(camera.yaw) * static_cast<double>(framebuffer.width)
            * 0.32);
    sun_y = (static_cast<double>(framebuffer.height) * 0.22)
        + (camera.pitch * static_cast<double>(framebuffer.height) * 0.18);
    start_x = static_cast<int32_t>(std::floor(sun_x - sun_radius));
    end_x = static_cast<int32_t>(std::ceil(sun_x + sun_radius));
    start_y = static_cast<int32_t>(std::floor(sun_y - sun_radius));
    end_y = static_cast<int32_t>(std::ceil(sun_y + sun_radius));
    if (start_x < 0)
        start_x = 0;
    if (start_y < 0)
        start_y = 0;
    if (end_x >= framebuffer.width)
        end_x = framebuffer.width - 1;
    if (end_y >= framebuffer.height)
        end_y = framebuffer.height - 1;
    y = start_y;
    while (y <= end_y)
    {
        x = start_x;
        while (x <= end_x)
        {
            delta_x = static_cast<double>(x) - sun_x;
            delta_y = static_cast<double>(y) - sun_y;
            distance_squared = (delta_x * delta_x) + (delta_y * delta_y);
            pixel_index = static_cast<size_t>((y * framebuffer.width) + x);
            if (distance_squared < 360.0)
                framebuffer.pixels[pixel_index] = RendererColor::rgba(254U, 238U,
                    160U);
            else if (distance_squared < 1440.0)
                framebuffer.pixels[pixel_index] = RendererColor::rgba(182U, 208U,
                    226U);
            x = x + 1;
        }
        y = y + 1;
    }
}

void SkyboxRenderer::clear_frame(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const Camera &camera)
{
    int32_t y;
    uint32_t sky_color;
    size_t row_index;

    y = 0;
    while (y < framebuffer.height)
    {
        row_index = static_cast<size_t>(y * framebuffer.width);
        sky_color = skybox_row_color(y, framebuffer.height);
        std::fill_n(framebuffer.pixels + row_index, framebuffer.width,
            sky_color);
        std::fill_n(depth_buffer.data() + row_index, framebuffer.width, 1.0e30);
        y = y + 1;
    }
    draw_skybox_sun(framebuffer, camera);
}
