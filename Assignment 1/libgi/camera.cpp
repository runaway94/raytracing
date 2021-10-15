#include "camera.h"

#include "random.h"

#include <iostream>
#include <fstream>

using namespace glm;
using namespace std;

//! Set up a camera ray using cam.up, cam.dir, cam.w, cam.h (see \ref camera::update_frustum)
ray cam_ray(const camera &cam, int x, int y, vec2 offset) {

	cout << "test." << endl;

	vec3 U = cross(cam.dir, cam.up);
	vec3 V = cross(U, cam.dir);

	float px = (x+0.5f+offset.x)/cam.w * 2 - 1;
	float py = (y+0.5f+offset.y)/cam.h * 2 - 1;
	vec3 pxy = cam.pos+cam.dir + U*px + V*py;
	vec3 d = normalize(pxy - cam.pos);

	return ray(cam.pos, d);
}

void test_camrays(const camera &camera, int stride) {
	ofstream out("test.obj");
	int i = 1;
	for (int y = 0; y < camera.h; y += stride)
		for (int x = 0; x < camera.w; x += stride) {
			ray ray = cam_ray(camera, x, y);
			out << "v " << ray.o.x << " " << ray.o.y << " " << ray.o.z << endl;
			out << "v " << ray.d.x << " " << ray.d.y << " " << ray.d.z << endl;
			out << "l " << i++ << " " << i++ << endl;
		}
}


