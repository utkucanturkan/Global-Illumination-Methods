#include <QApplication>

#include <iostream>

#include "Sphere.h"
#include "Triangle.h"
#include "camera.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Camera camera({10, 10, 0});
    Camera camera({-10, 0, 0});

    glm::dvec3 light{10, 10, 10};

    RayTracer raytracer(camera, light);

    Sphere* greenSphere = &Sphere({10, -2, 0}, 1.0f, Material({0, 1, 0}));
    Sphere* blueSphere = &Sphere({10, 0, -2}, 1.0f, Material({0, 0, 1}));
    Sphere* redSphere = &Sphere({10, 2, 0}, 1.0f, Material({1, 0, 0}));
    Sphere* yellowSphere = &Sphere({10, 0, 2}, 1.0f, Material({0.9, 0.9, 0.1}));

    Triangle* bluetriangle = &Triangle({10, 0, -2}, {10, 2, -2}, {10, 0, -4}, Material({0, 0, 1}));
    Triangle* redtriangle = &Triangle({10, 1, 2}, {10, -1, 2}, {10, 1, 4}, Material({1, 0, 0}));

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // TODO Add objects to the scene
    scene.push_back(greenSphere);
    scene.push_back(blueSphere);
    scene.push_back(redSphere);
    scene.push_back(yellowSphere);

    scene.push_back(bluetriangle);
    scene.push_back(redtriangle);

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
