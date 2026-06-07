#ifndef WORLD_EDIT_COMMAND_HPP
# define WORLD_EDIT_COMMAND_HPP

#include <cstdint>

class World;

class WorldEditCommand
{
    public:
        WorldEditCommand();
        WorldEditCommand(const WorldEditCommand &other);
        virtual ~WorldEditCommand();
        WorldEditCommand &operator=(const WorldEditCommand &other);

        virtual int32_t execute(World &world) const = 0;
};

#endif
