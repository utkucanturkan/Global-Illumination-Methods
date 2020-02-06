#pragma once

#include <glm/glm.hpp>

#include "bbox.h"
#include "material.h"
#include "ray.h"

/// A base class for all entities in the scene.
struct Entity {

    Entity() : material(Material(glm::dvec3(0, 0, 0), 1.0, glm::dvec4(1.0, 0.0, 0.0, 0.0), 0, MaterialType::Diffuse)) {}
    Entity(const Material& material, double _radius) : material(material), radius(_radius) {}

    /// Check if a ray intersects the object
    virtual bool intersect(const Ray& ray, double& intersectionDistance) { return 0; };

    /// Returns an axis-aligned bounding box of the entity.
    //virtual BoundingBox boundingBox() const = 0;

    glm::dvec3 pos = {0, 0, 0};
    Material material;
    float radius;
};

// TODO Implement implicit sphere
// TODO Implement implicit triangle

// TODO Implement explicit sphere (triangles)
// TODO Implement explicit quad (triangles)
// TODO Implement explicit cube (triangles)
