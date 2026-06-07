#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../src/config/ApplicationOptions.hpp"
#include "../../src/platform/ApplicationWindow.hpp"
#include "../../src/validators/ApplicationValidator.hpp"
#include "../../src/interaction/BlockInteractor.hpp"
#include "../../src/config/CommandLine.hpp"
#include "../../src/diagnostics/DebugCrashHandler.hpp"
#include "../../src/diagnostics/FramebufferHasher.hpp"
#include "../../src/platform/InputReader.hpp"
#include "../../src/player/PlayerController.hpp"
#include "../../src/policy/RenderPolicy.hpp"
#include "../../src/policy/RenderStrategyFactory.hpp"
#include "../../Libft/Modules/DUMB/controls.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/camera/CameraInput.hpp"
#include "../../src/debug/RenderDebug.hpp"
#include "../../src/render/VoxelRenderer.hpp"
#include "../../src/world/World.hpp"
#include <chrono>
#include <cstdio>
#include <cstdint>

class Application
{
    public:
        Application();
        Application(const Application &other);
        ~Application();
        Application &operator=(const Application &other);

        static int run(int argc, char **argv);
};

#endif
