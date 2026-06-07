#include "../../src/geometry/ClipVertex.hpp"

ClipVertex::ClipVertex()
{
    this->view_x = 0.0;
    this->view_y = 0.0;
    this->view_z = 0.0;
    this->texture_u = 0.0;
    this->texture_v = 0.0;
}

ClipVertex::ClipVertex(const ClipVertex &other)
{
    *this = other;
}

ClipVertex::~ClipVertex()
{
}

ClipVertex &ClipVertex::operator=(const ClipVertex &other)
{
    if (this != &other)
    {
        this->view_x = other.view_x;
        this->view_y = other.view_y;
        this->view_z = other.view_z;
        this->texture_u = other.texture_u;
        this->texture_v = other.texture_v;
    }
    return (*this);
}
