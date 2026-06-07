#include "../../src/diagnostics/FramebufferHasher.hpp"

FramebufferHasher::FramebufferHasher()
{
}

FramebufferHasher::FramebufferHasher(const FramebufferHasher &other)
{
    *this = other;
}

FramebufferHasher::~FramebufferHasher()
{
}

FramebufferHasher &FramebufferHasher::operator=(const FramebufferHasher &other)
{
    (void)other;
    return (*this);
}

uint64_t FramebufferHasher::hash_framebuffer(const ft_render_framebuffer &framebuffer)
{
    const uint64_t fnv_offset_basis = UINT64_C(1469598103934665603);
    const uint64_t fnv_prime = UINT64_C(1099511628211);
    uint64_t hash;
    size_t index;
    size_t pixel_count;

    if (framebuffer.pixels == nullptr || framebuffer.width <= 0
        || framebuffer.height <= 0)
        return (UINT64_C(0));
    hash = fnv_offset_basis;
    pixel_count = static_cast<size_t>(framebuffer.width)
        * static_cast<size_t>(framebuffer.height);
    index = 0;
    while (index < pixel_count)
    {
        hash ^= static_cast<uint64_t>(framebuffer.pixels[index]);
        hash *= fnv_prime;
        index += 1;
    }
    return (hash);
}
