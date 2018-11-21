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

CastResult intersectNode(const SceneNode* node, 
						  const Ray & ray, 
						  const mat4 & trans) 
{
	CastResult result;

	if(node->m_nodeType == NodeType::ConstructiveNode)
	{
		ConstructiveNode* cnode = (ConstructiveNode*) node;

		if(cnode->is_Union()){
			// TODO
		}
		else if(cnode->is_Intersect()){
			// TODO
		}
		else if(cnode->is_Difference()){
			result = intersectNode(cnode.leftNode(), ray, trans*cnode->get_transform());
			if(result.isHit()){
				result.type = HitType::CSG;
				result.solidNode = (SolidNode*) gnode;
				result.parentTrans = trans;
				return result;
			} else {
				result = intersectNode(cnode.rightNode(), ray, trans*cnode->get_transform());
				if(result.isHit()){
					Ray ray_inside(result.intersection, ray.dir, true); // is inside
					result = intersectNode(cnode.rightNode(), ray, trans*cnode->get_transform());
				} else {
					return CastResult();
				}
			}
		}
	}
//----------------------------------------------------------------------------------------//
	else if(node->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode* gnode = (GeometryNode*) node;

		Ray ray_copy = ray;
		ray_copy.setTransform(inverse(trans*gnode->get_transform()));
		result = gnode->m_primitive->intersect(ray_copy);

		if(result.isHit()){

#ifndef DRAW_BBOX
			if(result.type == HitType::BBox){
				Mesh* mesh = (Mesh*) gnode->m_primitive;
				result = mesh->intersectTriangles(ray_copy);
				if(!result.isHit()) goto _NO_HIT_;
			}
#endif
			result.solidNode = (SolidNode*) gnode;
			result.parentTrans = trans;
		}
	}
_NO_HIT_:
	return result;
}

CastResult intersectSceneRecurse(const SceneNode* node, 
						  const Ray & ray, 
						  const mat4 & trans) 
{
	double t_min = INT_MAX;
	CastResult result_min;
		
	CastResult result = intersectNode(node, ray, trans);

	if(result.isHit() && result.t < t_min) {
		t_min = result.t;
		result_min = result;
	}

_NO_HIT_:
	for (SceneNode* child : node->children){
		const CastResult & result = 
			intersectSceneRecurse(child, ray, trans*node->get_transform());
		if(result.isHit() && result.t < t_min) {
			t_min = result.t;
			result_min = result;
		}
	}
	if(t_min != INT_MAX) return result_min;
	else return CastResult();
}

CastResult intersectNodeRecurse(const SceneNode* target,
						  const SceneNode* node,
						  const Ray & ray, 
						  const mat4 & trans)
{
	CastResult result;
	if(target == node) {
		result = intersectSceneRecurse(node, ray, trans);
	} else {
		for(SceneNode* child : node->children){
			result = intersectNodeRecurse(target, child, ray, trans*node->get_transform());
			if(result.isHit()) return result;
		}
	}
	return result;
}

CastResult intersectScene(const Ray & ray, 
						const SceneNode* target=nullptr
){
	CastResult result;
	if(target){
		intersectNodeRecurse(target, root, ray, mat4());
	} else {
		result = intersectSceneRecurse(root, ray, mat4());
	}
		if(result.isHit()){
			 result.transform();
			 if(ray.inside_shape) result.surface_normal = -result.surface_normal;
			 return result;
		} else {
			return CastResult();
		}
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

		GeometryNode* gnode;
		if(result.type == HitType::CSG){
			ConstructiveNode * cnode = (ConstructiveNode*) result.solidNode;
			gnode = (GeometryNode*) cnode->get_SceneNode();
		} else {
			gnode = (GeometryNode*) result.solidNode;
		}

		DASSERT(gnode != nullptr, "gnode is Null.");

		DASSERT(result.solidNode != nullptr, "null");

		/** Phong illumination **/
		PhongMaterial* phong = (PhongMaterial*) gnode->m_material;
		const vec3 & kd = phong->m_kd;
		const vec3 & ks = phong->m_ks;
		const double & p = phong->m_shininess;
		const double & opacity = phong->m_opacity;

		const vec3 & n = result.surface_normal;
		const vec3 & intersection = result.intersection;

		const vec3 & v = -ray.dir;

		// ambient = Kd * Ia
		vec3 color = ambient*kd;

//--------------------------  Lighting  -----------------------------//
 
	#if defined USE_LIGHT_FRONT && defined USE_LIGHT_BACK
		for(Light* l : lights)
	#else
	#ifdef USE_LIGHT_FRONT
		Light* l = lights.front();
	#endif
	#ifdef USE_LIGHT_BACK
		Light* l = lights.back();
	#else
		goto _SKIP_LIGHTING_;
	#endif
	#endif //both


		{
			const float & light_distance = glm::distance(l->position, intersection);
			const vec3 & I = l->colour 
				/ (l->falloff[0] + l->falloff[1]*light_distance + l->falloff[2]*light_distance*light_distance);

			const vec3 & light_dir = glm::normalize(l->position - intersection);

			const vec3 & r = -glm::reflect(light_dir, n);
			// const vec3 & h = (v + light_dir) / absVec3(v + light_dir);

			Ray shadowRay(intersection, light_dir, ray.inside_shape);
			const CastResult & shadowRay_result = intersectScene(shadowRay);

			vec3 color_self;

			if(shadowRay_result.isHit()) continue;	// discard;
			color_self = kd*I*std::max(float(0), glm::dot(light_dir, n))
						+ ks*I*std::max(double(0), std::pow(glm::dot(r, v), p) );
						// + ks*std::pow(glm::dot(h, n), p)*I;

		// TODO 

			//The intersectScene is repeated during following call...  
			// Get rid of it

		// TODO 

			// seg fault

			// if(shadowRay_result.isHit() && 
			// 	glm::distance(shadowRay_result.intersection, intersection) <= light_distance)
			// {
			// 	PhongMaterial* material = (PhongMaterial*) shadowRay_result.geoNode->m_material;
			// 	if(material->m_opacity != 1){
			// 		color_self = 0.5*ks*shading(Ray(shadowRay_result.intersection, shadowRay.dir), 
			// 			1, x, y, w, h,root, ambient, lights);
			// 	}
			// } else {
			// 	color_self = kd*I*std::max(float(0), glm::dot(light_dir, n))
			// 			+ ks*I*std::max(double(0), std::pow(glm::dot(r, v), p) );
			// 			// + ks*std::pow(glm::dot(h, n), p)*I;
			// }

#ifdef REFRACTION
			color += opacity*color_self;
#else
			color += color_self;
#endif
		} // for lights
//--------------------------  Lighting  -----------------------------//
_SKIP_LIGHTING_:

		if(recursionDepth < MAX_SHADE_RECURSION){
#ifdef REFLECTION
			// if(p>5){
				const Ray & ray_reflection = Ray(intersection, glm::reflect(ray.dir, n), ray.inside_shape);
				const vec3 & reflect_color = 
					0.5 * ks * shading(ray_reflection, recursionDepth + 1, node, x, y, w, h, ambient, lights);
				color += reflect_color;
			// }

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
				if(ray.inside_shape){
					startRefractiveIndex = 1.33;
					endRefractiveIndex = 1.00;
				} else {
					startRefractiveIndex = 1.00;
					endRefractiveIndex = 1.33;
				}
				
				const vec3 & refract_dir = get_refract(n, ray.dir, startRefractiveIndex, endRefractiveIndex);
				// const Ray & ray_transmition = Ray(intersection, ray.dir);
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
#endif
		}

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