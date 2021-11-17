#pragma once

#include "rt.h"

#include <glm/glm.hpp>
#include <string>
#include <tuple>


struct texture;
    
inline float roughness_from_exponent(float exponent) {
	return sqrtf(2.f / (exponent + 2.f));
}
inline float exponent_from_roughness(float roughness) {
	return 2 / (roughness * roughness) - 2;
}


struct brdf {
	
	virtual vec3 f(const diff_geom &geom, const vec3 &w_o, const vec3 &w_i) = 0;
};

//! Specular BRDFs can be layered onto non-specular ones
struct specular_brdf : public brdf {
	bool coat = false;
};

struct layered_brdf : public brdf {
	specular_brdf *coat;
	brdf *base;
	layered_brdf(specular_brdf *coat, brdf *base) : coat(coat), base(base) { coat->coat = true; }
	
	vec3 f(const diff_geom &geom, const vec3 &w_o, const vec3 &w_i) override;
};

struct lambertian_reflection : public brdf {
	vec3 f(const diff_geom &geom, const vec3 &w_o, const vec3 &w_i) override;
};

struct phong_specular_reflection : public specular_brdf {
	vec3 f(const diff_geom &geom, const vec3 &w_o, const vec3 &w_i) override;
};

struct gtr2_reflection : public specular_brdf {
	vec3 f(const diff_geom &geom, const vec3 &w_o, const vec3 &w_i) override;
};

brdf *new_brdf(const std::string name, scene &scene);

struct material {
	std::string name;
	vec3 albedo = vec3(0);
	vec3 emissive = vec3(0);
	texture *albedo_tex = nullptr;
	float ior = 1.3f, roughness = 0.1f;
	struct brdf *brdf = nullptr;
};


