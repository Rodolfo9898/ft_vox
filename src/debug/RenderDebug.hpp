#ifndef RENDER_DEBUG_HPP
# define RENDER_DEBUG_HPP

#include <cstdint>

class RenderDebug
{
    public:
        double fps;
        double frame_ms;
        int32_t visible_chunks;
        int32_t loaded_chunks;
        int32_t render_distance;
        uint32_t selected_block_id;

        RenderDebug();
        RenderDebug(const RenderDebug &other);
        ~RenderDebug();
        RenderDebug &operator=(const RenderDebug &other);

        void clear();
};

#endif
