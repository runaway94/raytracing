#pragma once

#include <glm/glm.hpp>
#include <cmath>

/*
 *  vector math things
 *
 */

//! compute clamped (to zero) dot product
inline float cdot(const vec3 &a, const vec3 &b) {
	float x = a.x*b.x + a.y*b.y + a.z*b.z;
	return x < 0.0f ? 0.0f : x;
}

//! compute absolute-value of dot product
inline float absdot(const vec3 &a, const vec3 &b) {
	float x = a.x*b.x + a.y*b.y + a.z*b.z;
	return x < 0.0f ? -x : x;
}


/*
 *  floating point things
 *
 */

//! move each vector component of \c from the minimal amount possible in the direction of \c to
inline vec3 nextafter(const vec3 &from, const vec3 &d) {
	return vec3(std::nextafter(from.x, d.x > 0 ? from.x+1 : from.x-1),
				std::nextafter(from.y, d.y > 0 ? from.y+1 : from.y-1),
				std::nextafter(from.z, d.z > 0 ? from.z+1 : from.z-1));
}


/*
 *  brdf helpers
 *  starting here, many things will probably originate from niho's code
 */

inline float fresnel_dielectric(float cos_wi, float ior_medium, float ior_material) {
    // check if entering or leaving material
    const float n1 = cos_wi < 0.0f ? ior_material : ior_medium;
    const float n2 = cos_wi < 0.0f ? ior_medium : ior_material;
    cos_wi = glm::clamp(glm::abs(cos_wi), 0.0f, 1.0f);
	// todo fresnel term for dielectrics.
	// make sure to handle internal reflection
	return 0.0f;
}


/* 
 * trigonometric helper functions
 *
 */
inline float cos_theta(float cos_t) {
    return cos_t;
}
inline float cos2_theta(float cos_t) {
    return cos_t*cos_t;
}
inline float abs_cos_theta(float cos_t) {
    return cos_t < 0.0f ? -cos_t : cos_t;
}
inline float sin2_theta(float cos_t) {
	float res = 1.0f - cos_t*cos_t;
    return res < 0.0f ? 0.0f : res;
}
inline float sin_theta(float cos_t) {
    return sqrtf(sin2_theta(cos_t));
}
inline float tan_theta(float cos_t) {
    return sin_theta(cos_t) / cos_theta(cos_t);
}
inline float tan2_theta(float cos_t) {
    return sin2_theta(cos_t) / cos2_theta(cos_t);
}
inline float cos_theta(const glm::vec3& N, const glm::vec3& w) {
    return glm::dot(N, w);
}
inline float abs_cos_theta(const glm::vec3& N, const glm::vec3& w) {
	float x = cos_theta(N, w);
    return x < 0.0f ? -x : x;
}
inline float cos2_theta(const glm::vec3& N, const glm::vec3& w) {
	float x = cos_theta(N, w);
    return x*x;
}
inline float sin2_theta(const glm::vec3& N, const glm::vec3& w) {
	float x = cos_theta(N, w);
	float y = 1.0f - x*x;
	return y < 0.0f ? 0.0f : y;
}
inline float sin_theta(const glm::vec3& N, const glm::vec3& w) {
    return sqrtf(sin2_theta(N, w));
}
inline float tan_theta(const glm::vec3& N, const glm::vec3& w) {
    return sin_theta(N, w) / cos_theta(N, w);
}
inline float tan2_theta(const glm::vec3& N, const glm::vec3& w) {
    return sin2_theta(N, w) / cos2_theta(N, w);
}

/*
 *  spherical geometry helpers
 */

inline bool same_hemisphere(const vec3 &N, const vec3 &v) {
    return glm::dot(N, v) > 0;
}

inline vec2 to_spherical(const vec3 &w) {
    const float theta = acosf(w.y);
    const float phi = atan2f(w.z, w.x);
    return vec2(glm::clamp(theta, 0.f, pi), phi < 0.f ? phi + 2.0f * pi : phi);
}

inline vec3 to_cartesian(const vec2 &w) {
    const float sin_t = sinf(w.x);
    return vec3(sin_t * cosf(w.y), sin_t * sinf(w.y), cosf(w.x));
}


/*! align vector v with given axis (e.g. to transform a tangent space sample along a world normal)
 *  \attention parameter-order inverted with regards to niho's code
 */
inline vec3 align(const vec3& v, const vec3& axis) {
    const float s = copysign(1.f, axis.z);
    const vec3 w = vec3(v.x, v.y, v.z * s);
    const vec3 h = vec3(axis.x, axis.y, axis.z + s);
    const float k = dot(w, h) / (1.f + (axis.z < 0 ? -axis.z : axis.z));
    return k * h - w;
}

inline void flip_normals_to_ray(diff_geom &dg, const ray &ray) {
	// todo: all computations rely on the shading normal for now,
	// this is not exactly correct.
	if (same_hemisphere(ray.d, dg.ns)) {
		dg.ng *= -1;
		dg.ns *= -1;
	}
}
