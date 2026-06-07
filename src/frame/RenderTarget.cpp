#include "../../src/frame/RenderTarget.hpp"

RenderTarget::RenderTarget()
{
    this->target_framebuffer.width = 0;
    this->target_framebuffer.height = 0;
    this->target_framebuffer.pixels = nullptr;
}

RenderTarget::RenderTarget(const RenderTarget &other)
{
    *this = other;
}

RenderTarget::~RenderTarget()
{
}

RenderTarget &RenderTarget::operator=(const RenderTarget &other)
{
    if (this != &other)
    {
        this->target_framebuffer = other.target_framebuffer;
        this->color_buffer = other.color_buffer;
        this->target_depth_buffer = other.target_depth_buffer;
        if (this->color_buffer.empty() == false)
            this->target_framebuffer.pixels = this->color_buffer.data();
        else
            this->target_framebuffer.pixels = nullptr;
    }
    return (*this);
}

ft_render_framebuffer &RenderTarget::prepare(const ft_render_framebuffer &output)
{
    size_t required_size;

    this->target_framebuffer.width = output.width / 3;
    this->target_framebuffer.height = output.height / 3;
    if (this->target_framebuffer.width < 320)
        this->target_framebuffer.width = output.width;
    if (this->target_framebuffer.height < 180)
        this->target_framebuffer.height = output.height;
    required_size = static_cast<size_t>(this->target_framebuffer.width)
        * static_cast<size_t>(this->target_framebuffer.height);
    if (this->color_buffer.size() != required_size)
        this->color_buffer.resize(required_size);
    if (this->target_depth_buffer.size() != required_size)
        this->target_depth_buffer.resize(required_size);
    this->target_framebuffer.pixels = this->color_buffer.data();
    return (this->target_framebuffer);
}

std::vector<double> &RenderTarget::depth_buffer()
{
    return (this->target_depth_buffer);
}

void RenderTarget::blit_to(ft_render_framebuffer &destination) const
{
    int32_t x;
    int32_t y;
    int32_t source_x;
    int32_t source_y;

    if (destination.pixels == nullptr || this->target_framebuffer.pixels == nullptr)
        return ;
    y = 0;
    while (y < destination.height)
    {
        source_y = (y * this->target_framebuffer.height) / destination.height;
        x = 0;
        while (x < destination.width)
        {
            source_x = (x * this->target_framebuffer.width) / destination.width;
            destination.pixels[(y * destination.width) + x]
                = this->target_framebuffer.pixels[(source_y
                    * this->target_framebuffer.width) + source_x];
            x = x + 1;
        }
        y = y + 1;
    }
}
