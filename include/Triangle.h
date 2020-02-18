#include "entities.h"

struct Triangle : public Entity {
    explicit Triangle(const glm::dvec3& a, const glm::dvec3& b, const glm::dvec3& c, const Material& _material): Entity(_material, 0) {
        v1 = a;
        v2 = b;
        v3 = c;
        pos = glm::dvec3(0.1, 0.1, 0.1);
    }

    bool intersect(const Ray& ray, double& intersectionDistance) override {
        double EPS = 0.0000001;

        glm::dvec3 ab = v2 - v1;
        glm::dvec3 ac = v3 - v1;

        glm::dvec3 n = glm::cross(ray.dir, ac);
        double det = glm::dot(ab, n);

        if (det < EPS)
            return false;
        // ray and triangle are parallel if det is close to 0
        if (std::fabs(det) < EPS)
            return false;

        double invDet = 1 / det;
        glm::dvec3 tvec = ray.origin - v1;
        double u = glm::dot(tvec, n) * invDet;
        if (u < 0 || u > 1)
            return false;

        glm::dvec3 qvec = glm::cross(tvec, ab);
        double v = glm::dot(ray.dir, qvec) * invDet;
        if (v < 0 || u + v > 1)
            return false;

        intersectionDistance = glm::dot(ac, qvec) * invDet;
        return (intersectionDistance > EPS) ? true : false;
    }

    glm::dvec3 v1;
    glm::dvec3 v2;
    glm::dvec3 v3;
};