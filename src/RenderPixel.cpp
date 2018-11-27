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
						  const mat4 & trans);

int csgPenetrateRecurse(const SceneNode* ntarget, float t_end, Ray ray, const mat4 & trans, int acc=0, float t_target_acc=0){
	CastResult result_target = intersectNode(ntarget, ray, trans);

	if(!result_target.isHit()){
		return acc;
	}

	float t_target = result_target.t + t_target_acc;

	if(t_target>= t_end){
		return acc;
	}
	else {
		result_target.transform();
		ray.org = result_target.intersection;
		return csgPenetrateRecurse(ntarget, t_end, ray, trans, acc+1, t_target);
	}
}

int csgPenetrate(const SceneNode* ntarget, const SceneNode* nend, const Ray & ray, const mat4 & trans){
	CastResult result_end = intersectNode(nend, ray, trans);
	float t_end = result_end.t;

	return csgPenetrateRecurse(ntarget, t_end, ray, trans, 0);
}


CastResult intersectNode(const SceneNode* node, 
						  const Ray & ray, 
						  const mat4 & trans) 
{
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

			const mat4 & new_trans = trans*cnode->get_transform();
			CastResult result_right_min = intersectNode(cnode->rightNode(), ray, new_trans);
			if(result_right_min.isHit()){

				// return CastResult();
				
				CastResult result_left_min = intersectNode(cnode->leftNode(), ray, new_trans);

				if(!result_left_min.isHit()) return CastResult();

				CastResult result_left_min_copy = result_left_min;
				result_left_min_copy.transform();
				Ray ray_inside_left(result_left_min_copy.intersection, ray.dir, true);
				CastResult result_left_max = intersectNode(cnode->leftNode(), ray_inside_left, new_trans);

				if(!result_left_max.isHit()){
					float t_left = result_left_min.t;
					float t_right = result_right_min.t;
					if(t_left <= t_right){
						return CastResult();
					} else {
						CastResult rtv = result_right_min;
						rtv.gnode = result_left_min.gnode;
						rtv.surface_normal = -rtv.surface_normal;
						rtv.type = HitType::CSG;
						rtv.parentTrans = new_trans;
						return rtv;
					}
				}

				CastResult result_right_min_copy = result_right_min;
				result_right_min_copy.transform();

				Ray ray_inside_right(result_right_min_copy.intersection, ray.dir, true); 
				CastResult result_right_max = intersectNode(cnode->rightNode(), ray_inside_right, new_trans);



				float t_left_min = result_left_min.t;
				float t_right_min = result_right_min.t;
				float t_left_max = result_left_max.t + result_left_min.t;
				float t_right_max = result_right_max.t + result_right_min.t;

				if(!result_right_max.isHit()){
					t_right_min = INT_MIN;
				}

				CastResult rtv;
				if(t_left_min < t_right_min || t_left_min > t_right_max){
					rtv = result_left_min;
				} else if(t_right_max < t_left_max){
					rtv = result_right_max;
					rtv.t = t_right_max;
					rtv.gnode = result_left_min.gnode;
					rtv.surface_normal = -rtv.surface_normal;
				} else {
					return CastResult();
				}

				rtv.type = HitType::CSG;
				rtv.parentTrans = new_trans;
				return rtv;
			} else {
				// test the supposely exposed left shape surface
				return intersectNode(cnode->leftNode(), ray, new_trans);
			}
		}
	}
//----------------------------------------------------------------------------------------//
	else if(node->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode* gnode = (GeometryNode*) node;

		Ray ray_copy = ray;
		ray_copy.setTransform(inverse(trans*gnode->get_transform()));
		CastResult result = gnode->m_primitive->intersect(ray_copy);

		if(result.isHit()){

#ifndef DRAW_BBOX
			if(result.type == HitType::BBox){
				Mesh* mesh = (Mesh*) gnode->m_primitive;
				result = mesh->intersectTriangles(ray_copy);
			}
#endif
			result.gnode = gnode;
			result.parentTrans = trans;
			return result;
		}
	}
	return CastResult();
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

	const ConstructiveNode* cnode = dynamic_cast<const ConstructiveNode*>(node);
	if(cnode){
		goto _SKIP_LOOP_;
	}

	for (SceneNode* child : node->children){
		const CastResult & result = 
			intersectSceneRecurse(child, ray, trans*node->get_transform());
		if(result.isHit() && result.t < t_min) {
			t_min = result.t;
			result_min = result;
		}
	}
_SKIP_LOOP_:
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

		GeometryNode* gnode = result.gnode;

		DASSERT(gnode != nullptr, "gnode is Null.");

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