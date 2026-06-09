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

int ApplicationWindow::initialize(bool headless_mode, int width, int height,
    bool fullscreen, bool allow_gpu)
{
    this->headless = headless_mode;
    if (width <= 0)
        width = 1280;
    if (height <= 0)
        height = 720;
    if (this->headless == true)
    {
        this->active_framebuffer.width  = width;
        this->active_framebuffer.height = height;
        this->headless_pixels.resize(
            static_cast<size_t>(this->active_framebuffer.width)
            * static_cast<size_t>(this->active_framebuffer.height));
        this->active_framebuffer.pixels = this->headless_pixels.data();
        return (0);
    }

    if (allow_gpu == true)
    {
        this->gpu_window = GpuWindow::create();
        if (this->gpu_window != nullptr)
        {
            if (this->gpu_window->initialize("Voxel World", width, height,
                    fullscreen))
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
    }

    ft_render_window_desc description;
    int32_t error_code;

    error_code = this->sw_window.initialize();
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("window object initialization",
            error_code));
    description.width  = width;
    description.height = height;
    description.title  = "Voxel World";
    description.flags  = fullscreen == true
        ? FT_RENDER_WINDOW_FLAG_FULLSCREEN : FT_RENDER_WINDOW_FLAG_NONE;
    error_code = this->sw_window.initialize(description);
    if (error_code != FT_ERR_SUCCESS)
    {
        (void)this->sw_window.destroy();
        return (ApplicationError::fail("window creation", error_code));
    }
    if (fullscreen == true)
    {
        error_code = this->sw_window.set_fullscreen(FT_TRUE);
        if (error_code != FT_ERR_SUCCESS)
        {
            (void)this->sw_window.destroy();
            return (ApplicationError::fail("fullscreen activation", error_code));
        }
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

int ApplicationWindow::mouse_x() const
{
    if (this->gpu_window != nullptr)
        return (this->gpu_window->get_mouse_x());
    return (0);
}

int ApplicationWindow::mouse_y() const
{
    if (this->gpu_window != nullptr)
        return (this->gpu_window->get_mouse_y());
    return (0);
}

bool ApplicationWindow::was_mouse_clicked() const
{
    if (this->gpu_window != nullptr)
        return (this->gpu_window->was_mouse_clicked());
    return (false);
}

void ApplicationWindow::set_cursor_visible(bool visible)
{
    if (this->gpu_window != nullptr)
        this->gpu_window->set_cursor_visible(visible);
}

bool ApplicationWindow::was_settings_key_pressed() const
{
    if (this->gpu_window != nullptr)
        return (this->gpu_window->was_settings_key_pressed());
    return (false);
}
