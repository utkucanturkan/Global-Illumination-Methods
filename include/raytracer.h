#pragma once

#include <algorithm>
#include <memory>
//#include <future>

#include <glm/glm.hpp>

#include "camera.h"
#include "entities.h"
#include "image.h"
#include "octree.h"

#include <cmath>

#define PI 3.1415926f

class RayTracer {
  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, glm::dvec3 light)
        : _camera(camera), _light(light), _image(std::make_shared<Image>(0, 0)){};

    void setScene(const Octree* scene) { _scene = scene; }

    void run(int w, int h) {
        // TODO Implement this
        _image = std::make_shared<Image>(w, h);

        glm::dvec3 forward = _camera.forward;
        glm::dvec3 right = glm::normalize(glm::cross(forward, _camera.up));
        glm::dvec3 up = glm::cross(right, forward);

        float _h = tan(45.0f * PI / 180.0f);
        float aspectRatio = (float)w / (float)h;
        float _w = _h * aspectRatio;

        // The structure of the for loop should remain for incremental rendering.
        for (int y = 0; y < h && _running; ++y) {
            for (int x = 0; x < w && _running; ++x) {
                // TODO Implement this

                // Ray ray({x, y, 0}, {0, 0, 1});

                glm::dvec2 screenCoord((2.0f * x) / w - 1.0f, (-2.0f * y) / h + 1.0f);

                Ray ray(_camera.pos, glm::normalize(forward + screenCoord.x * _w * right + screenCoord.y * _h * up));

                for (auto& e : _scene->intersect(ray)) {
                    glm::dvec3 intersection;
                    glm::dvec3 normal;

                    if (e->intersect(ray, intersection, normal)) {
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
