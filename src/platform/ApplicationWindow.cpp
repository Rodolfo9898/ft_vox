#include "../../src/platform/ApplicationWindow.hpp"

ApplicationWindow::ApplicationWindow()
{
    this->active_framebuffer.width  = 0;
    this->active_framebuffer.height = 0;
    this->active_framebuffer.pixels = nullptr;
    this->gpu_window = nullptr;
    this->headless   = false;
    this->opened     = false;
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
        this->headless_pixels    = other.headless_pixels;
        this->gpu_window         = nullptr;
        this->headless           = other.headless;
        this->opened             = false;
        if (this->headless_pixels.empty() == false)
            this->active_framebuffer.pixels = this->headless_pixels.data();
    }
    return (*this);
}

int ApplicationWindow::initialize(bool headless_mode)
{
    ft_render_screen_size screen_size;

    this->headless = headless_mode;
    if (this->headless == true)
    {
        this->active_framebuffer.width  = 1280;
        this->active_framebuffer.height = 720;
        this->headless_pixels.resize(
            static_cast<size_t>(this->active_framebuffer.width)
            * static_cast<size_t>(this->active_framebuffer.height));
        this->active_framebuffer.pixels = this->headless_pixels.data();
        return (0);
    }

    screen_size = ft_render_get_primary_screen_size();
    if (screen_size.width <= 0 || screen_size.height <= 0)
    {
        screen_size.width  = 1280;
        screen_size.height = 720;
    }

    this->gpu_window = GpuWindow::create();
    if (this->gpu_window != nullptr)
    {
        if (this->gpu_window->initialize("Voxel World",
                screen_size.width, screen_size.height, true))
        {
            this->active_framebuffer.width  = this->gpu_window->get_width();
            this->active_framebuffer.height = this->gpu_window->get_height();
            this->active_framebuffer.pixels = nullptr;
            this->opened = true;
            return (0);
        }
        delete this->gpu_window;
        this->gpu_window = nullptr;
    }

    ft_render_window_desc description;
    int32_t error_code;

    error_code = this->sw_window.initialize();
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("window object initialization",
            error_code));
    description.width  = screen_size.width;
    description.height = screen_size.height;
    description.title  = "Voxel World";
    description.flags  = FT_RENDER_WINDOW_FLAG_FULLSCREEN;
    error_code = this->sw_window.initialize(description);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)this->sw_window.destroy();
        return (ApplicationError::fail("window creation", error_code));
    }
    error_code = this->sw_window.set_fullscreen(FT_TRUE);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)this->sw_window.destroy();
        return (ApplicationError::fail("fullscreen activation", error_code));
    }
    this->active_framebuffer = this->sw_window.framebuffer();
    this->opened = true;
    return (0);
}

int ApplicationWindow::poll_events()
{
    if (this->headless == true)
        return (FT_ERR_SUCCESS);
    if (this->gpu_window != nullptr)
    {
        this->gpu_window->poll_events();
        return (FT_ERR_SUCCESS);
    }
    return (this->sw_window.poll_events());
}

int ApplicationWindow::present()
{
    if (this->headless == true)
        return (FT_ERR_SUCCESS);
    if (this->gpu_window != nullptr)
    {
        this->gpu_window->swap_buffers();
        return (FT_ERR_SUCCESS);
    }
    return (this->sw_window.present());
}

int ApplicationWindow::destroy()
{
    if (this->opened == false)
        return (FT_ERR_SUCCESS);
    this->opened = false;
    if (this->gpu_window != nullptr)
    {
        this->gpu_window->destroy();
        delete this->gpu_window;
        this->gpu_window = nullptr;
        return (FT_ERR_SUCCESS);
    }
    return (this->sw_window.destroy());
}

bool ApplicationWindow::should_close() const
{
    if (this->headless == true)
        return (false);
    if (this->gpu_window != nullptr)
        return (this->gpu_window->should_close());
    return (this->sw_window.should_close() == FT_TRUE);
}

bool ApplicationWindow::is_headless() const
{
    return (this->headless);
}

bool ApplicationWindow::is_gpu_mode() const
{
    return (this->gpu_window != nullptr);
}

GpuWindow *ApplicationWindow::get_gpu_window() const
{
    return (this->gpu_window);
}

ft_render_framebuffer &ApplicationWindow::framebuffer()
{
    return (this->active_framebuffer);
}
