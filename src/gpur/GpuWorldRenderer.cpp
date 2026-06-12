#include "../../src/gpur/GpuWorldRenderer.hpp"

const float GpuWorldRenderer::FOV_X = 0.83909963118f;

GpuWorldRenderer::GpuWorldRenderer()
    : _mega_vao(0), _mega_vbo(0), _mega_ebo(0), _mega_vbo_cap(0), _mega_ebo_cap(0), _water_vao(0),
      _water_ebo(0), _water_ebo_cap(0), _sky_vao(0), _crosshair_vao(0), _crosshair_vbo(0),
      _width(0), _height(0), _u_mvp(-1), _u_atlas(-1), _u_atlas_loaded(-1), _u_tile_uvs(-1),
      _u_fallback(-1), _u_sky_size(-1), _u_crosshair_color(-1)
{
    std::memset(_tile_uvs, 0, sizeof(_tile_uvs));
    std::memset(_fallback_colors, 0, sizeof(_fallback_colors));
}

GpuWorldRenderer::GpuWorldRenderer(const GpuWorldRenderer &other)
    : _mega_vao(0), _mega_vbo(0), _mega_ebo(0), _mega_vbo_cap(0), _mega_ebo_cap(0), _water_vao(0),
      _water_ebo(0), _water_ebo_cap(0), _sky_vao(0), _crosshair_vao(0), _crosshair_vbo(0),
      _width(0), _height(0), _u_mvp(-1), _u_atlas(-1), _u_atlas_loaded(-1), _u_tile_uvs(-1),
      _u_fallback(-1), _u_sky_size(-1), _u_crosshair_color(-1)
{
    (void)other;
    std::memset(_tile_uvs, 0, sizeof(_tile_uvs));
    std::memset(_fallback_colors, 0, sizeof(_fallback_colors));
}

GpuWorldRenderer::~GpuWorldRenderer()
{
    destroy();
}

GpuWorldRenderer &GpuWorldRenderer::operator=(const GpuWorldRenderer &other)
{
    (void)other;
    return *this;
}

bool GpuWorldRenderer::compile_shader_from_file(GpuShader &shader, const char *vert_path,
                                                const char *frag_path)
{
    std::string vert = GlslLoader::load(vert_path);
    std::string frag = GlslLoader::load(frag_path);
    if (vert.empty() || frag.empty())
        return false;
    return shader.compile(vert.c_str(), frag.c_str());
}

bool GpuWorldRenderer::compile_shaders(const std::string &d)
{
    if (!compile_shader_from_file(_world_shader, (d + "world.vert.glsl").c_str(),
                                  (d + "world.frag.glsl").c_str()))
        return false;
    if (!compile_shader_from_file(_sky_shader, (d + "sky.vert.glsl").c_str(),
                                  (d + "sky.frag.glsl").c_str()))
        return false;
    if (!compile_shader_from_file(_crosshair_shader, (d + "overlay.vert.glsl").c_str(),
                                  (d + "overlay.frag.glsl").c_str()))
        return false;
    return true;
}

void GpuWorldRenderer::cache_uniforms()
{
    _u_mvp = _world_shader.uniform("u_mvp");
    _u_atlas = _world_shader.uniform("u_atlas");
    _u_atlas_loaded = _world_shader.uniform("u_atlas_loaded");
    _u_tile_uvs = _world_shader.uniform("u_tile_uvs");
    _u_fallback = _world_shader.uniform("u_fallback_colors");
    _u_sky_size = _sky_shader.uniform("u_sky_size");
    _u_crosshair_color = _crosshair_shader.uniform("u_overlay_color");
}

void GpuWorldRenderer::bind_mega_vertex_attribs()
{
    constexpr GLsizei S = static_cast<GLsizei>(sizeof(MegaVertex));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, S, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, S,
                           reinterpret_cast<const void *>(static_cast<uintptr_t>(12)));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, S,
                           reinterpret_cast<const void *>(static_cast<uintptr_t>(16)));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, S,
                           reinterpret_cast<const void *>(static_cast<uintptr_t>(20)));
    glEnableVertexAttribArray(3);
}

void GpuWorldRenderer::setup_world_vao()
{
    glGenVertexArrays(1, &_mega_vao);
    glGenBuffers(1, &_mega_vbo);
    glGenBuffers(1, &_mega_ebo);
    glBindVertexArray(_mega_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
    bind_mega_vertex_attribs();
    glBindVertexArray(0);
}

void GpuWorldRenderer::setup_water_vao()
{
    glGenVertexArrays(1, &_water_vao);
    glGenBuffers(1, &_water_ebo);
    glBindVertexArray(_water_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
    bind_mega_vertex_attribs();
    glBindVertexArray(0);
}

void GpuWorldRenderer::upload_atlas_uniforms()
{
    _world_shader.use();
    glUniform1i(_u_atlas, 0);
    glUniform1i(_u_atlas_loaded, _atlas.is_loaded() ? 1 : 0);
    glUniform4fv(_u_tile_uvs, 96, _tile_uvs);
    glUniform3fv(_u_fallback, 16, _fallback_colors);
}

bool GpuWorldRenderer::initialize(int width, int height, GLuint sky_vao, GLuint crosshair_vao,
                                  GLuint crosshair_vbo, const std::string &shader_dir)
{
    destroy();
    _sky_vao = sky_vao;
    _crosshair_vao = crosshair_vao;
    _crosshair_vbo = crosshair_vbo;
    if (!compile_shaders(shader_dir))
        return false;
    cache_uniforms();
    setup_world_vao();
    setup_water_vao();
    const TextureAtlas &cpu_atlas = TextureAtlas::instance();
    _atlas.upload(cpu_atlas);
    _atlas.fill_tile_uvs(cpu_atlas, _tile_uvs);
    _atlas.fill_fallback_colors(_fallback_colors);
    upload_atlas_uniforms();
    _width = width;
    _height = height;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

// clang-format off
void GpuWorldRenderer::destroy()
{
    _atlas.destroy();
    if (_mega_vao != 0)      { glDeleteVertexArrays(1, &_mega_vao); _mega_vao = 0; }
    if (_mega_vbo != 0)      { glDeleteBuffers(1, &_mega_vbo); _mega_vbo = 0; }
    if (_mega_ebo != 0)      { glDeleteBuffers(1, &_mega_ebo); _mega_ebo = 0; }
    _mega_vbo_cap = 0; _mega_ebo_cap = 0;
    _mega_verts.clear(); _mega_idxs.clear();
    if (_water_vao != 0)     { glDeleteVertexArrays(1, &_water_vao); _water_vao = 0; }
    if (_water_ebo != 0)     { glDeleteBuffers(1, &_water_ebo); _water_ebo = 0; }
    _water_ebo_cap = 0; _water_idxs.clear();
    _world_shader.destroy();
    _sky_shader.destroy();
    _crosshair_shader.destroy();
}
// clang-format on

void GpuWorldRenderer::resize(int width, int height)
{
    _width = width;
    _height = height;
}

void GpuWorldRenderer::build_view_matrix(float view[16], const Camera &camera) const
{
    double cy = std::cos(camera.yaw), sy = std::sin(camera.yaw);
    double cp = std::cos(camera.pitch), sp = std::sin(camera.pitch);
    double tx = -camera.x, ty = -camera.y, tz = -camera.z;
    double vx = tx * cy - tz * sy, vy = ty, vz = tx * sy + tz * cy;
    double vy2 = vy * cp - vz * sp, vz2 = vy * sp + vz * cp;
    std::memset(view, 0, 16 * sizeof(float));
    // clang-format off
    view[0]  = static_cast<float>(cy);    view[8]  = static_cast<float>(-sy);   view[12] = static_cast<float>(vx);
    view[1]  = static_cast<float>(-sy*sp); view[5]  = static_cast<float>(cp);   view[9]  = static_cast<float>(-cy*sp); view[13] = static_cast<float>(vy2);
    view[2]  = static_cast<float>(sy*cp);  view[6]  = static_cast<float>(sp);   view[10] = static_cast<float>(cy*cp);  view[14] = static_cast<float>(vz2);
    view[15] = 1.0f;
    // clang-format on
}

void GpuWorldRenderer::build_proj_matrix(float proj[16], int width, int height, float near_z,
                                         float far_z) const
{
    float fx = 1.0f / FOV_X;
    float fy = fx * (static_cast<float>(width) * 0.5f) / (static_cast<float>(height) * 0.52f);
    float fn = far_z + near_z, fd = far_z - near_z;
    std::memset(proj, 0, 16 * sizeof(float));
    // clang-format off
    proj[0] = fx; proj[5] = fy; proj[10] = fn / fd; proj[11] = 1.0f; proj[14] = -2.0f * near_z * far_z / fd;
    // clang-format on
}

void GpuWorldRenderer::build_mvp(float out[16], const Camera &camera, int width, int height,
                                 float near_z, float far_z) const
{
    float view[16], proj[16];
    build_view_matrix(view, camera);
    build_proj_matrix(proj, width, height, near_z, far_z);
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
        {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k)
                s += proj[k * 4 + row] * view[col * 4 + k];
            out[col * 4 + row] = s;
        }
}

void GpuWorldRenderer::draw_sky() const
{
    _sky_shader.use();
    glUniform2f(_u_sky_size, static_cast<float>(_width), static_cast<float>(_height));
    glBindVertexArray(_sky_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void GpuWorldRenderer::draw_crosshair() const
{
    float sx = 8.0f / static_cast<float>(_width);
    float sy = 8.0f / static_cast<float>(_height);
    float verts[] = {-sx, 0.0f, sx, 0.0f, 0.0f, -sy, 0.0f, sy};
    glBindVertexArray(_crosshair_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _crosshair_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(verts)), verts, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(2 * sizeof(float)),
                          nullptr);
    glEnableVertexAttribArray(0);
    const float color[4] = {0.96f, 0.96f, 0.92f, 1.0f};
    _crosshair_shader.use();
    glUniform4fv(_u_crosshair_color, 1, color);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINES, 0, 4);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

void GpuWorldRenderer::add_chunk_vertices(const WorldChunk &wc, uint32_t base)
{
    const float ox = static_cast<float>(wc.world_x);
    const float oz = static_cast<float>(wc.world_z);
    for (const chunk_mesh_vertex &v : wc.mesh.vertices)
    {
        MegaVertex mv;
        mv.x = static_cast<float>(v.coordinate_x) + ox;
        mv.y = static_cast<float>(v.coordinate_y);
        mv.z = static_cast<float>(v.coordinate_z) + oz;
        mv.tu = v.texture_u; mv.tv = v.texture_v;
        mv.block_id = v.block_id; mv.face = static_cast<uint32_t>(v.face);
        _mega_verts.push_back(mv);
    }
    for (size_t j = 0; j < wc.mesh.indices.size(); ++j)
    {
        const uint32_t raw = wc.mesh.indices[j];
        if (wc.mesh.vertices[raw].block_id == TERRAIN_GENERATOR_WATER_BLOCK)
            _water_idxs.push_back(base + raw);
        else
            _mega_idxs.push_back(base + raw);
    }
}

void GpuWorldRenderer::collect_geometry(const Camera &camera, const World &world)
{
    _mega_verts.clear(); _mega_idxs.clear(); _water_idxs.clear();
    RenderCache cache;
    cache.configure(camera, _width, _height, world.active_render_distance);
    for (int i = 0; i < world.chunk_count; ++i)
    {
        const WorldChunk &wc = world.chunks[i];
        if (!wc.initialized || wc.mesh.vertices.empty() == FT_TRUE)
            continue;
        if (!MeshCuller::chunk_is_visible(camera, wc, cache))
            continue;
        add_chunk_vertices(wc, static_cast<uint32_t>(_mega_verts.size()));
    }
}

void GpuWorldRenderer::flush_solid(const float mvp[16])
{
    if (_mega_verts.empty())
        return;
    const GLsizeiptr vbytes = static_cast<GLsizeiptr>(_mega_verts.size() * sizeof(MegaVertex));
    const GLsizeiptr ibytes = static_cast<GLsizeiptr>(_mega_idxs.size() * sizeof(uint32_t));
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    if (static_cast<size_t>(vbytes) > _mega_vbo_cap)
        { glBufferData(GL_ARRAY_BUFFER, vbytes, nullptr, GL_DYNAMIC_DRAW); _mega_vbo_cap = static_cast<size_t>(vbytes); }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbytes, _mega_verts.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
    if (static_cast<size_t>(ibytes) > _mega_ebo_cap)
        { glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibytes, nullptr, GL_DYNAMIC_DRAW); _mega_ebo_cap = static_cast<size_t>(ibytes); }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibytes, _mega_idxs.data());
    _world_shader.use();
    glUniformMatrix4fv(_u_mvp, 1, GL_FALSE, mvp);
    _atlas.bind(0);
    glBindVertexArray(_mega_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_mega_idxs.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GpuWorldRenderer::flush_water(const float mvp[16])
{
    if (_water_idxs.empty())
        return;
    const GLsizeiptr ibytes = static_cast<GLsizeiptr>(_water_idxs.size() * sizeof(uint32_t));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
    if (static_cast<size_t>(ibytes) > _water_ebo_cap)
        { glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibytes, nullptr, GL_DYNAMIC_DRAW); _water_ebo_cap = static_cast<size_t>(ibytes); }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibytes, _water_idxs.data());
    _world_shader.use();
    glUniformMatrix4fv(_u_mvp, 1, GL_FALSE, mvp);
    _atlas.bind(0);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glDepthMask(GL_FALSE);
    glBindVertexArray(_water_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_water_idxs.size()), GL_UNSIGNED_INT,
                   nullptr);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE); glDisable(GL_BLEND);
}

void GpuWorldRenderer::render(const Camera &camera, const World &world)
{
    glViewport(0, 0, _width, _height);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_sky();
    float mvp[16];
    float far_z = static_cast<float>(world.active_render_distance) + 64.0f;
    build_mvp(mvp, camera, _width, _height, 0.08f, far_z);
    collect_geometry(camera, world);
    flush_solid(mvp);
    flush_water(mvp);
    draw_crosshair();
}

size_t GpuWorldRenderer::gpu_bytes() const
{
    size_t b = _mega_vbo_cap + _mega_ebo_cap + _water_ebo_cap;
    b += static_cast<size_t>(_atlas.is_loaded() ? 192 * 192 * 4 : 0);
    return b;
}
