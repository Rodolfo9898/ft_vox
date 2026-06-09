#ifndef PERF_SESSION_HPP
# define PERF_SESSION_HPP

#include "../../src/config/ApplicationOptions.hpp"
#include "../../src/config/LaunchSettings.hpp"
#include "../../src/platform/ApplicationWindow.hpp"
#include "../../src/render/VoxelRenderer.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../src/player/PlayerController.hpp"
#include "../../src/interaction/BlockInteractor.hpp"
#include "../../src/platform/InputReader.hpp"
#include "../../src/diagnostics/FramebufferHasher.hpp"
#include "../../src/policy/RenderDistanceStrategy.hpp"
#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/DUMB/controls.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <algorithm>

class PerfSession
{
    public:
        PerfSession();
        PerfSession(const PerfSession &other);
        ~PerfSession();
        PerfSession &operator=(const PerfSession &other);

        int run(const ApplicationOptions &options,
            const LaunchSettings &launch_settings,
            const RenderDistanceStrategy &strategy);
};

#endif
