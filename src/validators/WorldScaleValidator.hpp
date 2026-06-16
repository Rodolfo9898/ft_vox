#ifndef WORLD_SCALE_VALIDATOR_HPP
# define WORLD_SCALE_VALIDATOR_HPP

# include "../../src/validators/IValidator.hpp"
# include "../../src/chunks/WorldChunkLoader.hpp"
# include "../../Libft/Modules/Errno/errno.hpp"
# include "../../Libft/Modules/Game/game_voxel_chunk.hpp"
# include <cstdint>
# include <cstdio>

class WorldScaleValidator : public IValidator
{
    private:
        static int validate_chunk(int32_t chunk_x, int32_t chunk_z);

    public:
        WorldScaleValidator();
        WorldScaleValidator(const WorldScaleValidator &other);
        ~WorldScaleValidator();
        WorldScaleValidator &operator=(const WorldScaleValidator &other);

        virtual int validate() const override;
};

#endif
