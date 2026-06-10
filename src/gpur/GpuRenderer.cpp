#include "../../src/gpur/GpuRenderer.hpp"

static const char *WORLD_VERT = R"GLSL(
#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in uvec2 a_texcoord;
layout(location = 2) in uint a_block_id;
layout(location = 3) in uint a_face;

uniform mat4 u_mvp;

out vec2 v_uv_tile;
out float v_shade;
flat out uint v_block_id;
flat out uint v_face;

const float SHADE[6] = float[6](0.72, 0.82, 0.72, 1.0, 0.58, 0.72);

void main()
{
 gl_Position = u_mvp * vec4(a_position, 1.0);
 v_uv_tile = vec2(a_texcoord);
 v_shade = a_face < 6u ? SHADE[a_face] : 0.72;
 v_block_id = a_block_id;
 v_face = a_face;
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
 uint fid = min(v_face, 5u);
 if (bid == 8u)
 {
 frag_color = vec4(vec3(0.1, 0.45, 0.85) * v_shade, 0.6);
 return;
 }
 if (u_atlas_loaded == 1)
 {
 vec4 region = u_tile_uvs[bid * 6u + fid];
 vec2 uv = region.xy + fract(v_uv_tile) * region.zw;
 vec3 color = texture(u_atlas, uv).rgb;
 frag_color = vec4(color * v_shade, 1.0);
 }
 else
 {
 vec3 base = u_fallback_colors[bid];
 frag_color = vec4(base * v_shade, 1.0);
 }
}
)GLSL";

static const char *SKY_VERT = R"GLSL(
#version 330 core
void main()
{
 const vec2 QUAD[4] = vec2[4](
 vec2(-1.0, -1.0), vec2(1.0, -1.0),
 vec2(-1.0, 1.0), vec2(1.0, 1.0));
 gl_Position = vec4(QUAD[gl_VertexID], 0.9999, 1.0);
}
)GLSL";

static const char *SKY_FRAG = R"GLSL(
#version 330 core
uniform vec2 u_sky_size;
out vec4 frag_color;
void main()
{
 float t = gl_FragCoord.y / u_sky_size.y;
 vec3 bot = vec3(0.322, 0.604, 0.878);
 vec3 top = vec3(0.384, 0.776, 0.965);
 frag_color = vec4(mix(bot, top, t), 1.0);
}
)GLSL";

static const char *OVERLAY_VERT = R"GLSL(
#version 330 core
layout(location = 0) in vec2 a_pos;
void main() { gl_Position = vec4(a_pos, 0.0, 1.0); }
)GLSL";

static const char *OVERLAY_FRAG = R"GLSL(
#version 330 core
uniform vec4 u_overlay_color;
out vec4 frag_color;
void main() { frag_color = u_overlay_color; }
)GLSL";

static const char *OVERLAY_TEX_VERT = R"GLSL(
#version 330 core
uniform vec2 u_overlay_ndc_br;
out vec2 v_uv;
void main()
{
 bool is_right = (gl_VertexID == 1 || gl_VertexID == 3);
 bool is_top = (gl_VertexID == 2 || gl_VertexID == 3);
 gl_Position = vec4(
 is_right ? u_overlay_ndc_br.x : -1.0,
 is_top ? 1.0 : u_overlay_ndc_br.y,
 0.0, 1.0);
 v_uv = vec2(is_right ? 1.0 : 0.0, is_top ? 0.0 : 1.0);
}
)GLSL";

static const char *OVERLAY_TEX_FRAG = R"GLSL(
#version 330 core
in vec2 v_uv;
uniform sampler2D u_overlay_sampler;
out vec4 frag_color;
void main()
{
 frag_color = texture(u_overlay_sampler, v_uv);
}
)GLSL";

static const char *TINT_FRAG = R"GLSL(
#version 330 core
uniform vec4 u_tint_color;
out vec4 frag_color;
void main() { frag_color = u_tint_color; }
)GLSL";

GpuRenderer::GpuRenderer() 
 : _mega_vao(0), _mega_vbo(0), _mega_ebo(0),
 _mega_vbo_cap(0), _mega_ebo_cap(0),
 _water_vao(0), _water_ebo(0), _water_ebo_cap(0),
 _menu_tex(0), _menu_tex_w(0), _menu_tex_h(0),
 _sky_vao(0), _crosshair_vbo(0), _crosshair_vao(0),
 _overlay_tex(0), _overlay_w(0), _overlay_h(0),
 _ready(false), _width(0), _height(0),
 _u_mvp(-1), _u_atlas(-1), _u_atlas_loaded(-1),
 _u_tile_uvs(-1), _u_fallback(-1), _u_sky_size(-1), _u_overlay_color(-1),
 _u_overlay_sampler(-1), _u_overlay_ndc_br(-1), _u_tint_color(-1)
{
 std::memset(_tile_uvs, 0, sizeof(_tile_uvs));
 std::memset(_fallback_colors, 0, sizeof(_fallback_colors));
}

GpuRenderer::GpuRenderer(const GpuRenderer &other) { (void)other; }

GpuRenderer::~GpuRenderer() { destroy(); }

GpuRenderer &GpuRenderer::operator=(const GpuRenderer &other) { (void)other; return *this; }

bool GpuRenderer::initialize(int width, int height) 
{
 destroy();

 if (!_world_shader.compile(WORLD_VERT, WORLD_FRAG)) return (false);
 if (!_sky_shader.compile(SKY_VERT, SKY_FRAG)) return (false);
 if (!_overlay_shader.compile(OVERLAY_VERT, OVERLAY_FRAG)) return (false);
 if (!_overlay_tex_shader.compile(OVERLAY_TEX_VERT, OVERLAY_TEX_FRAG)) return (false);
 if (!_tint_shader.compile(SKY_VERT, TINT_FRAG)) return (false);

 _u_mvp = _world_shader.uniform("u_mvp");
 _u_atlas = _world_shader.uniform("u_atlas");
 _u_atlas_loaded = _world_shader.uniform("u_atlas_loaded");
 _u_tile_uvs = _world_shader.uniform("u_tile_uvs");
 _u_fallback = _world_shader.uniform("u_fallback_colors");
 _u_sky_size = _sky_shader.uniform("u_sky_size");
 _u_overlay_color = _overlay_shader.uniform("u_overlay_color");
 _u_overlay_sampler = _overlay_tex_shader.uniform("u_overlay_sampler");
 _u_overlay_ndc_br = _overlay_tex_shader.uniform("u_overlay_ndc_br");
 _u_tint_color = _tint_shader.uniform("u_tint_color");

 constexpr GLsizei S = static_cast<GLsizei>(sizeof(MegaVertex));
 glGenVertexArrays(1, &_mega_vao);
 glGenBuffers(1, &_mega_vbo);
 glGenBuffers(1, &_mega_ebo);
 glBindVertexArray(_mega_vao);
 glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
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

 glGenVertexArrays(1, &_water_vao);
 glGenBuffers(1, &_water_ebo);
 glBindVertexArray(_water_vao);
 glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
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

 glGenVertexArrays(1, &_sky_vao);
 glGenVertexArrays(1, &_crosshair_vao);
 glGenBuffers(1, &_crosshair_vbo);

 const TextureAtlas &cpu_atlas = TextureAtlas::instance();
 _atlas.upload(cpu_atlas);
 _atlas.fill_tile_uvs(cpu_atlas, _tile_uvs);
 _atlas.fill_fallback_colors(_fallback_colors);

 _world_shader.use();
 glUniform1i(_u_atlas, 0);
 glUniform1i(_u_atlas_loaded, _atlas.is_loaded() ? 1 : 0);
 glUniform4fv(_u_tile_uvs, 96, _tile_uvs);
 glUniform3fv(_u_fallback, 16, _fallback_colors);

 _width = width;
 _height = height;
 _ready = true;

 glEnable(GL_DEPTH_TEST);
 glDepthFunc(GL_LESS);
 glEnable(GL_CULL_FACE);
 glCullFace(GL_BACK);

 return (true);
}

void GpuRenderer::destroy() 
{
 _ready = false;
 _atlas.destroy();
 if (_mega_vao != 0) { glDeleteVertexArrays(1, &_mega_vao); _mega_vao = 0; }
 if (_mega_vbo != 0) { glDeleteBuffers(1, &_mega_vbo); _mega_vbo = 0; }
 if (_mega_ebo != 0) { glDeleteBuffers(1, &_mega_ebo); _mega_ebo = 0; }
 _mega_vbo_cap = 0; _mega_ebo_cap = 0;
 _mega_verts.clear(); _mega_idxs.clear();
 if (_water_vao != 0) { glDeleteVertexArrays(1, &_water_vao); _water_vao = 0; }
 if (_water_ebo != 0) { glDeleteBuffers(1, &_water_ebo); _water_ebo = 0; }
 _water_ebo_cap = 0; _water_idxs.clear();
 if (_menu_tex != 0) { glDeleteTextures(1, &_menu_tex); _menu_tex = 0; }
 _menu_tex_w = 0; _menu_tex_h = 0; _menu_rgba.clear();
 if (_overlay_tex != 0) { glDeleteTextures(1, &_overlay_tex); _overlay_tex = 0; }
 if (_sky_vao != 0) { glDeleteVertexArrays(1, &_sky_vao); _sky_vao = 0; }
 if (_crosshair_vao != 0) { glDeleteVertexArrays(1, &_crosshair_vao); _crosshair_vao = 0; }
 if (_crosshair_vbo != 0) { glDeleteBuffers(1, &_crosshair_vbo); _crosshair_vbo = 0; }
 _world_shader.destroy();
 _sky_shader.destroy();
 _overlay_shader.destroy();
 _overlay_tex_shader.destroy();
 _tint_shader.destroy();
 _overlay_rgba.clear();
}

void GpuRenderer::resize(int width, int height) 
{
 _width = width;
 _height = height;
 glViewport(0, 0, width, height);
}

void GpuRenderer::build_mvp(float out[16], const Camera &camera,
 int width, int height, float near_z, float far_z) const 
{
 double cy = std::cos(camera.yaw);
 double sy = std::sin(camera.yaw);
 double cp = std::cos(camera.pitch);
 double sp = std::sin(camera.pitch);

 double tx = -camera.x;
 double ty = -camera.y;
 double tz = -camera.z;

 double vx = tx * cy - tz * sy;
 double vy = ty;
 double vz = tx * sy + tz * cy;

 double vy2 = vy * cp - vz * sp;
 double vz2 = vy * sp + vz * cp;

 float r00 = static_cast<float>(cy);
 float r02 = static_cast<float>(-sy);
 float r10 = static_cast<float>(-sy * sp);
 float r11 = static_cast<float>(cp);
 float r12 = static_cast<float>(-cy * sp);
 float r20 = static_cast<float>(sy * cp);
 float r21 = static_cast<float>(sp);
 float r22 = static_cast<float>(cy * cp);

 float view[16];
 std::memset(view, 0, sizeof(view));
 view[0] = r00; view[4] = 0.0f; view[8] = r02; view[12] = static_cast<float>(vx);
 view[1] = r10; view[5] = r11; view[9] = r12; view[13] = static_cast<float>(vy2);
 view[2] = r20; view[6] = r21; view[10] = r22; view[14] = static_cast<float>(vz2);
 view[3] = 0.0f; view[7] = 0.0f; view[11] = 0.0f; view[15] = 1.0f;

 static const float fov_x = 0.83909963118f;
 float fx = 1.0f / fov_x;
 float fy = fx * (static_cast<float>(width) * 0.5f)
 / (static_cast<float>(height) * 0.52f);
 float fn = far_z + near_z;
 float fd = far_z - near_z;
 float A = fn / fd;
 float B = -2.0f * near_z * far_z / fd;

 float proj[16];
 std::memset(proj, 0, sizeof(proj));
 proj[0] = fx; proj[5] = fy;
 proj[10] = A; proj[11] = 1.0f;
 proj[14] = B;

 for (int col = 0; col < 4; ++col)
 for (int row = 0; row < 4; ++row)
 {
 float s = 0.0f;
 for (int k = 0; k < 4; ++k)
 s += proj[k * 4 + row] * view[col * 4 + k];
 out[col * 4 + row] = s;
 }
}

void GpuRenderer::draw_sky() const 
{
 _sky_shader.use();
 glUniform2f(_u_sky_size,
 static_cast<float>(_width), static_cast<float>(_height));
 glBindVertexArray(_sky_vao);
 glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 glBindVertexArray(0);
}

void GpuRenderer::draw_crosshair() const 
{
 float sx = 8.0f / static_cast<float>(_width);
 float sy = 8.0f / static_cast<float>(_height);

 float verts[] = { -sx, 0.0f, sx, 0.0f, 0.0f, -sy, 0.0f, sy };

 glBindVertexArray(_crosshair_vao);
 glBindBuffer(GL_ARRAY_BUFFER, _crosshair_vbo);
 glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(verts)),
 verts, GL_STREAM_DRAW);
 glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
 static_cast<GLsizei>(2 * sizeof(float)), nullptr);
 glEnableVertexAttribArray(0);

 _overlay_shader.use();
 const float crosshair_color[4] = { 0.96f, 0.96f, 0.92f, 1.0f };
 glUniform4fv(_u_overlay_color, 1, crosshair_color);
 glDisable(GL_DEPTH_TEST);
 glDrawArrays(GL_LINES, 0, 4);
 glEnable(GL_DEPTH_TEST);
 glBindVertexArray(0);
}

void GpuRenderer::render(const Camera &camera, const World &world) 
{
 if (!_ready)
 return ;

 glViewport(0, 0, _width, _height);
 glClear(GL_DEPTH_BUFFER_BIT);
 draw_sky();

 float far_z = static_cast<float>(world.active_render_distance) + 64.0f;
 float mvp[16];
 build_mvp(mvp, camera, _width, _height, 0.08f, far_z);

 RenderCache cache;
 cache.configure(camera, _width, _height, world.active_render_distance);

 _mega_verts.clear();
 _mega_idxs.clear();
 _water_idxs.clear();

 for (int i = 0; i < world.chunk_count; ++i)
 {
 const WorldChunk &wc = world.chunks[i];
 if (wc.initialized == false || wc.mesh.vertices.empty() == FT_TRUE)
 continue ;
 if (!MeshCuller::chunk_is_visible(camera, wc, cache))
 continue ;

 const uint32_t base = static_cast<uint32_t>(_mega_verts.size());
 const float ox = static_cast<float>(wc.world_x);
 const float oz = static_cast<float>(wc.world_z);
 const size_t vcount = wc.mesh.vertices.size();
 for (size_t j = 0; j < vcount; ++j)
 {
 const chunk_mesh_vertex &v = wc.mesh.vertices[j];
 MegaVertex mv;
 mv.x = static_cast<float>(v.coordinate_x) + ox;
 mv.y = static_cast<float>(v.coordinate_y);
 mv.z = static_cast<float>(v.coordinate_z) + oz;
 mv.tu = v.texture_u;
 mv.tv = v.texture_v;
 mv.block_id = v.block_id;
 mv.face = static_cast<uint32_t>(v.face);
 _mega_verts.push_back(mv);
 }
 const size_t icount = wc.mesh.indices.size();
 for (size_t j = 0; j < icount; ++j)
 {
 const uint32_t raw = wc.mesh.indices[j];
 if (wc.mesh.vertices[raw].block_id == TERRAIN_GENERATOR_WATER_BLOCK)
 _water_idxs.push_back(base + raw);
 else
 _mega_idxs.push_back(base + raw);
 }
 }

 if (!_mega_verts.empty())
 {
 const GLsizeiptr vbytes =
 static_cast<GLsizeiptr>(_mega_verts.size() * sizeof(MegaVertex));
 const GLsizeiptr ibytes =
 static_cast<GLsizeiptr>(_mega_idxs.size() * sizeof(uint32_t));

 glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
 if (static_cast<size_t>(vbytes) > _mega_vbo_cap)
 {
 glBufferData(GL_ARRAY_BUFFER, vbytes, nullptr, GL_DYNAMIC_DRAW);
 _mega_vbo_cap = static_cast<size_t>(vbytes);
 }
 glBufferSubData(GL_ARRAY_BUFFER, 0, vbytes, _mega_verts.data());

 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
 if (static_cast<size_t>(ibytes) > _mega_ebo_cap)
 {
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibytes, nullptr,
 GL_DYNAMIC_DRAW);
 _mega_ebo_cap = static_cast<size_t>(ibytes);
 }
 glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibytes, _mega_idxs.data());

 _world_shader.use();
 glUniformMatrix4fv(_u_mvp, 1, GL_FALSE, mvp);
 _atlas.bind(0);

 glBindVertexArray(_mega_vao);
 glDrawElements(GL_TRIANGLES,
 static_cast<GLsizei>(_mega_idxs.size()),
 GL_UNSIGNED_INT, nullptr);
 glBindVertexArray(0);
 }

 if (!_water_idxs.empty())
 {
 const GLsizeiptr ibytes =
 static_cast<GLsizeiptr>(_water_idxs.size() * sizeof(uint32_t));

 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
 if (static_cast<size_t>(ibytes) > _water_ebo_cap)
 {
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibytes, nullptr,
 GL_DYNAMIC_DRAW);
 _water_ebo_cap = static_cast<size_t>(ibytes);
 }
 glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibytes, _water_idxs.data());

 _world_shader.use();
 glUniformMatrix4fv(_u_mvp, 1, GL_FALSE, mvp);
 _atlas.bind(0);

 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glDepthMask(GL_FALSE);
 glBindVertexArray(_water_vao);
 glDrawElements(GL_TRIANGLES,
 static_cast<GLsizei>(_water_idxs.size()),
 GL_UNSIGNED_INT, nullptr);
 glBindVertexArray(0);
 glDepthMask(GL_TRUE);
 glDisable(GL_BLEND);
 }

 draw_crosshair();
}

void GpuRenderer::upload_overlay(const uint32_t *pixels, int w, int h) 
{
 if (pixels == nullptr || w <= 0 || h <= 0)
 return ;

 const size_t count = static_cast<size_t>(w) * static_cast<size_t>(h);
 if (_overlay_rgba.size() != count * 4U)
 _overlay_rgba.resize(count * 4U);

 for (size_t i = 0; i < count; ++i)
 {
 uint32_t p = pixels[i];
 uint8_t r = static_cast<uint8_t>((p >> 16) & 0xFFU);
 uint8_t g = static_cast<uint8_t>((p >> 8) & 0xFFU);
 uint8_t b = static_cast<uint8_t>( p & 0xFFU);
 uint8_t a = r > g ? r : g;
 a = a > b ? a : b;
 _overlay_rgba[i * 4U + 0U] = r;
 _overlay_rgba[i * 4U + 1U] = g;
 _overlay_rgba[i * 4U + 2U] = b;
 _overlay_rgba[i * 4U + 3U] = a;
 }

 bool needs_alloc = (_overlay_tex == 0 || w != _overlay_w || h != _overlay_h);
 if (_overlay_tex == 0)
 glGenTextures(1, &_overlay_tex);
 glBindTexture(GL_TEXTURE_2D, _overlay_tex);
 if (needs_alloc)
 {
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
 static_cast<GLsizei>(w), static_cast<GLsizei>(h), 0,
 GL_RGBA, GL_UNSIGNED_BYTE, _overlay_rgba.data());
 _overlay_w = w;
 _overlay_h = h;
 }
 else
 {
 glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
 static_cast<GLsizei>(w), static_cast<GLsizei>(h),
 GL_RGBA, GL_UNSIGNED_BYTE, _overlay_rgba.data());
 }
 glBindTexture(GL_TEXTURE_2D, 0);
}

void GpuRenderer::draw_overlay() 
{
 if (_overlay_tex == 0 || _overlay_w <= 0 || _overlay_h <= 0)
 return ;
 float ndc_br_x = static_cast<float>(_overlay_w) * 2.0f
 / static_cast<float>(_width) - 1.0f;
 float ndc_br_y = 1.0f - static_cast<float>(_overlay_h) * 2.0f
 / static_cast<float>(_height);
 _overlay_tex_shader.use();
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, _overlay_tex);
 glUniform1i(_u_overlay_sampler, 1);
 glUniform2f(_u_overlay_ndc_br, ndc_br_x, ndc_br_y);
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glDisable(GL_DEPTH_TEST);
 glBindVertexArray(_sky_vao);
 glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 glBindVertexArray(0);
 glDisable(GL_BLEND);
 glEnable(GL_DEPTH_TEST);
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, 0);
 glActiveTexture(GL_TEXTURE0);
}

void GpuRenderer::clear_screen() 
{
 glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GpuRenderer::upload_menu(const uint32_t *pixels, int w, int h) 
{
 if (!pixels || w <= 0 || h <= 0) return;
 const size_t count = static_cast<size_t>(w) * static_cast<size_t>(h);
 if (_menu_rgba.size() != count * 4U)
 _menu_rgba.resize(count * 4U);
 for (size_t i = 0; i < count; ++i)
 {
 uint32_t p = pixels[i];
 uint8_t r = static_cast<uint8_t>((p >> 16) & 0xFFU);
 uint8_t g2 = static_cast<uint8_t>((p >> 8) & 0xFFU);
 uint8_t b = static_cast<uint8_t>( p & 0xFFU);
 uint8_t a = (p == 0U) ? 0U : 255U;
 _menu_rgba[i*4U+0U] = r;
 _menu_rgba[i*4U+1U] = g2;
 _menu_rgba[i*4U+2U] = b;
 _menu_rgba[i*4U+3U] = a;
 }
 bool realloc = (_menu_tex == 0 || w != _menu_tex_w || h != _menu_tex_h);
 if (_menu_tex == 0) glGenTextures(1, &_menu_tex);
 glBindTexture(GL_TEXTURE_2D, _menu_tex);
 if (realloc)
 {
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
 static_cast<GLsizei>(w), static_cast<GLsizei>(h), 0,
 GL_RGBA, GL_UNSIGNED_BYTE, _menu_rgba.data());
 _menu_tex_w = w; _menu_tex_h = h;
 }
 else
 {
 glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
 static_cast<GLsizei>(w), static_cast<GLsizei>(h),
 GL_RGBA, GL_UNSIGNED_BYTE, _menu_rgba.data());
 }
 glBindTexture(GL_TEXTURE_2D, 0);
}

void GpuRenderer::draw_menu(float alpha) 
{
 if (_menu_tex == 0) return;
 (void)alpha;
 _overlay_tex_shader.use();
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, _menu_tex);
 glUniform1i(_u_overlay_sampler, 1);
 glUniform2f(_u_overlay_ndc_br, 1.0f, -1.0f);
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glDisable(GL_DEPTH_TEST);
 glBindVertexArray(_sky_vao);
 glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 glBindVertexArray(0);
 glDisable(GL_BLEND);
 glEnable(GL_DEPTH_TEST);
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, 0);
 glActiveTexture(GL_TEXTURE0);
}

void GpuRenderer::draw_tint(float r, float g, float b, float a) 
{
 if (!_ready)
 return ;
 _tint_shader.use();
 glUniform4f(_u_tint_color, r, g, b, a);
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glDisable(GL_DEPTH_TEST);
 glBindVertexArray(_sky_vao);
 glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 glBindVertexArray(0);
 glDisable(GL_BLEND);
 glEnable(GL_DEPTH_TEST);
}

bool GpuRenderer::is_ready() const { return _ready; }

uint32_t GpuRenderer::gpu_mb_approx() const 
{
 size_t b = _mega_vbo_cap + _mega_ebo_cap + _water_ebo_cap;
 b += static_cast<size_t>(_atlas.is_loaded() ? 192 * 192 * 4 : 0);
 b += static_cast<size_t>(_overlay_w) * static_cast<size_t>(_overlay_h) * 4U;
 b += static_cast<size_t>(_menu_tex_w) * static_cast<size_t>(_menu_tex_h) * 4U;
 return static_cast<uint32_t>(b / (1024UL * 1024UL));
}
