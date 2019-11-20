#include <QApplication>

#include <iostream>

#include "camera.h"
#include "gui.h"
#include "Sphere.h"
#include "Triangle.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Camera camera({10, 0, 0});
    glm::dvec3 light{10, 10, 10};

    RayTracer raytracer(camera, light);

	Sphere* sphere = &Sphere({5, 5, 5}, 2.f, Material({50, 50, 50}));

    Triangle *triangle;

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // TODO Add objects to the scene
    scene.push_back(sphere);
    //scene.push_back(triangle);


    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
