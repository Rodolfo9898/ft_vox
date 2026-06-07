#ifndef WORLD_VISIBILITY_VALIDATOR_HPP
# define WORLD_VISIBILITY_VALIDATOR_HPP

#include "../../src/world/World.hpp"

#include "../../src/coordinates/WorldCoordinates.hpp"
#include <cmath>
#include <cstdint>

class World;

class WorldVisibilityValidator
{
    public:
        WorldVisibilityValidator();
        WorldVisibilityValidator(const WorldVisibilityValidator &other);
        ~WorldVisibilityValidator();
        WorldVisibilityValidator &operator=(const WorldVisibilityValidator &other);

        static bool validate_visible_distance(const World &world,
            double camera_x, double camera_z, double yaw,
            int32_t required_distance);
};

#endif
