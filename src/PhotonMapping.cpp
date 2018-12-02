// Fall 2018

#include <glm/ext.hpp>

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

using namespace glm;
using namespace std;


extern SceneNode * root;
extern std::vector<std::vector<glm::vec3>> photonMap_chart;


void Photon_Mapping(
		int x, 
		int z, 

		// Lighting parameters  
		const SceneNode* target,
		const std::list<Light *> lights,
		vector<vector<vec3>> & photon_mapping
) {
	vec3 p(x, 0, z);
	
	for(Light* l : lights) {
		const vec3 & org = l->position;
		const vec3 & dir = p - org;

		const Ray ray(org, dir);
		const CastResult & result = intersectScene(ray, root); 

		if(result.isHit()){

			SceneNode* snode = (SceneNode*) result.gnode;
			DASSERT(snode!=nullptr, "null");

			if(target == snode){
			// 				const float & light_distance = glm::distance(l->position, intersection);
			// const vec3 & I = l->colour 
			// 	/ (l->falloff[0] + l->falloff[1]*light_distance + l->falloff[2]*light_distance*light_distance);

			// const vec3 & light_dir = glm::normalize(l->position - intersection);

			// const vec3 & r = -glm::reflect(light_dir, n);
			// // const vec3 & h = (v + light_dir) / absVec3(v + light_dir);

			// Ray shadowRay(intersection, light_dir, ray.inside_shape);
			// CastResult shadowRay_result = intersectScene(shadowRay);

			// vec3 color_self;

				
			// color_self = kd*I*std::max(float(0), glm::dot(light_dir, n))
			// 			+ ks*I*std::max(double(0), std::pow(glm::dot(r, v), p) );
			}

			if(snode->m_name=="water"){
				double startRefractiveIndex = 1.00;
				double endRefractiveIndex = 1.55;

				const vec3 & refract_dir = glm::normalize(
					get_refract(result.surface_normal, normalize(ray.dir), startRefractiveIndex, endRefractiveIndex));
				
				// Total internal reflection
				if(refract_dir == vec3(0)) DASSERT(false,"should not happen");

				const Ray & ray_transmition = Ray(result.intersection, refract_dir, true);
			}
		} else {

		}
	}
}

glm::vec3 get_PhotonColor(int x, int z){


}