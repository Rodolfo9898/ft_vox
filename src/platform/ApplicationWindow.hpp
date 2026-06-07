#ifndef APPLICATION_WINDOW_HPP
# define APPLICATION_WINDOW_HPP

#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../Libft/Modules/DUMB/render_window.hpp"
#include <cstdint>
#include <vector>

class ApplicationWindow
{
    private:
        ft_render_window window;
        ft_render_framebuffer active_framebuffer;
        std::vector<uint32_t> headless_pixels;
        bool headless;
        bool opened;

    public:
        ApplicationWindow();
        ApplicationWindow(const ApplicationWindow &other);
        ~ApplicationWindow();
        ApplicationWindow &operator=(const ApplicationWindow &other);

        int initialize(bool headless_mode);
        int poll_events();
        int present();
        int destroy();
        bool should_close() const;
        bool is_headless() const;
        ft_render_framebuffer &framebuffer();
};

#endif
