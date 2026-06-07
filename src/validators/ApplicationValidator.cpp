#include "../../src/validators/ApplicationValidator.hpp"

ApplicationValidator::ApplicationValidator()
{
}

ApplicationValidator::ApplicationValidator(const ApplicationValidator &other)
{
    *this = other;
}

ApplicationValidator::~ApplicationValidator()
{
}

ApplicationValidator &ApplicationValidator::operator=(const ApplicationValidator &other)
{
    (void)other;
    return (*this);
}

int ApplicationValidator::validate_camera_speed()
{
    return (CameraSpeedValidator::validate());
}

int ApplicationValidator::validate_collision()
{
    return (CollisionValidator::validate());
}

int ApplicationValidator::validate_block_edit()
{
    return (BlockEditValidator::validate());
}

int ApplicationValidator::validate_visible_distance()
{
    World world;
    Camera validation_camera;
    int32_t error_code;

    error_code = world.initialize("integration-seed");
    if (error_code != FT_ERR_SUCCESS)
        return (1);
    validation_camera.initialize();
    PlayerController::spawn_player_on_ground(&validation_camera, world);
    error_code = world.update_around(validation_camera.x,
        validation_camera.z, WORLD_CHUNK_COUNT);
    if (error_code != FT_ERR_SUCCESS)
    {
        world.destroy();
        return (1);
    }
    if (world.validate_visible_distance(validation_camera.x,
            validation_camera.z, validation_camera.yaw,
            WORLD_REQUIRED_VISIBLE_DISTANCE) == false)
    {
        std::fprintf(stderr,
            "visible-distance: failed required=%d chunks_loaded=%d\n",
            WORLD_REQUIRED_VISIBLE_DISTANCE, world.loaded_chunk_count);
        world.destroy();
        return (1);
    }
    std::printf("visible-distance: ok required=%d chunks_loaded=%d\n",
        WORLD_REQUIRED_VISIBLE_DISTANCE, world.loaded_chunk_count);
    world.destroy();
    return (0);
}

int ApplicationValidator::validate_terrain_determinism()
{
    return (TerrainDeterminismValidator::validate());
}

int ApplicationValidator::validate_world_scale()
{
    return (WorldScaleValidator::validate());
}

int ApplicationValidator::validate_caves()
{
    return (TerrainCaveValidator::validate());
}
