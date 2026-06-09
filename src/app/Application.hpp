#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../src/config/ApplicationOptions.hpp"
#include "../../src/config/LaunchSettings.hpp"
#include "../../src/config/LaunchSettingsMenu.hpp"
#include "../../src/platform/ApplicationWindow.hpp"
#include "../../src/validators/ApplicationValidator.hpp"
#include "../../src/config/CommandLine.hpp"
#include "../../src/diagnostics/DebugCrashHandler.hpp"
#include "../../src/policy/RenderPolicy.hpp"
#include "../../src/policy/RenderStrategyFactory.hpp"
#include "../../src/render/VoxelRenderer.hpp"
#include "../../src/settings/Settings.hpp"
#include "../../src/menu/MenuController.hpp"
#include "../../src/menu/MenuCanvas.hpp"
#include "../../src/app/GameSession.hpp"
#include "../../src/app/PerfSession.hpp"
#include "../../Libft/Modules/DUMB/controls.hpp"
#include <chrono>
#include <cstdio>
#include <cstdint>

#if defined(_WIN32)
# include <windows.h>
#endif
#if defined(__APPLE__)
# include <Carbon/Carbon.h>
#endif
#include <cstdlib>
#include <cstring>

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
