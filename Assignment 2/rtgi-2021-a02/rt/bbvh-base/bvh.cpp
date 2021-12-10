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

/*
*  Assignment 2.2a
*/

uint32_t naive_bvh::subdivide(
	std::vector<triangle> &triangles, //alle dreiecke, die wir prüfen wollen
	std::vector<vertex> &vertices, //
	uint32_t start, 
	uint32_t end
	) {
	
	nodes; 	//wird in der Bibliothek definiert, eine Liste von Knoten. 
			//ein Knoten hat einen rechten und einen linken nachfolger (als int das entsprechende Feld im Array)
			//ein triangle (wenn er ein Blatt knoten ist)
			//eine box

	// rekursionsende, wenn nur noch ein Dreieck da ist
	if(end-start == 1){

		//die id ist das nächste Feld
		uint32_t id = nodes.size(); 
		
		//array erweitern
		nodes.emplace_back();


		nodes[id].triangle = start;
		return id;
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


	int xDist = box.max.x - box.min.x;
	int yDist = box.max.y - box.min.y;
	int zDist = box.max.z - box.min.z;
	int maxDist = max(xDist, yDist);
	maxDist = max(maxDist, zDist);


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
	uint32_t rightSide = subdivide(triangles, vertices, middle, end);

	nodes[id].left = leftSide;
	nodes[id].right = rightSide;	
	nodes[id].box = box;

	return id;
}

/*
*  Assignment 2.2b
*/

triangle_intersection naive_bvh::closest_hit(const ray &ray) {
	

	/*
	//node stack, als array wäre performanter aber egal
	std::stack<node> nodeStack;

	//root auf den stack legen
	nodeStack.push(nodes[root]);

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
	return closestNode.inner(); */

		triangle_intersection closest, intersection;
	uint32_t stack[25];
	int32_t sp = 0;
	stack[0] = root;
#ifdef COUNT_HITS
	unsigned int hits = 0;
#endif
	while (sp >= 0) {
		node node = nodes[stack[sp--]];
#ifdef COUNT_HITS
		hits++;
#endif
		if (node.inner()) {
			float dist;
			if (intersect(node.box, ray, dist))
				if (dist < closest.t) {
					stack[++sp] = node.left;
					stack[++sp] = node.right;
				}
		}
		else {
			if (intersect(scene->triangles[node.triangle], scene->vertices.data(), ray, intersection))
				if (intersection.t < closest.t) {
					closest = intersection;
					closest.ref = node.triangle;
				}
		}
	}
#ifdef COUNT_HITS
	closest.ref = hits;
#endif
	return closest;

}


bool naive_bvh::any_hit(const ray &ray) {
	auto is = closest_hit(ray);
	if (is.valid())
		return true;
	return false;
}

