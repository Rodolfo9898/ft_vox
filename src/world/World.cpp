#include "../../src/world/World.hpp"

World::World()
{
    this->chunk_count = WORLD_CHUNK_COUNT;
    this->loaded_chunk_count = 0;
    this->center_chunk_x = 0;
    this->center_chunk_z = 0;
    this->active_render_distance = WORLD_REQUIRED_VISIBLE_DISTANCE;
    this->seed[0] = '\0';
}

World::~World()
{
    this->destroy();
}

void World::copy_seed(const char *seed_value)
{
    if (seed_value == nullptr)
        seed_value = "";
    std::strncpy(this->seed, seed_value, sizeof(this->seed) - 1U);
    this->seed[sizeof(this->seed) - 1U] = '\0';
}

int32_t World::initialize(const char *seed_value)
{
    int32_t index;
    int32_t error_code;

    this->destroy();
    this->chunk_count = WORLD_CHUNK_COUNT;
    this->loaded_chunk_count = 0;
    this->center_chunk_x = 0;
    this->center_chunk_z = 0;
    this->active_render_distance = WORLD_REQUIRED_VISIBLE_DISTANCE;
    this->copy_seed(seed_value);
    index = 0;
    while (index < this->chunk_count)
    {
        this->chunks[index].reset_coordinates();
        this->chunks[index].initialized = false;
        index = index + 1;
    }
    error_code = this->update_around(0.0, 0.0, WORLD_CHUNK_COUNT,
        this->active_render_distance);
    if (error_code != FT_ERR_SUCCESS)
        this->destroy();
    return (error_code);
}

void World::destroy()
{
    int32_t index;

    index = 0;
    while (index < this->chunk_count)
    {
        this->chunks[index].destroy();
        index = index + 1;
    }
    this->loaded_chunk_count = 0;
}

int32_t World::update_around(double camera_x, double camera_z,
    int32_t generation_budget)
{
    return (this->update_around(camera_x, camera_z, generation_budget,
        WORLD_REQUIRED_VISIBLE_DISTANCE));
}

int32_t World::update_around(double camera_x, double camera_z,
    int32_t generation_budget, int32_t render_distance)
{
    int32_t stream_chunk_radius;
    int32_t radius;
    int32_t offset_x;
    int32_t offset_z;
    int32_t generated_count;
    int32_t error_code;

    this->center_chunk_x = WorldCoordinates::floor_divide(
        static_cast<int32_t>(std::floor(camera_x)), GAME_VOXEL_CHUNK_WIDTH);
    this->center_chunk_z = WorldCoordinates::floor_divide(
        static_cast<int32_t>(std::floor(camera_z)), GAME_VOXEL_CHUNK_DEPTH);
    this->active_render_distance = WorldCoordinates::clamp_int(render_distance,
        WORLD_MIN_RENDER_DISTANCE,
        WORLD_CACHE_CHUNK_RADIUS * GAME_VOXEL_CHUNK_WIDTH);
    stream_chunk_radius = WorldCoordinates::render_distance_to_chunk_radius(
        this->active_render_distance);
    WorldChunkStore::evict_far_chunks(this->chunks, this->chunk_count,
        &this->loaded_chunk_count, this->center_chunk_x, this->center_chunk_z);
    generated_count = 0;
    radius = 0;
    while (radius <= stream_chunk_radius)
    {
        offset_z = -radius;
        while (offset_z <= radius)
        {
            offset_x = -radius;
            while (offset_x <= radius)
            {
                if ((offset_x == -radius || offset_x == radius
                        || offset_z == -radius || offset_z == radius)
                    && WorldCoordinates::chunk_distance_squared(
                        this->center_chunk_x + offset_x,
                        this->center_chunk_z + offset_z,
                        this->center_chunk_x, this->center_chunk_z)
                        <= stream_chunk_radius * stream_chunk_radius)
                {
                    if (WorldChunkStore::find_chunk(this->chunks, this->chunk_count, this->center_chunk_x + offset_x,
                            this->center_chunk_z + offset_z) == nullptr)
                    {
                        error_code = WorldChunkLoader::generate_missing_chunk(
                            this->chunks, this->chunk_count,
                            &this->loaded_chunk_count,
                            this->center_chunk_x + offset_x,
                            this->center_chunk_z + offset_z, this->seed);
                        if (error_code != FT_ERR_SUCCESS)
                            return (error_code);
                        generated_count = generated_count + 1;
                        if (generation_budget > 0
                            && generated_count >= generation_budget)
                            return (FT_ERR_SUCCESS);
                    }
                }
                offset_x = offset_x + 1;
            }
            offset_z = offset_z + 1;
        }
        radius = radius + 1;
    }
    return (FT_ERR_SUCCESS);
}

bool World::validate_visible_distance(double camera_x, double camera_z,
    double yaw, int32_t required_distance) const
{
    return (WorldVisibilityValidator::validate_visible_distance(*this, camera_x,
        camera_z, yaw, required_distance));
}

bool World::surface_top_at(int32_t world_x, int32_t world_z,
    double *surface_top) const
{
    return (WorldBlockQuery::surface_top_at(*this, world_x, world_z,
        surface_top));
}

bool World::solid_block_at(int32_t world_x, int32_t world_y,
    int32_t world_z) const
{
    return (WorldBlockQuery::solid_block_at(*this, world_x, world_y, world_z));
}

bool World::block_id_at(int32_t world_x, int32_t world_y, int32_t world_z,
    uint32_t *block_id) const
{
    return (WorldBlockQuery::block_id_at(*this, world_x, world_y, world_z,
        block_id));
}

int32_t World::delete_block_at(int32_t world_x, int32_t world_y,
    int32_t world_z)
{
    return (WorldBlockEditor::delete_block_at(*this, world_x, world_y,
        world_z));
}

int32_t World::place_block_at(int32_t world_x, int32_t world_y,
    int32_t world_z, uint32_t block_id)
{
    return (WorldBlockEditor::place_block_at(*this, world_x, world_y,
        world_z, block_id));
}

int32_t World::raycast_solid(double origin_x, double origin_y, double origin_z,
    double direction_x, double direction_y, double direction_z,
    double max_distance, int32_t *block_x, int32_t *block_y,
    int32_t *block_z) const
{
    return (WorldRaycaster::raycast_solid(*this, origin_x, origin_y, origin_z,
        direction_x, direction_y, direction_z, max_distance, block_x, block_y,
        block_z));
}

int32_t World::raycast_edit_target(double origin_x, double origin_y,
    double origin_z, double direction_x, double direction_y,
    double direction_z, double max_distance, int32_t *hit_block_x,
    int32_t *hit_block_y, int32_t *hit_block_z, int32_t *place_block_x,
    int32_t *place_block_y, int32_t *place_block_z,
    uint32_t *hit_block_id) const
{
    return (WorldRaycaster::raycast_edit_target(*this, origin_x, origin_y,
        origin_z, direction_x, direction_y, direction_z, max_distance,
        hit_block_x, hit_block_y, hit_block_z, place_block_x, place_block_y,
        place_block_z, hit_block_id));
}
