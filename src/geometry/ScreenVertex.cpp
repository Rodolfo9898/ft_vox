#include "../../src/geometry/ScreenVertex.hpp"

ScreenVertex::ScreenVertex()
{
    this->x = 0.0;
    this->y = 0.0;
    this->depth = 0.0;
    this->texture_u = 0.0;
    this->texture_v = 0.0;
}

ScreenVertex::ScreenVertex(const ScreenVertex &other)
{
    *this = other;
}

ScreenVertex::~ScreenVertex()
{
}

ScreenVertex &ScreenVertex::operator=(const ScreenVertex &other)
{
    if (this != &other)
    {
        this->x = other.x;
        this->y = other.y;
        this->depth = other.depth;
        this->texture_u = other.texture_u;
        this->texture_v = other.texture_v;
    }
    return (*this);
}
