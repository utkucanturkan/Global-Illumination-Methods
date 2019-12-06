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

        float x0, x1;

        if (discriminant < 0) {
            return false;
        } else if (discriminant == 0) {
            x0 = x1 = -0.5 * b / a;
        } else {
            float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * ((float)b - sqrt(discriminant));
            x0 = q / a;
            x1 = c / q;
        }

        if (x0 > x1) {
            std::swap(x0, x1);
        }
		return true;       
    }

    // BoundingBox boundingBox() const = 0;

    float radius;
};