#pragma once

#include <algorithm>
#include <memory>
//#include <future>

#include <glm/glm.hpp>

#include "camera.h"
#include "entities.h"
#include "image.h"
#include "octree.h"

#include <Light.h>
#include <cmath>

#include <iostream>
#include <random>

double erand48(unsigned short xsubi[3]) { return (double)rand() / (double)RAND_MAX; }

#define M_PI 3.1415926f

class RayTracer {
  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, std::vector<Light*> lights)
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

                glm::dvec2 screenCoord((2.0 * x) / (double)w - 1.0f, (-2.0 * y) / (double)h + 1.0);
                Ray ray(_camera.pos, (forward + screenCoord.x * _w * right + screenCoord.y * _h * up));
                unsigned short Xi[3] = {0, 0, y * y * y};

                glm::dvec3 pixelColor = traceRay(ray);
                //glm::dvec3 pixelColor = radiance(ray, 0, Xi);

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

    bool intersect(const Ray& ray, glm::dvec3& hitPoint, glm::dvec3& hitNormal, Material& material) {
        double spheres_dist = INFINITY;
        for (auto& e : _scene->intersect(ray)) {
            if (e->pos.x == 0 && !isPathTracing) {
                continue;
            }
            double dist_i;
            if (e->intersect(ray, dist_i) && (dist_i < spheres_dist)) {
                spheres_dist = dist_i;
                hitPoint = ray.origin + (ray.dir * dist_i);
                hitNormal = glm::normalize(hitPoint - e->pos);
                material = e->material;
            }            
        }

        // PLANES
        double checkerboard_dist = INFINITY;
        
        // BACK
        if (fabs(ray.dir.z) > 1e-3) {
            double d = -(ray.origin.z + 30) / ray.dir.z;
            glm::dvec3 pt = ray.origin + ray.dir * d;
            if (d > 0 && fabs(pt.y) < 10 && pt.x < 10 && pt.x > -10 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(0, 0, 1);
                material.color = (int(.5 * hitPoint.x + 1000) + int(.5 * hitPoint.y)) & 1
                                     ? glm::dvec3(.4, .4, .5)
                                     : glm::dvec3(.4, .4, .5);
            }
        }
        // BOTTOM
        if (fabs(ray.dir.y) > 1e-3) {
            double d = -(ray.origin.y + 10) / ray.dir.y;
            glm::dvec3 pt = ray.origin + ray.dir * d;
            if (d > 0 && fabs(pt.x) < 10 && pt.z < 0 && pt.z > -30 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(0, 1, 0);
                material.color = (int(.5 * hitPoint.x + 1000) + int(.5 * hitPoint.z)) & 1
                                     ? glm::dvec3(.3, .3, .3)
                                     : glm::dvec3(.3, .2, .1);
            }
        }
        // RIGHT
        if (fabs(ray.dir.x) > 1e-3) {
            double d = -(ray.origin.x - 10) / ray.dir.x;
            glm::dvec3 pt = ray.origin + ray.dir * d;
            if (d > 0 && fabs(pt.y) < 10 && pt.z < 0 && pt.z > -30 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(-1, 0, 0);
                material.color = (int(.5 * hitPoint.x + 1000) + int(.5 * hitPoint.z)) & 1
                                     ? glm::dvec3(.1, .5, .1)
                                     : glm::dvec3(.1, .5, .1);
            }
        }
        // LEFT
        if (fabs(ray.dir.x) > 1e-3) {
            double d = -(ray.origin.x + 10) / ray.dir.x;
            glm::dvec3 pt = ray.origin + ray.dir * d;
            if (d > 0 && fabs(pt.y) < 10 && pt.z < 0 && pt.z > -30 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(1, 0, 0);
                material.color = (int(.5 * hitPoint.x + 1000) + int(.5 * hitPoint.z)) & 1
                                     ? glm::dvec3(.5, .1, .1)
                                     : glm::dvec3(.5, .1, .1);
            }
        }
        // TOP
        if (fabs(ray.dir.y) > 1e-3) {
            double d = -(ray.origin.y - 10) / ray.dir.y;
            glm::dvec3 pt = ray.origin + ray.dir * d;
            if (d > 0 && fabs(pt.x) < 10 && pt.z < 0 && pt.z > -30 && d < spheres_dist) {
                checkerboard_dist = d;
                hitPoint = pt;
                hitNormal = glm::dvec3(0, -1, 0);
                material.color = (int(.5 * hitPoint.x + 1000) + int(.5 * hitPoint.z)) & 1
                                     ? glm::dvec3(.2, .2, .5)
                                     : glm::dvec3(.2, .2, .5);
            }
        }
        

        return std::min(spheres_dist, checkerboard_dist) < 1000;
    }

    glm::dvec3 traceRay(const Ray& ray, size_t depth = 0) {
        isPathTracing = false;
        glm::dvec3 nearestIntersectionPoint, nearestNormal;
        Material material;

        if (depth > 4 || !intersect(ray, nearestIntersectionPoint, nearestNormal, material)) {
            return glm::dvec3(.9, .9, .9); // background color
        }

        glm::dvec3 reflect_dir = glm::normalize(reflect(ray.dir, nearestNormal));
        glm::dvec3 refract_dir = glm::normalize(refract(ray.dir, nearestNormal, material.refractive_index));

        glm::dvec3 reflect_orig = glm::dot(reflect_dir, nearestNormal) < 0 ? nearestIntersectionPoint - nearestNormal * 1e-3 : nearestIntersectionPoint + nearestNormal * 1e-3;
        glm::dvec3 refract_orig = glm::dot(refract_dir, nearestNormal) < 0 ? nearestIntersectionPoint - nearestNormal * 1e-3 : nearestIntersectionPoint + nearestNormal * 1e-3;

        glm::dvec3 reflect_color = traceRay(Ray(reflect_orig, reflect_dir), depth + 1);
        glm::dvec3 refract_color = traceRay(Ray(refract_orig, refract_dir), depth + 1);

        double diffuse_light_intensity = 0, specular_light_intensity = 0;

        // Iterate over lights
        for (auto& e : _lights) {

            glm::dvec3 light_dir = glm::normalize((e->position - nearestIntersectionPoint));
            double light_distance = glm::length(e->position - nearestIntersectionPoint);
            glm::dvec3 shadow_orig = glm::dot(light_dir, nearestNormal) < 0 ? nearestIntersectionPoint - nearestNormal * 1e-3 : nearestIntersectionPoint + nearestNormal * 1e-3;

            
            // Shadows
            glm::dvec3 shadow_pt, shadow_N;
            Material tmpmaterial;
            if (intersect(Ray(shadow_orig, light_dir), shadow_pt, shadow_N, tmpmaterial) && glm::length(shadow_pt - shadow_orig) < light_distance)
                continue;
            
            diffuse_light_intensity += e->intensity * std::max(0.0, glm::dot(light_dir, nearestNormal));
            specular_light_intensity += powf(std::max(0.0, glm::dot(-glm::reflect(-light_dir, nearestNormal), ray.dir)), material.specular_exponent) * e->intensity;
        }

        return material.color * diffuse_light_intensity * material.albedo[0] + glm::dvec3(1.0, 1.0, 1.0) * specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
    }

    glm::dvec3 radiance(const Ray& ray, int depth, unsigned short* Xi, double E = 1.0) {
        isPathTracing = true;
        glm::dvec3 intersectionPoint, normal;
        Material material;

        if (!intersect(ray, intersectionPoint, normal, material)) {
            return glm::dvec3(.9, .9, .9); // background color
        }

        if (++depth > 10) {
            return glm::dvec3(0);
        }

        glm::dvec3 orientedNormal = (glm::dot(normal, ray.dir) < 0) ? normal : normal * -1.0;

        // Russian Roulette; use maximum reflectivity amount
        double p = (material.color.x > material.color.y && material.color.x > material.color.z) ? material.color.z : (material.color.y > material.color.z) ? material.color.y : material.color.z;

        if (++depth > 5 || !p) {
            if (erand48(Xi) < p) {
                material.color = material.color * (1.0 / p);
            } else {
                return material.emission * E;
            }
        }

        // Diffuse
        if (material.materialType == MaterialType::Diffuse) {
            // Ideal Diffuse Reflection
            double r1 = 2 * M_PI * erand48(Xi); // angle around
            double r2 = erand48(Xi);
            double r2s = sqrt(r2); // distance from center

            glm::dvec3 w = orientedNormal;
            glm::dvec3 u = glm::normalize(glm::cross((fabs(w.x) > .1 ? glm::dvec3(0, 1, 0) : glm::dvec3(1, 0, 0)), w));              // u is perpendicular to w
            glm::dvec3 v = glm::cross(w, u); // v is perpendicular to u and w
            glm::dvec3 d = glm::normalize((u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2))); // d is random reflection ray

            // loop over any lights
            glm::dvec3 e;
            for (auto& light : _scene->intersect(ray)) {
                if (light->pos.x != 0) // light is a sphere and x coordinate is 0 to distinguish from other spheres
                    continue; // Skip non-lights

                // create random direction towards sphere using method from realistic ray tracing
                glm::dvec3 sw = light->pos - intersectionPoint;
                glm::dvec3 su = glm::normalize(glm::cross((fabs(sw.x) > 0.1 ? glm::dvec3(0, 1, 0) : glm::dvec3(1, 0, 0)), sw));
                glm::dvec3 sv = glm::cross(sw, su);
                double cos_a_max = sqrt(1 - pow(light->radius, 2) / glm::dot((intersectionPoint - light->pos), (intersectionPoint - light->pos)));

                double eps1 = erand48(Xi);
                double eps2 = erand48(Xi);
                double cos_a = 1 - eps1 + eps1 * cos_a_max;
                double sin_a = sqrt(1 - cos_a * cos_a);
                double phi = 2 * 3.14159265358979 * eps2;
                glm::dvec3 l = glm::normalize(su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a);

                // shoot shadow rays
                Material tmpMaterial;
                if (intersect(Ray(intersectionPoint, l), glm::dvec3(), glm::dvec3(), tmpMaterial)) {
                    double omega = 2 * M_PI * (1 - cos_a_max);
                    e = e + (material.color * light->material.emission * glm::dot(l, orientedNormal) * omega) * (1.0 / M_PI); // 1/pi fpr brdf
                }
            }

            return material.emission * E + e + (material.color * radiance(Ray(intersectionPoint, d), depth, Xi, 0));
        } else if (material.materialType == MaterialType::Specular) {
            return material.emission + (material.color * radiance(Ray(intersectionPoint, (ray.dir - normal * 2.0 * glm::dot(normal, ray.dir))), depth, Xi));
        }
        // OTHERWISE WE HAVE A DIELECTRIC(GLASS) SURFACE
        Ray reflRay( intersectionPoint, (ray.dir - normal * 2.0 * glm::dot(normal, ray.dir))); // Ideal dielectric reflection

        bool into = glm::dot(normal, orientedNormal) > 0; // ray from outside going in?
        double nc = 1;
        double nt = 1.5;
        double nnt = into ? nc / nt : nt / nc;
        double ddn = glm::dot(ray.dir, orientedNormal);
        double cos2t;
        // if total internal reflection, REFLECT
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) { // total internal reflection
            return material.emission + (material.color * radiance(reflRay, depth, Xi));
        }
        // otherwise, choose REFLECTION or REFRACTION
        glm::dvec3 tdir = glm::normalize((ray.dir * nnt - normal * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))));
        double a = nt - nc;
        double b = nt + nc;
        double R0 = a * a / (b * b);
        double c = 1 - (into ? -ddn : glm::dot(tdir, normal));
        double Re = R0 + (1 - R0) * c * c * c * c * c;
        double Tr = 1 - Re;
        double P = .25 + .5 * Re;
        double RP = Re / P;
        double TP = Tr / (1 - P);
        return material.emission + material.color * (depth > 2 ? (erand48(Xi) < P ? radiance(reflRay, depth, Xi) * RP : radiance(Ray(intersectionPoint, tdir), depth, Xi) * TP) : radiance(reflRay, depth, Xi) * Re + radiance(Ray(intersectionPoint, tdir), depth, Xi) * Tr);
    }

    bool running() const { return _running; }
    void stop() { _running = false; }
    void start() { _running = true; }

    std::shared_ptr<Image> getImage() const { return _image; }

  private:
    bool _running = false;
    const Octree* _scene;
    Camera _camera;
    std::vector<Light*> _lights;
    std::shared_ptr<Image> _image;
    bool isPathTracing = false;
};
