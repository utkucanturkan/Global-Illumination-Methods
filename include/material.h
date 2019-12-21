#pragma once

#include <glm/glm.hpp>

/// Represents the material properties of an entity. For now it only contains color, but it should
/// probably be extended to allow more options.
struct Material {
    constexpr explicit Material(glm::dvec3 color,const float refractiveIndex,const glm::dvec4& albedo,const float specularExponent): color(std::move(color)), refractive_index(refractiveIndex), albedo(albedo), specular_exponent(specularExponent) {}

    Material() : refractive_index(1), albedo(1, 0, 0, 0), color(), specular_exponent() {}

    glm::dvec3 color;
    float refractive_index;
    glm::dvec4 albedo;
    float specular_exponent;
};