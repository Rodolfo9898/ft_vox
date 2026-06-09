#ifndef GPUR_CHUNK_MESH_HPP
# define GPUR_CHUNK_MESH_HPP

#include <cstdint>
#include "../../Libft/Modules/GPGR/gl_funcs.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cstddef>

class GpuChunkMesh
{
    GLuint  _vao;
    GLuint  _vbo;
    GLuint  _ebo;
    size_t  _uploaded_vertices;
    size_t  _uploaded_indices;
    int32_t _index_count;

    public:
        GpuChunkMesh() noexcept
            : _vao(0), _vbo(0), _ebo(0),
              _uploaded_vertices(0), _uploaded_indices(0),
              _index_count(0) {}
        ~GpuChunkMesh() noexcept { destroy(); }

        GpuChunkMesh(const GpuChunkMesh &)            = delete;
        GpuChunkMesh &operator=(const GpuChunkMesh &) = delete;

        void sync(const chunk_mesh &mesh) noexcept;
        void draw() const noexcept;
        void destroy() noexcept;
        bool has_geometry() const noexcept;
};

#endif
