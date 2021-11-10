#pragma once

#include "rt.h"
#include "camera.h"
#include <iostream>

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

 	std::cout << "M: " << M << std::endl;
	
	float beta = (det(A1))/M;
	std::cout << "Beta: " << beta  << std::endl;

	if(beta < 0 || beta > 1){
		return false;
	}

	float gamma = (det(A2))/M;
	std::cout << "gamma: " << gamma  << std::endl;
	if(gamma < 0 || gamma > 1 || (beta + gamma) > 1){
		return false;
	}

	float tt = (det(A3))/M;
	std::cout << "t: " << tt << std::endl;
	if(tt < ray.t_min || tt > ray.t_max){
		return false;
	}

	info.t = tt;
	info.beta = beta;
	info.gamma = gamma;

	return true;
}	

