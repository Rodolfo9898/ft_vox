#include "../../src/edits/WorldEditCommand.hpp"

WorldEditCommand::WorldEditCommand() {}
WorldEditCommand::WorldEditCommand(const WorldEditCommand &other) { (void)other; }
WorldEditCommand::~WorldEditCommand() {}
WorldEditCommand &WorldEditCommand::operator=(const WorldEditCommand &other)
{ (void)other; return *this; }
