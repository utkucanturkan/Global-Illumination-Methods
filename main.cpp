#include <QApplication>

#include <iostream>

#include "Sphere.h"
#include "Triangle.h"
#include "camera.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Camera camera({10, 10, 0});
    glm::dvec3 light{10, 10, 10};

    RayTracer raytracer(camera, light);

	const int W = 500, H = 500;
    const float radius = 40.f;
    
    Sphere* sphere1 = &Sphere(
		{(W / 2) - (radius * sqrt(2)), H / 2, 20}, 
		radius, 
		Material({0, 0, 1}));

    Sphere* sphere2 = &Sphere(
		{H / 2, (W / 2) - (radius * sqrt(2)), 0}, 
		radius, 
		Material({0, 1, 0}));

    Sphere* sphere3 = &Sphere(
		{(W / 2) + (radius * sqrt(2)), H / 2, 0}, 
		radius, 
		Material({1, 0, 0}));

    Sphere* sphere4 = &Sphere(
		{H / 2, (W / 2) + (radius * sqrt(2)), 0}, 
		radius, 
		Material({0.9, 0.9, 0.1}));

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // TODO Add objects to the scene
    scene.push_back(sphere1);
    scene.push_back(sphere2);
    scene.push_back(sphere3);
    scene.push_back(sphere4);

    raytracer.setScene(&scene);

    Gui window(W, H, raytracer);
    window.show();
    return app.exec();
}
