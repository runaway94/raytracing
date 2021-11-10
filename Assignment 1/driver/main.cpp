#include "primary-hit.h"

#include "libgi/rt.h"
#include "libgi/context.h"
#include "libgi/intersect.h"
#include "libgi/util.h"
#include "libgi/color.h"

#include "libgi/global-context.h"

#include "libgi/wavefront-rt.h"

using namespace glm;
using namespace std;

gi_algorithm::sample_result primary_hit_display::sample_pixel(uint32_t x, uint32_t y, uint32_t samples) {
	sample_result result;

	//Assignment2
	for(int i = 0; i < rc->scene.triangles.size(); ++i) {
		ray ray = cam_ray(rc->scene.camera, x, y);
		triangle_intersection info;
		bool intersection = intersect(rc->scene.triangles[i], rc->scene.vertices.data(), ray, info);
		if(intersection){
			//vector for color! make color on position x,y to col (r,g,b)
			vec3 col;
			col.x = info.beta;
			col.y = info.gamma;
			col.z = info.t;
			result.push_back({col, vec2(0)});
		}
	}

	return result;
}


namespace wf {
	namespace cpu {
		struct store_hitpoint_albedo : public batch_ray_and_intersection_processing_cpu {
			void run() override {
				auto res = rc->resolution();
				float one_over_samples = 1.0f/rc->sppx;
				auto *rt = dynamic_cast<batch_rt*>(rc->scene.batch_rt);
				assert(rt != nullptr);
				cout << res << endl;
				#pragma omp parallel for
				for (int y = 0; y < res.y; ++y)
					for (int x = 0; x < res.x; ++x) {
						vec3 radiance(0);
						for (int sample = 0; sample < rc->sppx; ++sample) {
							triangle_intersection closest = rt->rd.intersections[y*res.x+x];
							if (closest.valid()) {
								diff_geom dg(closest, rc->scene);
								radiance += dg.albedo();
							}
						}
						radiance *= one_over_samples;
						rc->framebuffer.color(x,y) = vec4(radiance, 1);
					}

			}
		};
	}
}

<<<<<<< HEAD
void rt_bench() {
	//create Buffer for rays and intersections with the size of the camera resolution
	buffer<triangle_intersection> triangle_intersections(rc->scene.camera.w, rc->scene.camera.h);
	buffer<ray> rays(rc->scene.camera.w, rc->scene.camera.h);
	
	//init Buffer with Camera rays
	rays.for_each([&](unsigned x, unsigned y) {
		rays(x, y) = cam_ray(rc->scene.camera, x, y);
	});
	
	//calculate closest triangle intersection for each ray
	raii_timer bench_timer("rt_bench");
	rays.for_each([&](unsigned x, unsigned y) {
		triangle_intersections(x, y) = rc->scene.single_rt->closest_hit(rays(x, y));
	});
}

int main(int argc, char **argv)
{

	vec3 a = vec3(1, 1, 0);
	vec3 b = vec3(5, 1, 0);
	vec3 c = vec3(3, 4, 0);

	vertex ver[3];
	ver[0].pos = a;
	ver[1].pos = b;
	ver[2].pos = c;

	triangle tri;
	tri.a = 0;
	tri.b = 1;
	tri.c = 2;

	vec3 o = vec3(0, 0, 3);
	vec3 d = vec3(3, 2, -3);
	ray r = ray(o, d);

	triangle_intersection info;

	bool treffer = intersect(tri, ver, r, info);
	cout << treffer << endl;

	// parse_cmdline(argc, argv);

	// repl_update_checks uc;
	// if (cmdline.script != "") {
	// 	ifstream script(cmdline.script);
	// 	repl(script, uc);
	// }
	// if (cmdline.interact)
	// 	repl(cin, uc);
=======
void primary_hit_display_wf::compute_samples() {
>>>>>>> 58caf0320091bfbec6f8583f6e6ae3f710fff2d3

// 	#pragma omp parallel for
// 	for (int y = 0; y < res.y; ++y)
// 		for (int x = 0; x < res.x; ++x)
// 			rays[y*w+x] = cam_ray(rc->scene.camera, x, y, glm::vec2(rc->rng.uniform_float()-0.5f, rc->rng.uniform_float()-0.5f));
// 
	wf::cpu::batch_cam_ray_setup_cpu().run();
	auto *batch_rt = rc->scene.batch_rt;
	assert(batch_rt != nullptr);
	batch_rt->compute_closest_hit();

<<<<<<< HEAD
	// delete rc->algo;
	// return 0;
=======
	wf::cpu::store_hitpoint_albedo().run();
>>>>>>> 58caf0320091bfbec6f8583f6e6ae3f710fff2d3
}
