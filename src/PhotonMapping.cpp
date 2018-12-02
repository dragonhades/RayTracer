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

vec2 world_2_photonMapping(double x, double z){
	return vec2(int(x/photon_w*photon_dw), int(z/photon_h*photon_dh));
}

vec2 photonMapping_2_world(int x, int z){
	return vec2(x*photon_w/photon_dw, z*photon_h/photon_dh);
}

void Photon_Mapping(
		int x, 
		int z, 

		// Lighting parameters  
		const SceneNode* target,
		const std::list<Light *> lights
) {

	int w = photon_w;
	int h = photon_h;
	int dw = photon_dw;
	int dh = photon_dh;

	vec2 world = photonMapping_2_world(x, z);
	vec3 p(world[0], 0, world[1]);

	mat4 trans = mat4();
	findNode(root, target->m_name, trans);
	// DPRINTVEC(trans);
	p = vec3(trans*vec4(p, 1));
	// p += vec3(-500, -100, -500);

	// DPRINTVEC(p);

	for(Light* l : lights) {
		const vec3 & org = l->position;
		const vec3 & dir = p - org;

		Ray ray(org, dir, false);
		CastResult result = intersectScene(ray); 

		if(result.isHit()){

			vec2 pt = world_2_photonMapping(result.intersection_old.x, result.intersection_old.z);

			SceneNode* snode = (SceneNode*) result.gnode;
			DASSERT(snode!=nullptr, "null");

			if(snode->m_name=="water"){
				double startRefractiveIndex = 1.00;
				double endRefractiveIndex = 1.55;

				const vec3 & refract_dir = glm::normalize(
					get_refract(result.surface_normal, normalize(ray.dir), startRefractiveIndex, endRefractiveIndex));
				
				// Total internal reflection
				if(refract_dir == vec3(0)) DASSERT(false,"should not happen");

				ray = Ray(result.intersection, refract_dir, true);

				const CastResult & result2 = intersectScene(ray); 

				if(result2.isHit()){
					result = result2;

					pt = world_2_photonMapping(result.intersection_old.x, result.intersection_old.z);

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
				const vec3 & kd = phong->m_kd;
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

				photonMap_chart[pt.y][pt.x] += color; 
			}
		} else {
			return;
		}
	}
}