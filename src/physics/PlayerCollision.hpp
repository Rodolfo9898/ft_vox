#ifndef PLAYER_COLLISION_HPP
# define PLAYER_COLLISION_HPP

#include "../../src/physics/PlayerGround.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include <cmath>
#include <cstdint>

class PlayerCollision
{
    public:
        PlayerCollision();
        PlayerCollision(const PlayerCollision &other);
        ~PlayerCollision();
        PlayerCollision &operator=(const PlayerCollision &other);

        static bool player_box_intersects_world(const World &world, double x,
            double eye_y, double z);
        static bool move_player_axis(Camera *camera, const World &world,
            int axis, double delta);
};

#endif
