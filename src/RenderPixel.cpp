// Fall 2018

#include <glm/ext.hpp>

#include <cmath>
#include <numeric>
#include <thread>
#include <mutex>
#include <utility>

#include "RayTracer.hpp"
#include "Ray.hpp"
#include "DEBUG.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "ConstructiveNode.hpp"
#include "SolidNode.hpp"
#include "PhongMaterial.hpp"
#include "Primitive.hpp"
#include "Mesh.hpp"
#include "Math.hpp"
#include "Intersect.hpp"
#include "PhotonMapping.hpp"

using namespace glm;
using namespace std;

const uint nearPlane = 6;
const uint farPlane = 20;

Ray getPrimaryRay(const float x, const float y, const int width, const int height, 
	const int fov, const int z_near) {

    const float side = -2.0f * tan((fov*PI/180.0) / 2.0f);
    
    vec3 P = vec3( z_near * (x / width - 0.5f) * side * width / height, 
                z_near * -(y / height - 0.5f) * side, 
                z_near);

    return Ray(vec3(0), glm::normalize(P), false);
}

glm::vec3 Render_Pixel(
		int x, 
		int y, 
		int w, 
		int h,

		// Viewing parameters
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

	// ray = (eye, pixel-eye)
#ifdef SSAA
	Ray ray = getPrimaryRay(x, y, w, h, fovy, -nearPlane);
#else
	Ray ray = getPrimaryRay(x+0.5f, y+0.5f, w, h, fovy, -nearPlane);
#endif	
   
    return shading(ray, 1, root, x, y, w, h, ambient, lights);
}

// http://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/reflection_refraction.pdf
glm::vec3 shading(
		const Ray & ray, 
		const int recursionDepth,

		// What to render  
		const SceneNode * node,
		
		int x, 
		int y, 
		int w, 
		int h,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

	/** intersect(scene, ray) **/
	const CastResult & result = intersectScene(ray);

	if(result.isHit()){

		GeometryNode* gnode = result.gnode;
		DASSERT(gnode != nullptr, "gnode is Null.");

		/** Phong illumination **/
		PhongMaterial* phong = (PhongMaterial*) gnode->m_material;
		vec3 kd;
		if(phong->m_type == MaterialType::Texture)
			kd = phong->get_kd(result.intersection_old.x, result.intersection_old.z);
		else 
			kd = phong->get_kd();
		const vec3 & ks = phong->m_ks;
		const double & p = phong->m_shininess;
		const double & opacity = phong->m_opacity;

		const vec3 & n = result.surface_normal;
		const vec3 & intersection = result.intersection;

		const vec3 & v = -ray.dir;

		// ambient = Kd * Ia
		vec3 color = ambient*kd;

#ifdef PHOTON_MAPPING
		if(gnode->m_name == "ground"){
			// DPRINTVEC(vec2(result.intersection_old.x, result.intersection_old.z));
			vec2 coord = world_2_photonMap(result.intersection_old.x, result.intersection_old.z);
			vec3 c =  photonMap_chart[int(coord[1])][int(coord[0])] + color;
			// DPRINTVEC(c);
			return c;
		}
#endif

//--------------------------  Lighting  -----------------------------//

#ifdef LIGHT_TURN_ON

		for(Light* l : lights) {

		// Light* l = lights.front();
		// for(int i=0; i<1; i++) {


			const float & light_distance = glm::distance(l->position, intersection);
			const vec3 & I = l->colour 
				/ (l->falloff[0] + l->falloff[1]*light_distance + l->falloff[2]*light_distance*light_distance);

			const vec3 & light_dir = glm::normalize(l->position - intersection);

			const vec3 & r = -glm::reflect(light_dir, n);
			// const vec3 & h = (v + light_dir) / absVec3(v + light_dir);

			Ray shadowRay(intersection, light_dir, ray.inside_shape);
			CastResult shadowRay_result = intersectScene(shadowRay);

			vec3 color_self;

_shadow_ray_loop_:
			if(shadowRay_result.isHit()){
				// PhongMaterial* mat = (PhongMaterial*) shadowRay_result.gnode->m_material;
				// if(mat){
				// 	if(mat->m_opacity != 0.0) // if object is not transparent;
				// 		continue;	// discard;
				// 	else {
				// 		shadowRay = Ray(shadowRay_result.intersection, light_dir, ray.inside_shape);
				// 		shadowRay_result = intersectScene(shadowRay);
				// 		goto _shadow_ray_loop_;
				// 	}
				// } else {
					continue;
				// }
			} 
				
			color_self = kd*I*std::max(float(0), glm::dot(light_dir, n))
						+ ks*I*std::max(double(0), std::pow(glm::dot(r, v), p) );
						// + ks*std::pow(glm::dot(h, n), p)*I;


 #ifdef REFRACTION
			color += opacity*color_self;
 #else
			color += color_self;
 #endif
		} // for lights
//--------------------------  Lighting  -----------------------------//

		if(recursionDepth < MAX_SHADE_RECURSION){
 #ifdef REFLECTION
			// if(p>5){
				const Ray & ray_reflection = Ray(intersection, glm::reflect(ray.dir, n), ray.inside_shape);
				const vec3 & reflect_color = 
					0.5 * ks * shading(ray_reflection, recursionDepth + 1, node, x, y, w, h, ambient, lights);
				color += reflect_color;
			// }
 #endif
 #ifdef REFRACTION
			if(opacity != 1){
				/*
					Air 1.0
					Glass 1.55
					Water 1.33
					Diamond 2.42 
				*/

				double startRefractiveIndex;
				double endRefractiveIndex;
				if( ! ray.inside_shape){
					startRefractiveIndex = 1.00;
					endRefractiveIndex = 1.55;
				} else {
					startRefractiveIndex = 1.55;
					endRefractiveIndex = 1.00;
				}
				// DPRINTVEC(n);
				const vec3 & refract_dir = glm::normalize(get_refract(n, normalize(ray.dir), startRefractiveIndex, endRefractiveIndex));
				
				// Total internal reflection
				//  But we've done it already
				if(refract_dir == vec3(0)) return color; 
				

				const Ray & ray_transmition = Ray(intersection, refract_dir, !ray.inside_shape);

				vec3 refract_color;
				if(ray.inside_shape){
					refract_color = 0.8 * ks*
						shading(ray_transmition, recursionDepth + 1, root, x, y, w, h, ambient, lights);
				} else {
					refract_color = 0.8 * ks*
						shading(ray_transmition, recursionDepth + 1, gnode, x, y, w, h, ambient, lights);	
				}
				// DPRINTVEC(refract_color);
				color += (1.0-opacity)*refract_color;
			}
		
 #endif	
		}
#endif
		return color;

	} else {	// if not hit

		// background color
		// return vec3(0.0*y / float(h), y*0.1 / float(h), y*0.4 / float(h));	// blue shading
		// return vec3(y / float(h), y*0.1 / float(h), y*0.3 / float(h));	// red shading
		return vec3(0.5*y / float(h));	// white shading

		// return vec3(0.3, 0.02, 0.05);	// red

		// return vec3(y/(h/10) % 2 == 0 ? 0 : 0.4);

		// return vec3(0);
	}
}