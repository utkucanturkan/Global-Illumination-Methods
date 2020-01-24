#include "entities.h"

struct Triangle : public Entity {
    Triangle(const glm::dvec3& a,
             const glm::dvec3& b,
             const glm::dvec3& c,
             const Material& _material)
        : Entity(_material) {
        v1 = a;
        v2 = b;
        v3 = c;
    }

    bool intersect(const Ray& ray, double& intersectionDistance) override {
        float EPS = 0.0000001;

        glm::dvec3 ab = v2 - v1;
        glm::dvec3 ac = v3 - v1;

        glm::dvec3 n = glm::cross(ray.dir, ac);
        float det = glm::dot(ab, n);

        if (det < EPS)
            return false;
        // ray and triangle are parallel if det is close to 0
        if (std::fabs(det) < EPS)
            return false;

        float invDet = 1 / det;
        glm::dvec3 tvec = ray.dir - v1;
        float u = glm::dot(tvec, n) * invDet;
        if (u < 0 || u > 1)
            return false;

        glm::dvec3 qvec = glm::cross(tvec, ab);
        float v = glm::dot(ray.dir, qvec) * invDet;
        if (v < 0 || u + v > 1)
            return false;

        intersectionDistance = glm::dot(ac, qvec) * invDet;
        return (intersectionDistance > EPS) ? true : false;
    }

    glm::dvec3 v1;
    glm::dvec3 v2;
    glm::dvec3 v3;
};