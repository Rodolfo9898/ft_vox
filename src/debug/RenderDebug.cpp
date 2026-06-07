#include "../../src/debug/RenderDebug.hpp"

RenderDebug::RenderDebug()
{
    this->clear();
}

RenderDebug::RenderDebug(const RenderDebug &other)
{
    *this = other;
}

RenderDebug::~RenderDebug()
{
}

RenderDebug &RenderDebug::operator=(const RenderDebug &other)
{
    if (this != &other)
    {
        this->fps = other.fps;
        this->frame_ms = other.frame_ms;
        this->visible_chunks = other.visible_chunks;
        this->loaded_chunks = other.loaded_chunks;
        this->render_distance = other.render_distance;
        this->selected_block_id = other.selected_block_id;
    }
    return (*this);
}

void RenderDebug::clear()
{
    this->fps = 0.0;
    this->frame_ms = 0.0;
    this->visible_chunks = 0;
    this->loaded_chunks = 0;
    this->render_distance = 0;
    this->selected_block_id = 0U;
}
