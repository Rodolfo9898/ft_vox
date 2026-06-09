#ifndef PLAYER_MOVEMENT_HPP
# define PLAYER_MOVEMENT_HPP

#ifndef GAME_USE_VOXEL_REGION_BACKEND
# define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../src/physics/PlayerCollision.hpp"
#include "../../src/camera/CameraInput.hpp"
#include <cmath>

class PlayerMovement
{
    private:
        static void camera_horizontal_delta(const Camera &camera,
            const CameraInput &input, double delta_seconds,
            double speed_scale, double *delta_x, double *delta_z);
        static bool player_in_water(const Camera &camera, const World &world);

    public:
        PlayerMovement();
        PlayerMovement(const PlayerMovement &other);
        ~PlayerMovement();
        PlayerMovement &operator=(const PlayerMovement &other);

        static void update_player_horizontal_motion(Camera *camera,
            const CameraInput &input, const World &world, double delta_seconds);
        static void update_player_vertical_motion(Camera *camera,
            const CameraInput &input, const World &world, double delta_seconds);
};

#endif
