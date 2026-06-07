#include "../../src/geometry/TriangleTexture.hpp"

TriangleTexture::TriangleTexture()
{
    this->atlas = nullptr;
    this->fallback_color = 0U;
    this->base_x = 0;
    this->base_y = 0;
    this->sample_width = 1;
    this->sample_height = 1;
    this->shade = 1.0;
    this->loaded = false;
}

TriangleTexture::TriangleTexture(const TriangleTexture &other)
{
    *this = other;
}

TriangleTexture::~TriangleTexture()
{
}

TriangleTexture &TriangleTexture::operator=(const TriangleTexture &other)
{
    if (this != &other)
    {
        this->atlas = other.atlas;
        this->fallback_color = other.fallback_color;
        this->base_x = other.base_x;
        this->base_y = other.base_y;
        this->sample_width = other.sample_width;
        this->sample_height = other.sample_height;
        this->shade = other.shade;
        this->loaded = other.loaded;
    }
    return (*this);
}
