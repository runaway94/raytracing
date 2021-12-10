#include "bvh.h"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <stack>

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
	assert(start < end);

	// Rekursionsabbruch: Nur noch ein Dreieck in der Liste
	if (end - start == 1) {
		uint32_t id = nodes.size();
		nodes.emplace_back();
		nodes[id].triangle = start;
		return id;
	}

	// Hilfsfunktionen
	auto bounding_box = [&](const triangle &triangle) {
		aabb box;
		box.grow(vertices[triangle.a].pos);
		box.grow(vertices[triangle.b].pos);
		box.grow(vertices[triangle.c].pos);
		return box;
	};
	auto center = [&](const triangle &triangle) {
		return (vertices[triangle.a].pos +
				vertices[triangle.b].pos +
				vertices[triangle.c].pos) * 0.333333f;
	};

	// Bestimmen der Bounding Box der (Teil-)Szene
	aabb box;
	for (int i = start; i < end; ++i)
		box.grow(bounding_box(triangles[i]));

	// Sortieren nach der größten Achse
	vec3 extent = box.max - box.min;
	float largest = max(extent.x, max(extent.y, extent.z));
	if (largest == extent.x)
		std::sort(triangles.data()+start, triangles.data()+end,
				  [&](const triangle &a, const triangle &b) { return center(a).x < center(b).x; });
	else if (largest == extent.y)
		std::sort(triangles.data()+start, triangles.data()+end,
				  [&](const triangle &a, const triangle &b) { return center(a).y < center(b).y; });
	else 
		std::sort(triangles.data()+start, triangles.data()+end,
				  [&](const triangle &a, const triangle &b) { return center(a).z < center(b).z; });

	// In der Mitte zerteilen
	int mid = start + (end-start)/2;
	uint32_t id = nodes.size();
	nodes.emplace_back();
	uint32_t l = subdivide(triangles, vertices, start, mid);
	uint32_t r = subdivide(triangles, vertices, mid,   end);
	nodes[id].left  = l;
	nodes[id].right = r;
	nodes[id].box = box;
	return id;
}

triangle_intersection naive_bvh::closest_hit(const ray &ray) {
	
	//node stack, als array wäre performanter aber egal
	std::stack<node> nodeStack;

	//root auf den stack legen
	nodeStack.push(nodes[0]);

	node closestNode;
	float closestHit;
	
	while (!nodeStack.empty())
	{
		//obersten knoten vom stack nehmen
		node n = nodeStack.top();
		nodeStack.pop();

		//distance als erweiterten rückgabewert übergeben
		float distance;

		//der ray trifft die box...
		if(intersect(n.box, ray, distance)){

			//die distanz ist die bisher nähste -> 
			if(distance < closestHit){
				closestHit = distance;
				closestNode = n;
			}
			nodeStack.push(nodes[n.left]);
			nodeStack.push(nodes[n.right]);
		}
	}
	return closestNode.inner();

}


bool naive_bvh::any_hit(const ray &ray) {
	auto is = closest_hit(ray);
	if (is.valid())
		return true;
	return false;
}

