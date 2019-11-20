#include "entities.h"

struct Sphere : public Entity {

	Sphere(glm::dvec3 _position, float _radius, Material _material)
        : Entity(_material), radius(_radius) {
        pos = _position;
	}

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) { 
		
		glm::dvec3 l = pos - ray.origin;
        float tca = glm::dot(l, ray.dir);
        if (tca < 0)
            return false;
        float d2 = glm::dot(l, l) - tca * tca;
        if (d2 > pow(radius,2))
            return false;

        //float thc = sqrt(pow(radius, 2) - d2);


		return 0; 
	}

    //BoundingBox boundingBox() const = 0;

    float radius;
};