#include "entities.h"

struct Sphere : public Entity {

    Sphere(const glm::dvec3 _position, const float _radius, const Material _material)
        : Entity(_material, _radius) {
        pos = _position;
    }

    bool intersect(const Ray& ray, double& intersectionDistance) override {

        glm::dvec3 L = pos - ray.origin;
        double tca = glm::dot(L, ray.dir);
        double d2 = glm::dot(L, L) - pow(tca, 2);
        if (d2 > pow(radius, 2))
            return false;
        double thc = sqrtf(pow(radius, 2) - d2);
        intersectionDistance = tca - thc;
        double t1 = tca + thc;
        if (intersectionDistance < 0)
            intersectionDistance = t1;
        if (intersectionDistance < 0)
            return false;
        return true;
    }

    // BoundingBox boundingBox() const = 0;

    
};