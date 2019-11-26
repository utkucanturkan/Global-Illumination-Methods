#pragma once

#include <algorithm>
#include <memory>
//#include <future>

#include <glm/glm.hpp>

#include "camera.h"
#include "entities.h"
#include "image.h"
#include "octree.h"

class RayTracer {
  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, glm::dvec3 light)
        : _camera(camera), _light(light), _image(std::make_shared<Image>(0, 0)){};

    void setScene(const Octree* scene) { _scene = scene; }

    void run(int w, int h) {
        // TODO Implement this
        _image = std::make_shared<Image>(w, h);

		// Eye Coordinate System

		// n -> _camera.forward
		// u
		glm::dvec3 u = glm::normalize(glm::cross(_camera.forward, _camera.up));
		// v
        glm::dvec3 v = glm::cross(u, _camera.forward);

		// Image Plane
        double _h = tan(_camera.focalDist);
        double _w = _h * (w / h);

		glm::dvec3 c = _camera.pos - _camera.forward;


        // The structure of the for loop should remain for incremental rendering.
        for (int y = 0; y < h && _running; ++y) {
            for (int x = 0; x < w && _running; ++x) {
                // TODO Implement this

				double pointU = (2.0f * x) / w - 1.0f;
                double pointV = (2.0 * y) / h + 1.0f;

                glm::dvec3 direction = _camera.forward + pointU * _w * u + pointV * _h * v;
                Ray ray(_camera.pos, glm::normalize(direction));

                for (auto& e : _scene->intersect(ray)) {
                    if (e->intersect(ray, ray.dir, ray.dir)) {
                        glm::dvec3 color = e->material.color;
                        _image->setPixel(x, y, color);
                    }
                }
            }
        }
    }

    bool running() const { return _running; }
    void stop() { _running = false; }
    void start() { _running = true; }

    std::shared_ptr<Image> getImage() const { return _image; }

  private:
    bool _running = false;
    const Octree* _scene;
    Camera _camera;
    glm::dvec3 _light;
    std::shared_ptr<Image> _image;
};
