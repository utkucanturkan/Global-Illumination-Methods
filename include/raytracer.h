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

#define M_PI 3.1415926f

class RayTracer {
  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, glm::dvec3 light)
        : _camera(camera), _light(light), _image(std::make_shared<Image>(0, 0)){};

    void setScene(const Octree* scene) { _scene = scene; }

    void run(int w, int h) {
        _image = std::make_shared<Image>(w, h);

        glm::dvec3 forward = _camera.forward;
        glm::dvec3 right = glm::normalize(glm::cross(forward, _camera.up));
        glm::dvec3 up = glm::cross(right, forward);

        float _h = tan(25.0f * M_PI / 180.0f);
        float aspectRatio = (float)w / (float)h;
        float _w = _h * aspectRatio;

        // The structure of the for loop should remain for incremental rendering.
        for (int y = 0; y < h && _running; ++y) {
            for (int x = 0; x < w && _running; ++x) {

                glm::dvec2 screenCoord((2.0f * x) / w - 1.0f, (-2.0f * y) / h + 1.0f);
                Ray ray(_camera.pos, (forward + screenCoord.x * _w * right + screenCoord.y * _h * up));

                glm::dvec3 color = traceRay(ray);

                _image->setPixel(x, y, color);
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

        return k < 0 ? glm::dvec3(1, 0, 0)
                     : tempI.operator*=(eta) + tempN.operator*=((eta * cosi - sqrtf(k)));
    }

    bool sceneIntersect(const Ray& ray, glm::dvec3& hitPoint, glm::dvec3& hitNormal, Material& material) {
        float spheres_dist = INFINITY;
        for (auto& e : _scene->intersect(ray)) {
            glm::dvec3 intersection{0, 0, 0};
            glm::dvec3 normal;
            if (e->intersect(ray, intersection, normal) && (intersection[0] < spheres_dist)) {
                glm::dvec3 tempRaydir = ray.dir;
                spheres_dist = intersection[0];
                hitPoint = ray.origin + tempRaydir.operator*=(intersection[0]);
                hitNormal = glm::normalize(hitPoint - e->pos);
                material = e->material;
            }
        }

        // CHECKBOARD
        float checkerboard_dist = std::numeric_limits<float>::max();
        if (fabs(ray.dir[1]) > 1e-3) {
            float d = -(ray.origin[1] + 1) / ray.dir[1]; // the checkerboard plane has equation x = -5
            glm::dvec3 tempRayDir = ray.dir;
            glm::dvec3 pt = ray.origin + tempRayDir.operator*=(d);
            if (d > 0 && fabs(pt[0]) < 10 && pt[2] < 5 && pt[2] > -30 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(0, 1, 0);
                material.color = (int(.5 * hitPoint[0] + 1000) + int(.5 * hitPoint[2])) & 1 ? glm::dvec3(1, .6, .1) : glm::dvec3(1, 1, 1);
            }
        }
        return std::min(spheres_dist, checkerboard_dist) < 1000;
    }

    glm::dvec3 traceRay(const Ray& ray, size_t depth = 0) {
        glm::dvec3 hit, normal;
        Material material;

        if (depth > 4 || !sceneIntersect(ray, hit, normal, material)) {
            return glm::dvec3(0.1, 0.5, 0.6);
        }

        glm::dvec3 reflect_dir = glm::normalize(reflect(ray.dir, normal));
        glm::dvec3 refract_dir = glm::normalize(refract(ray.dir, normal, material.refractive_index));

        glm::dvec3 reflect_orig = glm::dot(reflect_dir, normal) < 0 ? hit - normal * 1e-3 : hit + normal * 1e-3;
        glm::dvec3 refract_orig = glm::dot(refract_dir, normal) < 0 ? hit - normal * 1e-3 : hit + normal * 1e-3;

        glm::dvec3 reflect_color = traceRay(Ray(reflect_orig, reflect_dir), depth + 1);
        glm::dvec3 refract_color = traceRay(Ray(refract_orig, refract_dir), depth + 1);

        float lightIntensity = 1.0, diffuse_light_intensity = 0, specular_light_intensity = 0;

        glm::dvec3 light_dir = glm::normalize((_light - hit));
        float light_distance = glm::length(_light - hit);

        glm::dvec3 shadow_orig = glm::dot(light_dir, normal) < 0 ? hit - normal * 1e-3 : hit + normal * 1e-3;
        glm::dvec3 shadow_pt, shadow_N;
        Material tmpmaterial;

        if (!sceneIntersect(Ray(shadow_orig, light_dir), shadow_pt, shadow_N, tmpmaterial) && glm::length(shadow_pt - shadow_orig) > light_distance) {
            return material.color;
        }

        diffuse_light_intensity += lightIntensity * std::max(0.0, glm::dot(light_dir, normal));
        specular_light_intensity += powf(std::max(0.0, glm::dot(-glm::reflect(-light_dir, normal), ray.dir)), material.specular_exponent) * lightIntensity;        

        material.color.operator*=(diffuse_light_intensity).operator*=(material.albedo[0]);
        material.color += glm::dvec3(1., 1., 1.).operator*=(specular_light_intensity).operator*=(material.albedo[1]);
        material.color += reflect_color.operator*=(material.albedo[2]);
        material.color += refract_color.operator*=(material.albedo[3]);
        return material.color;
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
