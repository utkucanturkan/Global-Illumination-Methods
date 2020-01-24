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
#include <Light.h>

#define M_PI 3.1415926f

class RayTracer {
  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, std::vector<Light> lights)
        : _camera(camera), _lights(lights), _image(std::make_shared<Image>(0, 0)){};

    void setScene(const Octree* scene) { _scene = scene; }

    void run(int w, int h) {
        _image = std::make_shared<Image>(w, h);

        glm::dvec3 forward = _camera.forward;
        glm::dvec3 right = glm::normalize(glm::cross(forward, _camera.up));
        glm::dvec3 up = glm::cross(right, forward);

        double _h = tan(25.0 * M_PI / 180.0);
        double aspectRatio = (double)w / (double)h;
        double _w = _h * aspectRatio;

        // The structure of the for loop should remain for incremental rendering.
        for (int y = 0; y < h && _running; ++y) {
            for (int x = 0; x < w && _running; ++x) {

                glm::dvec2 screenCoord((2.0f * x) / (double)w - 1.0f, (-2.0f * y) / (double)h + 1.0f);
                Ray ray(_camera.pos, (forward + screenCoord.x * _w * right + screenCoord.y * _h * up));

                glm::dvec3 pixelColor = traceRay(ray);

                _image->setPixel(x, y, pixelColor);
            }
        }
    }

    glm::dvec3 refract(const glm::dvec3& I, const glm::dvec3& N, const float eta_t, const float eta_i = 1.f) {
        // Snell's law
        float cosi = -std::max(-1.0, std::min(1.0, glm::dot(I, N)));

        if (cosi < 0)
            return refract(I, -N, eta_i, eta_t);
        float eta = eta_i / eta_t;
        float k = 1 - eta * eta * (1 - cosi * cosi);

        glm::dvec3 tempI = I;
        glm::dvec3 tempN = N;

        return k < 0 ? glm::dvec3(1, 0, 0) : tempI.operator*=(eta) + tempN.operator*=((eta * cosi - sqrtf(k)));
    }

    bool sceneIntersect(const Ray& ray, glm::dvec3& hitPoint, glm::dvec3& hitNormal, Material& material) {
        double spheres_dist = INFINITY;
        for (auto& e : _scene->intersect(ray)) {
            double dist_i;
            if (e->intersect(ray, dist_i) && (dist_i < spheres_dist)) {
                spheres_dist = dist_i;
                hitPoint = ray.origin + (ray.dir * dist_i);
                hitNormal = glm::normalize(hitPoint - e->pos);
                material = e->material;
            }
        }

        // CHECKBOARD
        double checkerboard_dist = INFINITY;
        if (fabs(ray.dir.y) > 1e-3) {
            double d = -(ray.origin.y + 5) / ray.dir.y; // the checkerboard plane has equation y = -4
            glm::dvec3 pt = ray.origin + ray.dir * d;
            if (d > 0 && fabs(pt.x) < 20 && pt.z < 0 && pt.z > -30 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(0, 1, 0);
                material.color = (int(.5 * hitPoint.x + 1000) + int(.5 * hitPoint.z)) & 1 ? glm::dvec3(.3, .3, .3) : glm::dvec3(.3, .2, .1);
            }
        }
        return std::min(spheres_dist, checkerboard_dist) < 1000;
    }

    glm::dvec3 traceRay(const Ray& ray, size_t depth = 0) {
        glm::dvec3 point, N;
        Material material;

        if (depth > 4 || !sceneIntersect(ray, point, N, material)) {
            return glm::dvec3(0.1, 0.5, 0.6);   // background color
        }

        glm::dvec3 reflect_dir = glm::normalize(reflect(ray.dir, N));
        glm::dvec3 refract_dir = glm::normalize(refract(ray.dir, N, material.refractive_index));

        glm::dvec3 reflect_orig = glm::dot(reflect_dir, N) < 0 ? point - N * 1e-3 : point + N * 1e-3;
        glm::dvec3 refract_orig = glm::dot(refract_dir, N) < 0 ? point - N * 1e-3 : point + N * 1e-3;

        glm::dvec3 reflect_color = traceRay(Ray(reflect_orig, reflect_dir), depth + 1);
        glm::dvec3 refract_color = traceRay(Ray(refract_orig, refract_dir), depth + 1);

        double diffuse_light_intensity = 0, specular_light_intensity = 0;
        
        // Iterate over lights
        for (auto& e : _lights) {
            glm::dvec3 light_dir = glm::normalize((e.position - point));
            double light_distance = glm::length(e.position - point);
            glm::dvec3 shadow_orig = glm::dot(light_dir, N) < 0 ? point - N * 1e-3 : point + N * 1e-3;
            glm::dvec3 shadow_pt, shadow_N;
            Material tmpmaterial;
            if (sceneIntersect(Ray(shadow_orig, light_dir), shadow_pt, shadow_N, tmpmaterial) && glm::length(shadow_pt - shadow_orig) < light_distance)
                continue;
            diffuse_light_intensity += e.intensity * std::max(0.0, glm::dot(light_dir, N));
            specular_light_intensity += powf(std::max(0.0, glm::dot(-glm::reflect(-light_dir, N), ray.dir)), material.specular_exponent) * e.intensity;       
        }

        return material.color * diffuse_light_intensity * material.albedo[0] + glm::dvec3(1.0, 1.0, 1.0) * specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
    }

    bool running() const { return _running; }
    void stop() { _running = false; }
    void start() { _running = true; }

    std::shared_ptr<Image> getImage() const { return _image; }

  private:
    bool _running = false;
    const Octree* _scene;
    Camera _camera;
    std::vector<Light> _lights;
    std::shared_ptr<Image> _image;
};
