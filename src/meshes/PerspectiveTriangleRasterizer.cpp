#include "../../src/meshes/PerspectiveTriangleRasterizer.hpp"

PerspectiveTriangleRasterizer::PerspectiveTriangleRasterizer()
{
}

PerspectiveTriangleRasterizer::PerspectiveTriangleRasterizer(
    const PerspectiveTriangleRasterizer &other)
    : TriangleRasterizer(other)
{
    *this = other;
}

PerspectiveTriangleRasterizer::~PerspectiveTriangleRasterizer()
{
}

PerspectiveTriangleRasterizer &PerspectiveTriangleRasterizer::operator=(
    const PerspectiveTriangleRasterizer &other)
{
    (void)other;
    return (*this);
}

double PerspectiveTriangleRasterizer::edge(const ScreenVertex &a,
    const ScreenVertex &b, double x, double y)
{
    return (((x - a.x) * (b.y - a.y)) - ((y - a.y) * (b.x - a.x)));
}

double PerspectiveTriangleRasterizer::edge_step_x(const ScreenVertex &a,
    const ScreenVertex &b)
{
    return (b.y - a.y);
}

double PerspectiveTriangleRasterizer::edge_step_y(const ScreenVertex &a,
    const ScreenVertex &b)
{
    return (a.x - b.x);
}

void PerspectiveTriangleRasterizer::draw_triangle(
    ft_render_framebuffer &framebuffer, std::vector<double> &depth_buffer,
    const ScreenVertex vertices[3], uint32_t block_id, uint8_t face) const
{
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    double area;
    int32_t start_x;
    int32_t end_x;
    int32_t start_y;
    int32_t end_y;
    int32_t x;
    int32_t y;
    double edge0_row;
    double edge1_row;
    double edge2_row;
    double edge0;
    double edge1;
    double edge2;
    double step_x0;
    double step_y0;
    double step_x1;
    double step_y1;
    double step_x2;
    double step_y2;
    double inv_area;
    double inv_depth_values[3];
    double texture_u_over_depth_values[3];
    double texture_v_over_depth_values[3];
    double inv_depth_row;
    double texture_u_over_depth_row;
    double texture_v_over_depth_row;
    double inv_depth_step_x;
    double inv_depth_step_y;
    double texture_u_over_depth_step_x;
    double texture_u_over_depth_step_y;
    double texture_v_over_depth_step_x;
    double texture_v_over_depth_step_y;
    double depth;
    double inv_depth;
    double texture_u_over_depth;
    double texture_v_over_depth;
    double texture_u;
    double texture_v;
    size_t pixel_index;
    TriangleTexture texture;

    area = edge(vertices[0], vertices[1], vertices[2].x, vertices[2].y);
    if (std::fabs(area) < 0.000001)
        return ;
    inv_area = 1.0 / area;
    min_x = std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x));
    max_x = std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x));
    min_y = std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y));
    max_y = std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y));
    start_x = static_cast<int32_t>(std::floor(min_x));
    end_x = static_cast<int32_t>(std::ceil(max_x));
    start_y = static_cast<int32_t>(std::floor(min_y));
    end_y = static_cast<int32_t>(std::ceil(max_y));
    if (start_x < 0)
        start_x = 0;
    if (start_y < 0)
        start_y = 0;
    if (end_x >= framebuffer.width)
        end_x = framebuffer.width - 1;
    if (end_y >= framebuffer.height)
        end_y = framebuffer.height - 1;
    if (start_x > end_x || start_y > end_y)
        return ;
    step_x0 = edge_step_x(vertices[1], vertices[2]);
    step_y0 = edge_step_y(vertices[1], vertices[2]);
    step_x1 = edge_step_x(vertices[2], vertices[0]);
    step_y1 = edge_step_y(vertices[2], vertices[0]);
    step_x2 = edge_step_x(vertices[0], vertices[1]);
    step_y2 = edge_step_y(vertices[0], vertices[1]);
    edge0_row = edge(vertices[1], vertices[2],
        static_cast<double>(start_x) + 0.5,
        static_cast<double>(start_y) + 0.5);
    edge1_row = edge(vertices[2], vertices[0],
        static_cast<double>(start_x) + 0.5,
        static_cast<double>(start_y) + 0.5);
    edge2_row = edge(vertices[0], vertices[1],
        static_cast<double>(start_x) + 0.5,
        static_cast<double>(start_y) + 0.5);
    inv_depth_values[0] = 1.0 / vertices[0].depth;
    inv_depth_values[1] = 1.0 / vertices[1].depth;
    inv_depth_values[2] = 1.0 / vertices[2].depth;
    texture_u_over_depth_values[0] = vertices[0].texture_u
        * inv_depth_values[0];
    texture_u_over_depth_values[1] = vertices[1].texture_u
        * inv_depth_values[1];
    texture_u_over_depth_values[2] = vertices[2].texture_u
        * inv_depth_values[2];
    texture_v_over_depth_values[0] = vertices[0].texture_v
        * inv_depth_values[0];
    texture_v_over_depth_values[1] = vertices[1].texture_v
        * inv_depth_values[1];
    texture_v_over_depth_values[2] = vertices[2].texture_v
        * inv_depth_values[2];
    inv_depth_step_x = ((inv_depth_values[0] * step_x0)
        + (inv_depth_values[1] * step_x1)
        + (inv_depth_values[2] * step_x2)) * inv_area;
    inv_depth_step_y = ((inv_depth_values[0] * step_y0)
        + (inv_depth_values[1] * step_y1)
        + (inv_depth_values[2] * step_y2)) * inv_area;
    texture_u_over_depth_step_x = ((texture_u_over_depth_values[0] * step_x0)
        + (texture_u_over_depth_values[1] * step_x1)
        + (texture_u_over_depth_values[2] * step_x2)) * inv_area;
    texture_u_over_depth_step_y = ((texture_u_over_depth_values[0] * step_y0)
        + (texture_u_over_depth_values[1] * step_y1)
        + (texture_u_over_depth_values[2] * step_y2)) * inv_area;
    texture_v_over_depth_step_x = ((texture_v_over_depth_values[0] * step_x0)
        + (texture_v_over_depth_values[1] * step_x1)
        + (texture_v_over_depth_values[2] * step_x2)) * inv_area;
    texture_v_over_depth_step_y = ((texture_v_over_depth_values[0] * step_y0)
        + (texture_v_over_depth_values[1] * step_y1)
        + (texture_v_over_depth_values[2] * step_y2)) * inv_area;
    inv_depth_row = ((inv_depth_values[0] * edge0_row)
        + (inv_depth_values[1] * edge1_row)
        + (inv_depth_values[2] * edge2_row)) * inv_area;
    texture_u_over_depth_row = ((texture_u_over_depth_values[0] * edge0_row)
        + (texture_u_over_depth_values[1] * edge1_row)
        + (texture_u_over_depth_values[2] * edge2_row)) * inv_area;
    texture_v_over_depth_row = ((texture_v_over_depth_values[0] * edge0_row)
        + (texture_v_over_depth_values[1] * edge1_row)
        + (texture_v_over_depth_values[2] * edge2_row)) * inv_area;
    texture = TextureAtlas::prepare_triangle_texture(block_id, face);
    y = start_y;
    while (y <= end_y)
    {
        edge0 = edge0_row;
        edge1 = edge1_row;
        edge2 = edge2_row;
        inv_depth = inv_depth_row;
        texture_u_over_depth = texture_u_over_depth_row;
        texture_v_over_depth = texture_v_over_depth_row;
        x = start_x;
        while (x <= end_x)
        {
            if ((edge0 >= 0.0 && edge1 >= 0.0 && edge2 >= 0.0)
                || (edge0 <= 0.0 && edge1 <= 0.0 && edge2 <= 0.0))
            {
                pixel_index = static_cast<size_t>((y * framebuffer.width) + x);
                if (inv_depth > 0.0)
                {
                    depth = 1.0 / inv_depth;
                    if (depth > 0.0 && depth < depth_buffer[pixel_index])
                    {
                        texture_u = texture_u_over_depth / inv_depth;
                        texture_v = texture_v_over_depth / inv_depth;
                        depth_buffer[pixel_index] = depth;
                        framebuffer.pixels[pixel_index] =
                            RendererColor::shade_color(
                                TextureAtlas::sample_triangle_texture(texture,
                                    texture_u, texture_v), texture.shade);
                    }
                }
            }
            edge0 += step_x0;
            edge1 += step_x1;
            edge2 += step_x2;
            inv_depth += inv_depth_step_x;
            texture_u_over_depth += texture_u_over_depth_step_x;
            texture_v_over_depth += texture_v_over_depth_step_x;
            x = x + 1;
        }
        edge0_row += step_y0;
        edge1_row += step_y1;
        edge2_row += step_y2;
        inv_depth_row += inv_depth_step_y;
        texture_u_over_depth_row += texture_u_over_depth_step_y;
        texture_v_over_depth_row += texture_v_over_depth_step_y;
        y = y + 1;
    }
}
