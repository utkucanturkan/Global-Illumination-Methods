#include <QApplication>

#include <iostream>

#include "Sphere.h"
#include "Triangle.h"
#include "camera.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // x, y, z
    Camera camera({0, 0, 20});

    glm::dvec3 light{0, 0, 10};

    RayTracer raytracer(camera, light);

    // Materials
    Material ivory(glm::dvec3(0.4, 0.4, 0.3), 1.0, glm::dvec4(0.6, 0.3, 0.1, 0.0), 50.);
    Material glass(glm::dvec3(0.6, 0.7, 0.8), 1.5, glm::dvec4(0.0, 0.5, 0.1, 0.8), 125.);
    Material red_rubber(glm::dvec3(1, 0, 0), 1.0, glm::dvec4(0.9, 0.1, 0.0, 0.0), 10.);
    Material mirror(glm::dvec3(1.0, 1.0, 1.0), 1.0, glm::dvec4(0.0, 10.0, 0.8, 0.0), 1425.);

    // x, y, z
    Sphere* ivorySphere = &Sphere({-3, 0, -6}, 2, ivory);
    Sphere* glassSphere = &Sphere({0, 3, -6}, 2, glass);
    Sphere* redRubberSphere = &Sphere({3, 0, -6}, 2, red_rubber);
    Sphere* mirrorSphere = &Sphere({0, -3, -6}, 2, mirror);

    // Triangle* bluetriangle = &Triangle({0, 0, -5}, {2, 0, -5}, {2, 2, -5}, Material({1, 0, 0}));
    // Triangle* redtriangle = &Triangle({-5, -1, -1}, {-5, -1, 1}, {-5, 1, 0}, Material({1, 0,
    // 0}));

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // TODO Add objects to the scene
    scene.push_back(ivorySphere);
    scene.push_back(redRubberSphere);
    scene.push_back(glassSphere);
    scene.push_back(mirrorSphere);

    // scene.push_back(bluetriangle);
    // scene.push_back(redtriangle);

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
