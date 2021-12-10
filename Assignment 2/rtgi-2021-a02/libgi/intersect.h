#pragma once
#include <iostream>
#include "rt.h"

struct aabb {
	vec3 min, max;
	aabb() : min(FLT_MAX), max(-FLT_MAX) {}
	void grow(vec3 v) {
		min = glm::min(v, min);
		max = glm::max(v, max);
	}
	void grow(const aabb &other) {
		min = glm::min(other.min, min);
		max = glm::max(other.max, max);
	}
};


inline float det(float A3[3][3]){

	float d =
	A3[0][0] * A3[1][1] * A3[2][2] +
	A3[0][1] * A3[1][2] * A3[2][0] +
	A3[0][2] * A3[1][0] * A3[2][1] -

	A3[0][2] * A3[1][1] * A3[2][0] -
	A3[0][0] * A3[1][2] * A3[2][1] -
	A3[0][1] * A3[1][0] * A3[2][2]; 

	return d;

}

// See Shirley (2nd Ed.), pp. 206. (library or excerpt online)
inline bool intersect(const triangle &t, const vertex *vertices, const ray &ray, triangle_intersection &info) {
		
	vec3 aPos = vertices[t.a].pos;
	vec3 bPos = vertices[t.b].pos;
	vec3 cPos = vertices[t.c].pos;
	vec3 dPos = ray.d;
	vec3 oPos = ray.o;

	// std::cout << "Positions: " << aPos << bPos << cPos << std::endl;
	// std::cout << "Ray: " << oPos << " to " << dPos << std::endl;


	/* Shirley book:
		--						 --  --     --    --       --
		| (ax - bx)	(ax - cx) dx  |  | beta  |    | ax - ox | Cramer's rule          | A1| *A1 -> erste spalte wird durch Ergebnis ersetzt usw.
		| (ay - by) (ay - cy) dy  |  | gamma | =  | ay - oy | =============> beta =  -----
		| (az - bz) (az - cz) dz  |  | t     |    | az - oz |                        | A |
		--						 --  --     --    --       --

		Formel
		--       -- --     --    -- --            
		| a  d  g | | beta  |    | j |              j(ei - hf) + k(gf - di) + l(dh - eg)
		| b  e  h | | gamma | =  | k | ===> beta =  ------------------------------------
		| c  f  i | | t     |	 | l |              a(ei - hf) + b(gf - di) + c(dh - eg) 
		--       -- --     --    -- --   
		 											 i(ak - jb) + h(jc - al) + g(bl - kc)
											gamma = ------------------------------------
											 		 a(ei - hf) + b(gf - di) + c(dh - eg) 
	
													 f(ak - jb) + h(jc - al) + d(bl - kc)
											t =	  -	------------------------------------
											 		 a(ei - hf) + b(gf - di) + c(dh - eg) 	
	*/

	// A 
	float a = aPos.x - bPos.x; float d = aPos.x - cPos.x; float g = dPos.x; 
	float b = aPos.y - bPos.y; float e = aPos.y - cPos.y; float h = dPos.y;
	float c = aPos.z - bPos.z; float f = aPos.z - cPos.z; float i = dPos.z;

	float j = aPos.x - oPos.x;
	float k = aPos.y - oPos.y;
	float l = aPos.z - oPos.z;

	float A[3][3] = {
		{a, d, g},
		{b, e, h},
		{c, f, i}
	};

	float A1[3][3] = {
		{j, d, g},
		{k, e, h},
		{l, f, i}
	};

	float A2[3][3] = {
		{a, j, g},
		{b, k, h},
		{c, l, i}
	};

	float A3[3][3] = {
		{a, d, j},
		{b, e, k},
		{c, f, l}
	};

	float M = det(A);

 	//std::cout << "M: " << M << std::endl;
	
	float beta = (det(A1))/M;
	//std::cout << "Beta: " << beta  << std::endl;

	if(beta < 0 || beta > 1){
		return false;
	}

	float gamma = (det(A2))/M;
	//std::cout << "gamma: " << gamma  << std::endl;
	if(gamma < 0 || gamma > 1 || (beta + gamma) > 1){
		return false;
	}

	float tt = (det(A3))/M;
	//std::cout << "t: " << tt << std::endl;
	if(tt < ray.t_min || tt > ray.t_max){
		return false;
	}

	info.t = tt;
	info.beta = beta;
	info.gamma = gamma;

	return true;
}	

/*
*  Assignment 2.1
*/

inline bool intersect(const aabb &box, const ray &ray, float &is) {
	// todo
	//is = distance

	// int t_dist = 0;

	// float txmin;
	// float txmax;
	// if(ray.d.x >= 0){
	// 	txmin = (box.min.x - ray.o.x)/ray.d.x;
	// 	txmax = (box.max.x - ray.o.x)/ray.d.x;
	// }
	// else {
	// 	txmin = (box.max.x - ray.o.x)/ray.d.x;
	// 	txmax = (box.min.x - ray.o.x)/ray.d.x;
	// }

	// float tymin;
	// float tymax;
	// if(ray.d.y >= 0){
	// 	float tymin = (box.min.y - ray.o.y)/ray.d.y;
	// 	float tymax = (box.max.y - ray.o.y)/ray.d.y;
	// }
	// else {
	// 	tymin = (box.max.y - ray.o.y)/ray.d.y;
	// 	tymax = (box.min.y - ray.o.y)/ray.d.y;
	// }

	// float tzmin;
	// float tzmax;
	// if(ray.d.z >= 0){
	// 	tzmin = (box.min.z - ray.o.z)/ray.d.z;
	// 	tzmax = (box.max.z - ray.o.z)/ray.d.z;
	// }
	// else {
	// 	tzmin = (box.max.z - ray.o.z)/ray.d.z;
	// 	tzmax = (box.min.z - ray.o.z)/ray.d.z;
	// }

	// // std::cout << "txmin: " << txmin << "txmax: " << txmax << std::endl;
	// // std::cout << "tymin: " << tymin << "tymax: " << tymax << std::endl;
	// // std::cout << "tzmin: " << tzmin << "tzmax: " << tzmax << std::endl;
	// if(txmin > tymax || txmin > tzmin || tymin > txmax || tymin > tzmax || tzmin > txmax || tzmin > tymax) {
	// 	return false;
	// }

	// if(txmin < tymin && tymin < tzmin){
	// 	is = txmin;
	// }
	// else if(tymin < txmin && tymin < tzmin){
	// 	is = tymin;
	// }
	// else is = tzmin;
	// return true;


	float t_near = -FLT_MAX;
	float t_far  =  FLT_MAX;

	if (ray.d.x == 0) {
		if (ray.o.x < box.min.x || ray.o.x > box.max.x)
			return false;
	}
	else {
		float t1 = (box.min.x - ray.o.x) / ray.d.x;
		float t2 = (box.max.x - ray.o.x) / ray.d.x;

		if (t1 > t2)	{	float tmp = t1;	t1 = t2; t2 = tmp; 	}

		if (t1 > t_near)	t_near = t1;
		if (t2 < t_far)		t_far = t2;

		if (t_near > t_far)	// box missed
			return false;

		if (t_far < ray.t_min || t_near > ray.t_max)
			return false;
	}

	if (ray.d.y == 0) {
		if (ray.o.y < box.min.y || ray.o.y > box.max.y)
			return false;
	}
	else {
		float t1 = (box.min.y - ray.o.y) / ray.d.y;
		float t2 = (box.max.y - ray.o.y) / ray.d.y;

		if (t1 > t2)	{	float tmp = t1;	t1 = t2; t2 = tmp; 	}

		if (t1 > t_near)	t_near = t1;
		if (t2 < t_far)		t_far = t2;

		if (t_near > t_far)	// box missed
			return false;

		if (t_far < ray.t_min || t_near > ray.t_max)
			return false;
	}

	if (ray.d.z == 0) {
		if (ray.o.z < box.min.z || ray.o.z > box.max.z)
			return false;
	}
	else {
		float t1 = (box.min.z - ray.o.z) / ray.d.z;
		float t2 = (box.max.z - ray.o.z) / ray.d.z;

		if (t1 > t2)	{	float tmp = t1;	t1 = t2; t2 = tmp; }

		if (t1 > t_near)	t_near = t1;
		if (t2 < t_far)		t_far = t2;

		if (t_near > t_far)	// box missed
			return false;

		if (t_far < ray.t_min || t_near > ray.t_max)
			return false;
	}

	is = t_near;
	return true;
}

inline bool intersect2(const aabb &box, const ray &ray, float &is) {
	// todo
	return false;
}


inline bool intersect3(const aabb &box, const ray &ray, float &is) {
	// todo
	return false;
}

inline bool intersect4(const aabb &box, const ray &ray, float &is) {
	// todo
	return false;
}



// // See Shirley (2nd Ed.), pp. 206. (library or excerpt online)
// inline bool intersect(const triangle &t, const vertex *vertices, const ray &ray, triangle_intersection &info) {

// 	vec3 pos = vertices[t.a].pos;
// 	const float a_x = pos.x;
// 	const float a_y = pos.y;
// 	const float a_z = pos.z;

// 	pos = vertices[t.b].pos;
// 	const float &a = a_x - pos.x;
// 	const float &b = a_y - pos.y;
// 	const float &c = a_z - pos.z;
	
// 	pos = vertices[t.c].pos;
// 	const float &d = a_x - pos.x;
// 	const float &e = a_y - pos.y;
// 	const float &f = a_z - pos.z;
	
// 	const float &g = ray.d.x;
// 	const float &h = ray.d.y;
// 	const float &i = ray.d.z;
	
// 	const float &j = a_x - ray.o.x;
// 	const float &k = a_y - ray.o.y;
// 	const float &l = a_z - ray.o.z;

// 	float common1 = e*i - h*f;
// 	float common2 = g*f - d*i;
// 	float common3 = d*h - e*g;
// 	float M 	  = a * common1  +  b * common2  +  c * common3;
// 	float beta 	  = j * common1  +  k * common2  +  l * common3;

// 	common1       = a*k - j*b;
// 	common2       = j*c - a*l;
// 	common3       = b*l - k*c;
// 	float gamma   = i * common1  +  h * common2  +  g * common3;
// 	float tt    = -(f * common1  +  e * common2  +  d * common3);

// 	beta /= M;
// 	gamma /= M;
// 	tt /= M;

// 	if (tt > ray.t_min && tt < ray.t_max)
// 		if (beta > 0 && gamma > 0 && beta + gamma <= 1)
// 		{
// 			info.t = tt;
// 			info.beta = beta;
// 			info.gamma = gamma;
// 			return true;
// 		}

// 	return false;
// }	
