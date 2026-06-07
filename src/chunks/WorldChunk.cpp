#include "../../src/chunks/WorldChunk.hpp"

WorldChunk::WorldChunk()
{
    this->reset_coordinates();
    this->initialized = false;
}

WorldChunk::~WorldChunk()
{
    this->destroy();
}

void WorldChunk::reset_coordinates()
{
    this->chunk_x = 0;
    this->chunk_z = 0;
    this->world_x = 0;
    this->world_z = 0;
}

void WorldChunk::destroy()
{
    if (this->initialized == false)
        return ;
    (void)chunk_mesh_destroy(this->mesh);
    (void)this->chunk.destroy();
    this->initialized = false;
}
