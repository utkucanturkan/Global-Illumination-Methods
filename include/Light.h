#pragma once

#include <glm/glm.hpp>

struct Light {
    explicit Light(const glm::dvec3 _position, const double _intensity) : position(_position), intensity(_intensity) {}
    glm::dvec3 position;
    double intensity;
};