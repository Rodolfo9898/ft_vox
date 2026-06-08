#ifndef GPUR_WINDOW_HPP
# define GPUR_WINDOW_HPP

class GpuWindow
{
    public:
        virtual ~GpuWindow() = default;

        virtual bool initialize(const char *title, int width, int height,
            bool fullscreen) noexcept = 0;
        virtual void destroy() noexcept = 0;
        virtual void poll_events() noexcept = 0;
        virtual void swap_buffers() noexcept = 0;
        virtual bool should_close() const noexcept = 0;
        virtual int  get_width()  const noexcept = 0;
        virtual int  get_height() const noexcept = 0;

        static GpuWindow *create() noexcept;
};

#endif
