#ifndef COLLISION_VALIDATOR_HPP
# define COLLISION_VALIDATOR_HPP

#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../src/platform/InputReader.hpp"
#include "../../src/player/PlayerController.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include <cmath>
#include <cstdio>
#include <cstdint>

class CollisionValidator
{
    public:
        CollisionValidator();
        CollisionValidator(const CollisionValidator &other);
        ~CollisionValidator();
        CollisionValidator &operator=(const CollisionValidator &other);

        static int validate();
};

#endif
