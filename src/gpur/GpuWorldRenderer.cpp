#include "../../src/gpur/GpuWorldRenderer.hpp"
#include "../../src/assets/TextureAtlas.hpp"

static const char *WORLD_VERT = R"GLSL(
#version 330 core
layout(location = 0) in vec3  a_position;
layout(location = 1) in uvec2 a_texcoord;
layout(location = 2) in uint  a_block_id;
layout(location = 3) in uint  a_face;
uniform mat4 u_mvp;
out vec2 v_uv_tile;
out float v_shade;
flat out uint v_block_id;
flat out uint v_face;
const float SHADE[6] = float[6](0.72, 0.82, 0.72, 1.0, 0.58, 0.72);
void main()
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_uv_tile   = vec2(a_texcoord);
    v_shade     = a_face < 6u ? SHADE[a_face] : 0.72;
    v_block_id  = a_block_id;
    v_face      = a_face;
}
)GLSL";

static const char *WORLD_FRAG = R"GLSL(
#version 330 core
in vec2 v_uv_tile;
in float v_shade;
flat in uint v_block_id;
flat in uint v_face;
uniform sampler2D u_atlas;
uniform int u_atlas_loaded;
uniform vec4 u_tile_uvs[96];
uniform vec3 u_fallback_colors[16];
out vec4 frag_color;
void main()
{
    uint bid = min(v_block_id, 15u);
    uint fid = min(v_face,     5u);
    if (bid == 8u) { frag_color = vec4(vec3(0.1, 0.45, 0.85) * v_shade, 0.6); return; }
    if (u_atlas_loaded == 1)
    {
        vec4 region = u_tile_uvs[bid * 6u + fid];
        vec2 uv     = region.xy + fract(v_uv_tile) * region.zw;
        frag_color  = vec4(texture(u_atlas, uv).rgb * v_shade, 1.0);
    }
    else
        frag_color = vec4(u_fallback_colors[bid] * v_shade, 1.0);
}
)GLSL";

GpuWorldRenderer::GpuWorldRenderer()
    : _mega_vao(0), _mega_vbo(0), _mega_ebo(0),
      _mega_vbo_cap(0), _mega_ebo_cap(0),
      _water_vao(0), _water_ebo(0), _water_ebo_cap(0),
      _u_mvp(-1), _u_atlas(-1), _u_atlas_loaded(-1),
      _u_tile_uvs(-1), _u_fallback(-1)
{
    std::memset(_tile_uvs,       0, sizeof(_tile_uvs));
    std::memset(_fallback_colors, 0, sizeof(_fallback_colors));
}

GpuWorldRenderer::GpuWorldRenderer(const GpuWorldRenderer &other)
{ (void)other; }

GpuWorldRenderer::~GpuWorldRenderer() { destroy(); }

GpuWorldRenderer &GpuWorldRenderer::operator=(const GpuWorldRenderer &other)
{ (void)other; return *this; }

void GpuWorldRenderer::setup_vao(GLuint vao, GLuint vbo, GLuint ebo) noexcept
{
    constexpr GLsizei S = static_cast<GLsizei>(sizeof(MegaVertex));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
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
    glBindVertexArray(0);
}

bool GpuWorldRenderer::initialize() noexcept
{
    if (!_world_shader.compile(WORLD_VERT, WORLD_FRAG)) return false;
    _u_mvp          = _world_shader.uniform("u_mvp");
    _u_atlas        = _world_shader.uniform("u_atlas");
    _u_atlas_loaded = _world_shader.uniform("u_atlas_loaded");
    _u_tile_uvs     = _world_shader.uniform("u_tile_uvs");
    _u_fallback     = _world_shader.uniform("u_fallback_colors");

    glGenVertexArrays(1, &_mega_vao);
    glGenBuffers(1, &_mega_vbo);
    glGenBuffers(1, &_mega_ebo);
    setup_vao(_mega_vao, _mega_vbo, _mega_ebo);

    glGenVertexArrays(1, &_water_vao);
    glGenBuffers(1, &_water_ebo);
    setup_vao(_water_vao, _mega_vbo, _water_ebo);

    const TextureAtlas &cpu = TextureAtlas::instance();
    _atlas.upload(cpu);
    _atlas.fill_tile_uvs(cpu, _tile_uvs);
    _atlas.fill_fallback_colors(_fallback_colors);

    _world_shader.use();
    glUniform1i(_u_atlas, 0);
    glUniform1i(_u_atlas_loaded, _atlas.is_loaded() ? 1 : 0);
    glUniform4fv(_u_tile_uvs, 96, _tile_uvs);
    glUniform3fv(_u_fallback, 16, _fallback_colors);
    return true;
}

void GpuWorldRenderer::destroy() noexcept
{
    _atlas.destroy();
    if (_mega_vao != 0) { glDeleteVertexArrays(1, &_mega_vao); _mega_vao = 0; }
    if (_mega_vbo != 0) { glDeleteBuffers(1, &_mega_vbo);      _mega_vbo = 0; }
    if (_mega_ebo != 0) { glDeleteBuffers(1, &_mega_ebo);      _mega_ebo = 0; }
    _mega_vbo_cap = 0; _mega_ebo_cap = 0;
    _mega_verts.clear(); _mega_idxs.clear();
    if (_water_vao != 0) { glDeleteVertexArrays(1, &_water_vao); _water_vao = 0; }
    if (_water_ebo != 0) { glDeleteBuffers(1, &_water_ebo);      _water_ebo = 0; }
    _water_ebo_cap = 0; _water_idxs.clear();
    _world_shader.destroy();
}

void GpuWorldRenderer::build_buffers(const Camera &camera,
    const World &world) noexcept
{
    _mega_verts.clear();
    _mega_idxs.clear();
    _water_idxs.clear();

    RenderCache cache;
    cache.configure(camera, 0, 0, world.active_render_distance);

    for (int i = 0; i < world.chunk_count; ++i)
    {
        const WorldChunk &wc = world.chunks[i];
        if (!wc.initialized || wc.mesh.vertices.empty() == FT_TRUE) continue;
        if (!MeshCuller::chunk_is_visible(camera, wc, cache))       continue;

        const uint32_t base = static_cast<uint32_t>(_mega_verts.size());
        const float ox = static_cast<float>(wc.world_x);
        const float oz = static_cast<float>(wc.world_z);

        for (size_t j = 0; j < wc.mesh.vertices.size(); ++j)
        {
            const chunk_mesh_vertex &v = wc.mesh.vertices[j];
            MegaVertex mv;
            mv.x        = static_cast<float>(v.coordinate_x) + ox;
            mv.y        = static_cast<float>(v.coordinate_y);
            mv.z        = static_cast<float>(v.coordinate_z) + oz;
            mv.tu       = v.texture_u;
            mv.tv       = v.texture_v;
            mv.block_id = v.block_id;
            mv.face     = static_cast<uint32_t>(v.face);
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
}

void GpuWorldRenderer::upload_and_draw_opaque() noexcept
{
    if (_mega_idxs.empty()) return;
    const GLsizeiptr vb = static_cast<GLsizeiptr>(_mega_verts.size() * sizeof(MegaVertex));
    const GLsizeiptr ib = static_cast<GLsizeiptr>(_mega_idxs.size()  * sizeof(uint32_t));
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    if (static_cast<size_t>(vb) > _mega_vbo_cap)
    { glBufferData(GL_ARRAY_BUFFER, vb, nullptr, GL_DYNAMIC_DRAW); _mega_vbo_cap = static_cast<size_t>(vb); }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vb, _mega_verts.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
    if (static_cast<size_t>(ib) > _mega_ebo_cap)
    { glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib, nullptr, GL_DYNAMIC_DRAW); _mega_ebo_cap = static_cast<size_t>(ib); }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ib, _mega_idxs.data());
    glBindVertexArray(_mega_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_mega_idxs.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GpuWorldRenderer::upload_and_draw_water() noexcept
{
    if (_water_idxs.empty()) return;
    const GLsizeiptr ib = static_cast<GLsizeiptr>(_water_idxs.size() * sizeof(uint32_t));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
    if (static_cast<size_t>(ib) > _water_ebo_cap)
    { glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib, nullptr, GL_DYNAMIC_DRAW); _water_ebo_cap = static_cast<size_t>(ib); }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ib, _water_idxs.data());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glBindVertexArray(_water_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_water_idxs.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void GpuWorldRenderer::render(const Camera &camera,
    const World &world, const float mvp[16]) noexcept
{
    if (_mega_verts.capacity() == 0 && _mega_idxs.capacity() == 0) {}
    build_buffers(camera, world);
    if (_mega_verts.empty()) return;
    _world_shader.use();
    glUniformMatrix4fv(_u_mvp, 1, GL_FALSE, mvp);
    _atlas.bind(0);
    upload_and_draw_opaque();
    _world_shader.use();
    glUniformMatrix4fv(_u_mvp, 1, GL_FALSE, mvp);
    _atlas.bind(0);
    upload_and_draw_water();
}

size_t GpuWorldRenderer::gpu_bytes() const noexcept
{
    size_t b = _mega_vbo_cap + _mega_ebo_cap + _water_ebo_cap;
    b += static_cast<size_t>(_atlas.is_loaded() ? 192 * 192 * 4 : 0);
    return b;
}
