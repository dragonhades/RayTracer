// Fall 2018

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <cmath>
#include <numeric>
#include <thread>
#include <mutex>
#include <utility>

#include "PhotonMapping.hpp"

#include "DEBUG.hpp"
#include "CastResult.hpp"
#include "Intersect.hpp"
#include "Math.hpp"
#include "PhongMaterial.hpp"

using namespace glm;
using namespace std;


extern SceneNode * root;
extern std::vector<std::vector<glm::vec3>> photonMap_chart;

glm::vec2 world_2_photonMap(double x, double z){
	return vec2(int(x/obj_w*photon_mapw), int(z/obj_h*photon_maph));
}

// vec2 photonMap_2_world(int x, int z){
// 	return vec2(x*photon_mapw400/photon_distw10000, z*photon_h/photon_dh);
// }

glm::vec2 raydist_2_world(int x, int z){
	return vec2(x* obj_w / photon_distw, z* obj_h / photon_disth);
}

void Photon_Mapping(
		int raydist_x, 
		int raydist_z, 

		// Lighting parameters  
		const glm::mat4 & trans,
		const std::list<Light *> lights
) {

	vec2 world_xz = raydist_2_world(raydist_x, raydist_z);
	vec3 p(world_xz[0], 0, world_xz[1]);

	p = vec3(trans*vec4(p, 1));

	// DPRINTVEC(p);

	for(Light* l : lights) {
		const vec3 & org = l->position;
		const vec3 & dir = p - org;

		Ray ray(org, dir, false);
		CastResult result = intersectScene(ray); 

		if(result.isHit()){

			vec2 map = world_2_photonMap(result.intersection_old.x, result.intersection_old.z);

			SceneNode* snode = (SceneNode*) result.gnode;
			DASSERT(snode!=nullptr, "null");

			if(snode->m_name=="water"){
				double startRefractiveIndex = 1.00;
				double endRefractiveIndex = 1.55;

				const vec3 & refract_dir = glm::normalize(
					get_refract(result.surface_normal, normalize(ray.dir), startRefractiveIndex, endRefractiveIndex));


				if(refract_dir == vec3(0)) DASSERT(false,"should not happen");

				ray = Ray(result.intersection, refract_dir, true);

				const CastResult & result2 = intersectScene(ray); 

				if(result2.isHit()){
					result = result2;

					map = world_2_photonMap(result.intersection_old.x, result.intersection_old.z);

					snode = (SceneNode*) result.gnode;
					DASSERT(snode!=nullptr, "null");
				} else {
					return;
				}
			}

			if(snode->m_name=="ground"){
				// DPRINT("!");
				// we found the map surface
				GeometryNode * gnode = (GeometryNode*) snode;

				PhongMaterial* phong = (PhongMaterial*) gnode->m_material;
				const vec3 & kd = phong->get_kd();
				const vec3 & ks = phong->m_ks;
				const double & p = phong->m_shininess;
				const double & opacity = phong->m_opacity;

				const vec3 & n = result.surface_normal;
				const vec3 & intersection = result.intersection;

				const vec3 & v = -ray.dir;

				const float & light_distance = glm::distance(l->position, intersection);
				const vec3 & I = l->colour 
					/ (l->falloff[0] + l->falloff[1]*light_distance + l->falloff[2]*light_distance*light_distance);

				const vec3 & light_dir = glm::normalize(l->position - intersection);

				const vec3 & r = -glm::reflect(light_dir, n);

				vec3 color = kd*I*std::max(float(0), glm::dot(light_dir, n))
							+ ks*I*std::max(double(0), std::pow(glm::dot(r, v), p) );

				photonMap_chart[map.y][map.x] += color;
			}
		} else {
			return;
		}
	}
}