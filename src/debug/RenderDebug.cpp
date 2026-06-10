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
        this->ram_mb = other.ram_mb;
        this->vram_approx_mb = other.vram_approx_mb;
        this->camera_x = other.camera_x;
        this->camera_y = other.camera_y;
        this->camera_z = other.camera_z;
        this->camera_speed = other.camera_speed;
        this->boost_speed  = other.boost_speed;
        std::memcpy(this->seed,       other.seed,       sizeof(this->seed));
        std::memcpy(this->biome_name, other.biome_name, sizeof(this->biome_name));
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
    this->ram_mb = 0U;
    this->vram_approx_mb = 0U;
    this->camera_x = 0.0;
    this->camera_y = 0.0;
    this->camera_z = 0.0;
    this->camera_speed = 0.0;
    this->boost_speed  = 0.0;
    this->seed[0]       = '\0';
    this->biome_name[0] = '\0';
}
