#include "../../src/gpur/GpuRenderer.hpp"

GpuRenderer::GpuRenderer() noexcept : _ready(false), _width(0), _height(0) {}

GpuRenderer::GpuRenderer(const GpuRenderer &other)
{ (void)other; _ready = false; _width = 0; _height = 0; }

GpuRenderer::~GpuRenderer() noexcept { destroy(); }

GpuRenderer &GpuRenderer::operator=(const GpuRenderer &other)
{ (void)other; return *this; }

bool GpuRenderer::initialize(int width, int height) noexcept
{
    destroy();
    if (!_sky.initialize())   return false;
    if (!_world.initialize()) return false;
    if (!_hud.initialize())   return false;
    _width  = width;
    _height = height;
    _ready  = true;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

void GpuRenderer::destroy() noexcept
{
    _sky.destroy();
    _world.destroy();
    _hud.destroy();
    _ready = false;
}

void GpuRenderer::resize(int width, int height) noexcept
{
    _width  = width;
    _height = height;
    glViewport(0, 0, width, height);
}

void GpuRenderer::render(const Camera &camera,
    const World &world) noexcept
{
    if (!_ready) return;
    glViewport(0, 0, _width, _height);
    glClear(GL_DEPTH_BUFFER_BIT);
    _sky.draw_sky(_width, _height);
    float mvp[16];
    GpuMvpBuilder::build(mvp, camera, _width, _height, 0.08f,
        static_cast<float>(world.active_render_distance) + 64.0f);
    _world.render(camera, world, mvp);
    _hud.draw_crosshair(_width, _height);
}

void GpuRenderer::upload_overlay(const uint32_t *pixels,
    int w, int h) noexcept
{
    _hud.upload_overlay(pixels, w, h);
}

void GpuRenderer::draw_overlay() noexcept
{
    _hud.draw_overlay(_width, _height);
}

void GpuRenderer::draw_tint(float r, float g, float b, float a) noexcept
{
    if (!_ready) return;
    _sky.draw_tint(r, g, b, a);
}

void GpuRenderer::clear_screen() noexcept
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GpuRenderer::upload_menu(const uint32_t *pixels,
    int w, int h) noexcept
{
    _hud.upload_menu(pixels, w, h);
}

void GpuRenderer::draw_menu(float alpha) noexcept
{
    (void)alpha;
    _hud.draw_menu();
}

bool GpuRenderer::is_ready() const noexcept { return _ready; }

uint32_t GpuRenderer::gpu_mb_approx() const noexcept
{
    size_t b = _world.gpu_bytes() + _hud.gpu_bytes();
    return static_cast<uint32_t>(b / (1024UL * 1024UL));
}
