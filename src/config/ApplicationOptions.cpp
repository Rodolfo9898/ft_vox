#include "../../src/config/ApplicationOptions.hpp"

ApplicationOptions::ApplicationOptions()
{
    this->perf_test_mode = false;
    this->perf_headless_mode = false;
    this->perf_move_mode = false;
    this->perf_boost_mode = false;
    this->validate_camera_speed_mode = false;
    this->validate_collision_mode = false;
    this->validate_block_edit_mode = false;
    this->validate_visible_distance_mode = false;
    this->validate_terrain_determinism_mode = false;
    this->validate_world_scale_mode = false;
    this->validate_caves_mode = false;
    this->perf_seconds_limit = 10.0;
}

ApplicationOptions::ApplicationOptions(const ApplicationOptions &other)
{
    *this = other;
}

ApplicationOptions::~ApplicationOptions()
{
}

ApplicationOptions &ApplicationOptions::operator=(const ApplicationOptions &other)
{
    if (this != &other)
    {
        this->perf_test_mode = other.perf_test_mode;
        this->perf_headless_mode = other.perf_headless_mode;
        this->perf_move_mode = other.perf_move_mode;
        this->perf_boost_mode = other.perf_boost_mode;
        this->validate_camera_speed_mode = other.validate_camera_speed_mode;
        this->validate_collision_mode = other.validate_collision_mode;
        this->validate_block_edit_mode = other.validate_block_edit_mode;
        this->validate_visible_distance_mode = other.validate_visible_distance_mode;
        this->validate_terrain_determinism_mode
            = other.validate_terrain_determinism_mode;
        this->validate_world_scale_mode = other.validate_world_scale_mode;
        this->validate_caves_mode = other.validate_caves_mode;
        this->perf_seconds_limit = other.perf_seconds_limit;
    }
    return (*this);
}

int ApplicationOptions::parse(int argc, char **argv)
{
    int result;

    this->perf_headless_mode = CommandLine::has_flag(argc, argv,
        "--perf-test-headless");
    this->perf_test_mode = CommandLine::has_flag(argc, argv, "--perf-test")
        || this->perf_headless_mode;
    this->perf_move_mode = CommandLine::has_flag(argc, argv, "--perf-move");
    this->perf_boost_mode = CommandLine::has_flag(argc, argv, "--perf-boost");
    this->validate_camera_speed_mode = CommandLine::has_flag(argc, argv,
        "--validate-camera-speed");
    this->validate_collision_mode = CommandLine::has_flag(argc, argv,
        "--validate-collision");
    this->validate_block_edit_mode = CommandLine::has_flag(argc, argv,
        "--validate-block-edit");
    this->validate_visible_distance_mode = CommandLine::has_flag(argc, argv,
        "--validate-visible-distance");
    this->validate_terrain_determinism_mode = CommandLine::has_flag(argc, argv,
        "--validate-terrain-determinism");
    this->validate_world_scale_mode = CommandLine::has_flag(argc, argv,
        "--validate-world-scale");
    this->validate_caves_mode = CommandLine::has_flag(argc, argv,
        "--validate-caves");
    result = CommandLine::parse_perf_seconds(argc, argv,
        &this->perf_seconds_limit);
    if (result < 0)
    {
        std::fprintf(stderr,
            "Application: invalid perf duration, use --perf-seconds=SECONDS\n");
        return (1);
    }
    return (0);
}
