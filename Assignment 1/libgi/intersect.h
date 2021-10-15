#pragma once

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

// See Shirley (2nd Ed.), pp. 206. (library or excerpt online)
inline bool intersect(const triangle &t, const vertex *vertices, const ray &ray, triangle_intersection &info) {
		
	vec3 aPos = vertices[t.a].pos;
	vec3 bPos = vertices[t.b].pos;
	vec3 cPos = vertices[t.c].pos;
	vec3 dPos = ray.d;
	vec3 oPos = ray.o;

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
											t =		------------------------------------
											 		 a(ei - hf) + b(gf - di) + c(dh - eg) 	
	*/

	// A 
	float a = aPos.x - bPos.x; float d = aPos.x - cPos.x; float g = dPos.x; 
	float b = aPos.y - bPos.y; float e = aPos.y - cPos.y; float h = dPos.y;
	float c = aPos.z - bPos.z; float f = aPos.z - cPos.z; float i = dPos.z;

	float j = aPos.x - oPos.x;
	float k = aPos.y - oPos.y;
	float l = aPos.z - oPos.z;
	
	float M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g); 
	
	float beta = (j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g)) / M;
	if(beta < 0 || beta > 1){
		return false;
	}

	float gamma = (i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c)) / M;
	if(gamma < 0 || gamma > 1 || (beta + gamma) > 1){
		return false;
	}

	float tVar = (f*(a*k - j*b) + h*(j*c - a*l) + d*(b*l - k*c)) / M;
	if(tVar < ray.t_min || tVar > ray.t_max){
		return false;
	}

	info.t = tVar;
	info.beta = beta;
	info.gamma = gamma;

	return true;
}	

