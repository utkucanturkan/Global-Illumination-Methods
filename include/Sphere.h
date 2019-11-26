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
        return (discriminant > 0);

        /*
		if (discriminant < 0)
			return false;
		else {
			discriminant = sqrt(discriminant);
			double t0 = -b - discriminant;
			double t1 = -b + discriminant;

						//t=(t0<t1) ? t0 : t1;
			return true;
		}
        */
    }

    // BoundingBox boundingBox() const = 0;

    float radius;
};