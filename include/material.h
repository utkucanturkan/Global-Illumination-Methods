#pragma once

#include <glm/glm.hpp>

/// Represents the material properties of an entity. For now it only contains color, but it should
/// probably be extended to allow more options.
enum class MaterialType { Diffuse, Specular, Refractive, Light, Dielec};

struct Material {
    explicit Material(glm::dvec3 color,
                      const double refractiveIndex,
                      const glm::dvec4 albedo,
                      const double specularExponent,
                      MaterialType _type,
                      glm::dvec3 emission = glm::dvec3(0))
        : color(std::move(color)), refractive_index(refractiveIndex), albedo(albedo),
          specular_exponent(specularExponent), materialType(_type) {}

    Material()
        : refractive_index(1), albedo(1, 0, 0, 0), color(), specular_exponent(),
          materialType(MaterialType::Diffuse), emission(glm::dvec3(0)) {}

    glm::dvec4 albedo;
    glm::dvec3 color;
    double refractive_index;
    double specular_exponent;
    MaterialType materialType;
    glm::dvec3 emission;
};
