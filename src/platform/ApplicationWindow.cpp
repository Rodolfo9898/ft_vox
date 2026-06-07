#include "../../src/platform/ApplicationWindow.hpp"

ApplicationWindow::ApplicationWindow()
{
    this->active_framebuffer.width = 0;
    this->active_framebuffer.height = 0;
    this->active_framebuffer.pixels = nullptr;
    this->headless = false;
    this->opened = false;
}

ApplicationWindow::ApplicationWindow(const ApplicationWindow &other)
{
    *this = other;
}

ApplicationWindow::~ApplicationWindow()
{
    (void)this->destroy();
}

ApplicationWindow &ApplicationWindow::operator=(const ApplicationWindow &other)
{
    if (this != &other)
    {
        this->active_framebuffer = other.active_framebuffer;
        this->headless_pixels = other.headless_pixels;
        this->headless = other.headless;
        this->opened = false;
        if (this->headless_pixels.empty() == false)
            this->active_framebuffer.pixels = this->headless_pixels.data();
    }
    return (*this);
}

int ApplicationWindow::initialize(bool headless_mode)
{
    ft_render_window_desc description;
    ft_render_screen_size screen_size;
    int32_t error_code;

    this->headless = headless_mode;
    if (this->headless == true)
    {
        this->active_framebuffer.width = 1280;
        this->active_framebuffer.height = 720;
        this->headless_pixels.resize(static_cast<size_t>(
            this->active_framebuffer.width)
            * static_cast<size_t>(this->active_framebuffer.height));
        this->active_framebuffer.pixels = this->headless_pixels.data();
        return (0);
    }
    error_code = this->window.initialize();
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("window object initialization",
            error_code));
    screen_size = ft_render_get_primary_screen_size();
    if (screen_size.width <= 0 || screen_size.height <= 0)
    {
        screen_size.width = 1280;
        screen_size.height = 720;
    }
    description.width = screen_size.width;
    description.height = screen_size.height;
    description.title = "Voxel World";
    description.flags = FT_RENDER_WINDOW_FLAG_FULLSCREEN;
    error_code = this->window.initialize(description);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)this->window.destroy();
        return (ApplicationError::fail("window creation", error_code));
    }
    error_code = this->window.set_fullscreen(FT_TRUE);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)this->window.destroy();
        return (ApplicationError::fail("fullscreen activation", error_code));
    }
    this->active_framebuffer = this->window.framebuffer();
    this->opened = true;
    return (0);
}

int ApplicationWindow::poll_events()
{
    if (this->headless == true)
        return (FT_ERR_SUCCESS);
    return (this->window.poll_events());
}

int ApplicationWindow::present()
{
    if (this->headless == true)
        return (FT_ERR_SUCCESS);
    return (this->window.present());
}

int ApplicationWindow::destroy()
{
    if (this->opened == false)
        return (FT_ERR_SUCCESS);
    this->opened = false;
    return (this->window.destroy());
}

bool ApplicationWindow::should_close() const
{
    if (this->headless == true)
        return (false);
    return (this->window.should_close() == FT_TRUE);
}

bool ApplicationWindow::is_headless() const
{
    return (this->headless);
}

ft_render_framebuffer &ApplicationWindow::framebuffer()
{
    return (this->active_framebuffer);
}
