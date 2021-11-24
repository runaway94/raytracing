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
		uint32_t id = nodes.size();
		
		//array erweitern
		nodes.emplace_back();

		nodes[id].triangle = start;
		return;
	}

	//array erweitern
	nodes.emplace_back();

	//eine box machen, die alle dreiecke enthält
	//anfangs ist die Box ganz klein und wächst dann für jedes Eck von jedem Dreieck, das sich noch draußen befindet
	aabb box;
	for(int i = start; i < end; i++){
		triangle tri = triangles[i];
		box.grow(vertices[tri.a].pos);
		box.grow(vertices[tri.b].pos);
		box.grow(vertices[tri.c].pos);
	}

	//Hilfsfunktion, um die Mitte der Dreiecke zu berechnen
	auto center = [&](const triangle &triangle) {
		return (vertices[triangle.a].pos + vertices[triangle.b].pos + vertices[triangle.c].pos) * 0.33333f;
	};

	int xDist = box.min.x - box.max.x;
	int yDist = box.min.y - box.max.y;
	int zDist = box.min.z - box.max.z;

	int maxDist = max(xDist, yDist);
	int maxDist = max(maxDist, zDist);


	//dreiecke entlang der längsten Kante der Box sortieren
	if(maxDist == xDist){
		//split at x
		// -> sortiere die dreiecke nach der x position und nimm dann einfach jeweils das halbe array
		std::sort(triangles.data() + start, triangles.data() + end, 

		//lambda expression (wonach soll sortiert werden, wie in Java)
		[&](const triangle &a, const triangle &b) {
			return center(a).x < center(b).x;
		});
	}
	else if(maxDist == yDist){
		//split at y
		std::sort(triangles.data() + start, triangles.data() + end, 

		//lambda function
		[&](const triangle &a, const triangle &b) {
			return center(a).y < center(b).y;
		});
	}
	else if(maxDist == zDist){
		//split at z

		std::sort(triangles.data() + start, triangles.data() + end, 

		//lambda function
		[&](const triangle &a, const triangle &b) {
			return center(a).z < center(b).z;
		});
	}

	//liste der dreiecke halbieren und funktion rekursiv aufrufen
	int middle = start + (end-start)/2;

	uint32_t id = nodes.size();
	
	//array erweitern
	nodes.emplace_back();

	uint32_t leftSide = subdivide(triangles, vertices, start, middle);
	uint32_t rightSide = subdivide(triangles, vertices, middle+1, end);

	nodes[id].left = leftSide;
	nodes[id].right = rightSide;	
	nodes[id].box = box;

	return id;
}

triangle_intersection naive_bvh::closest_hit(const ray &ray) {
	triangle_intersection closest, intersect;
	
	//array stack
	uint32_t stack[25];
	uint32_t sp = 0;
	
	//push root onto stack
	stack[0] = root;

	//check if ray intersects root
	//while stack is not empty -> while sp >= 0
	while(sp>=0){
		//push last node from stack
		node node = nodes[stack[sp]];

		//check if node intersects
		
	}




	throw std::logic_error("Not implemented, yet");
	return triangle_intersection();
}


bool naive_bvh::any_hit(const ray &ray) {
	auto is = closest_hit(ray);
	if (is.valid())
		return true;
	return false;
}

