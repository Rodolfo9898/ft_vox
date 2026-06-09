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
        GpuChunkMesh() noexcept;
        GpuChunkMesh(const GpuChunkMesh &other);
        ~GpuChunkMesh() noexcept;
        GpuChunkMesh &operator=(const GpuChunkMesh &other);


        void sync(const chunk_mesh &mesh) noexcept;
        void draw() const noexcept;
        void destroy() noexcept;
        bool has_geometry() const noexcept;
};

#endif
