#include "entities.h"

struct Triangle : public Entity {
    Triangle(const glm::dvec3& position, const Material& _material) : Entity(_material) {
        pos = position;
    }

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) override { 
			
		return 0; 
	}
};