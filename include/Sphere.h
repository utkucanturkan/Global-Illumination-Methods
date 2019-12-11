#include "entities.h"

struct Sphere : public Entity {

    Sphere(const glm::dvec3 _position, const float _radius, const Material _material)
        : Entity(_material), radius(_radius) {
        pos = _position;
    }

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) override {

        glm::dvec3 oc = ray.origin - pos;
        float a = glm::dot(ray.dir, ray.dir);
        float b = 2.0 * glm::dot(oc, ray.dir);
        float c = glm::dot(oc, oc) - pow(radius, 2);
        float discriminant = pow(b, 2) - 4 * a * c;

        // the ray does not intersect
        if (discriminant <= 0) {
            return false;
        }

        // Find two points of intersection, t1 close and t2 far
        float t1 = (-b - std::sqrt(discriminant)) / (2 * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2 * a);

        if (t1 > t2) {
            std::swap(t1, t2);
        }

        intersect[0] = t1;

        return true;
    }

    // BoundingBox boundingBox() const = 0;

    float radius;
};