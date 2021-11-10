#include "bvh.h"

#include <algorithm>
#include <iostream>
#include <chrono>

using namespace glm;

// 
//    naive_bvh
//

void naive_bvh::build(::scene *scene) {
	this->scene = scene;
	std::cout << "Building BVH..." << std::endl;
	auto t1 = std::chrono::high_resolution_clock::now();

	root = subdivide(scene->triangles, scene->vertices, 0, scene->triangles.size());
	
	auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << "Done after " << duration << "ms" << std::endl;
}

uint32_t naive_bvh::subdivide(std::vector<triangle> &triangles, std::vector<vertex> &vertices, uint32_t start, uint32_t end) {
	
	// rekursionsende, wenn nur noch ein Dreieck da ist
	if(end-start == 1){
		//...
		return;
	}

	//array erweitern
	nodes.emplace_back();

	//eine box machen, die alle dreiecke enthält
	aabb box;
	for(int i = start; i < end; i++){
		triangle tri = triangles[i];
		box.grow(vertices[tri.a].pos);
		box.grow(vertices[tri.b].pos);
		box.grow(vertices[tri.c].pos);
	}

	int xDist = box.min.x - box.max.x;
	int yDist = box.min.y - box.max.y;
	int zDist = box.min.z - box.max.z;

	int maxDist = max(xDist, yDist);
	int maxDist = max(maxDist, zDist);

	if(maxDist == xDist){
		//split at x
		// -> sortiere die dreiecke nach der x position und nimm dann einfach jeweils das halbe array
	}
	else if(maxDist == yDist){
		//split at y
	}
	else if(maxDist == zDist){
		//split at z
	}

	




	
	return 0;
}

triangle_intersection naive_bvh::closest_hit(const ray &ray) {
	// todo
	throw std::logic_error("Not implemented, yet");
	return triangle_intersection();
}


bool naive_bvh::any_hit(const ray &ray) {
	auto is = closest_hit(ray);
	if (is.valid())
		return true;
	return false;
}

