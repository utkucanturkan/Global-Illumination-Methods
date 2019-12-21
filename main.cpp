#include <QApplication>

#include <iostream>

#include "Sphere.h"
#include "Triangle.h"
#include "camera.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // x, y, z
    Camera camera({0, 5, 15});

    glm::dvec3 light{0, 10, 10};

    RayTracer raytracer(camera, light);

    // Materials
    Material ivory(glm::dvec3(0.4, 0.4, 0.3), 1.0, glm::dvec4(0.6, 0.3, 0.1, 0.0), 50.);
    Material glass(glm::dvec3(0.6, 0.7, 0.8), 1.5, glm::dvec4(0.0, 0.5, 0.1, 0.8), 125.);
    Material red_rubber(glm::dvec3(0.3, 0.1, 0.1), 1.0, glm::dvec4(0.9, 0.1, 0.0, 0.0), 10.);
    Material mirror(glm::dvec3(1.0, 1.0, 1.0), 1.0, glm::dvec4(0.0, 10.0, 0.8, 0.0), 1425.);

    // x, y, z
    Sphere* ivorySphere = &Sphere({-2, 3, -5}, 2, ivory);
    Sphere* glassSphere = &Sphere({-3, 1, 0}, 2, glass);
    Sphere* redRubberSphere = &Sphere({2, 1, 0}, 2, red_rubber);
    Sphere* mirrorSphere = &Sphere({2, 4, -5}, 2, mirror);

    Triangle* triangle = &Triangle({0, 0, -10}, {1, 0, -10}, {1, 1, -10}, ivory);

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // TODO Add objects to the scene
    scene.push_back(ivorySphere);
    scene.push_back(redRubberSphere);
    scene.push_back(glassSphere);
    scene.push_back(mirrorSphere);

    //scene.push_back(triangle);

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
