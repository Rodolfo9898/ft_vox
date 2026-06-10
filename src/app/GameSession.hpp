#ifndef GAME_SESSION_HPP
# define GAME_SESSION_HPP

#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../src/debug/RenderDebug.hpp"
#include "../../src/platform/InputReader.hpp"
#include "../../src/platform/ApplicationWindow.hpp"
#include "../../src/render/VoxelRenderer.hpp"
#include "../../src/player/PlayerController.hpp"
#include "../../src/physics/PlayerCollision.hpp"
#include "../../src/interaction/BlockInteractor.hpp"
#include "../../src/policy/RenderDistanceStrategy.hpp"
#include "../../src/settings/Settings.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/DUMB/controls.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstring>

#if defined(__APPLE__)
# include <mach/task.h>
# include <mach/mach.h>
#endif

class GameSession
{
    public:
        enum class Action { CONTINUE, OPEN_SETTINGS, EXIT_TO_MENU, ERROR };

    Camera      camera_;
    World       world_;
    RenderDebug render_debug_;

    int32_t  active_render_distance_;
    double   fps_accumulator_;
    double   rd_accumulator_;
    double   display_fps_;
    double   frame_ms_;
    uint64_t fps_frame_count_;
    uint32_t selected_block_id_;
    bool     boost_enabled_;
    bool     active_;
    int32_t  error_code_;
    char     seed_[32];

    static uint32_t system_ram_mb();

    public:
        GameSession();
        GameSession(const GameSession &other);
        ~GameSession();
        GameSession &operator=(const GameSession &other);

        int  start(const std::string &seed,
            ApplicationWindow &window, VoxelRenderer &renderer);
        void stop();
        bool is_active()           const;
        bool is_ready_to_play()    const;
        int  error_code()          const;

        int         loading_tick(const RenderDistanceStrategy &strategy);
        Action     update(double delta_seconds,
            const RenderDistanceStrategy &strategy,
            ApplicationWindow &window);
        void        render(ApplicationWindow &window,
            VoxelRenderer &renderer, bool with_overlay);
};

#endif
