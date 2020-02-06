#include <QApplication>

#include <iostream>

#include "Sphere.h"
#include "Triangle.h"
#include "camera.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Camera camera({0, 0, 20});

    std::vector<Light*> lights;
    lights.push_back(new Light(glm::dvec3(-2, 5, -20), 1.0));
    lights.push_back(new Light(glm::dvec3(0, 10, 20), 1.0));


    RayTracer raytracer(camera, lights);

    Material ivory(glm::dvec3(0.4, 0.4, 0.3), 1.0, glm::dvec4(0.6, 0.3, 0.1, 0.0), 50., MaterialType::Refractive);
    Material glass(glm::dvec3(0.6, 0.7, 0.8), 1.5, glm::dvec4(0.0, 0.5, 0.1, 0.8), 125., MaterialType::Dielec);
    Material red_rubber(glm::dvec3(0.3, 0.1, 0.1), 1.0, glm::dvec4(0.9, 0.1, 0.0, 0.0), 10., MaterialType::Diffuse);
    Material mirror(glm::dvec3(1.0, 1.0, 1.0), 1.0, glm::dvec4(0.0, 10.0, 0.8, 0.0), 1425., MaterialType::Specular);
    Material light(glm::dvec3(1.0), 0, glm::dvec4(0), 0, MaterialType::Diffuse, glm::dvec3(4.0));


    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});
    scene.push_back(new Sphere({-1, -8, -10}, 2, ivory));
    scene.push_back(new Sphere({-7, -8, -20}, 2, glass));
    scene.push_back(new Sphere({7, -8, -10}, 2, red_rubber));
    scene.push_back(new Sphere({4, -8, -20}, 2, mirror));
    scene.push_back(new Sphere({0, 10, -15}, 2, light));
    scene.push_back(new Triangle({0, 0, -10}, {50, 0, -10}, {50, 50, -10}, ivory));

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
