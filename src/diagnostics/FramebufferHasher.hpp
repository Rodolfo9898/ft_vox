#ifndef FRAMEBUFFER_HASHER_HPP
# define FRAMEBUFFER_HASHER_HPP

#include "../../Libft/Modules/DUMB/render_window.hpp"
#include <cstdint>
#include <cstddef>

class FramebufferHasher
{
    public:
        FramebufferHasher();
        FramebufferHasher(const FramebufferHasher &other);
        ~FramebufferHasher();
        FramebufferHasher &operator=(const FramebufferHasher &other);

        static uint64_t hash_framebuffer(const ft_render_framebuffer &framebuffer);
};

#endif
