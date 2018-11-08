// Fall 2018

#include <glm/ext.hpp>

#include <cmath>
#include <numeric>
#include <thread>
#include <mutex>

#include "RayTracer.hpp"
#include "Ray.hpp"
#include "DEBUG.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Primitive.hpp"
#include "Mesh.hpp"

using namespace glm;
using namespace std;

const uint nearPlane = 6;
const uint farPlane = 20;

float absVec3(const vec3 & vec){
	float value = 0.0;
	for(int i = 0; i < 3; i++) value += vec[i] * vec[i];
	return sqrt(value);
}

Ray getPrimaryRay(const float x, const float y, const int width, const int height, 
	const int fov, const int z_near) {

    const float side = -2.0f * tan((fov*PI/180.0) / 2.0f);
    
    vec3 P = vec3( z_near * (x / width - 0.5f) * side * width / height, 
                z_near * -(y / height - 0.5f) * side, 
                z_near);

    return Ray(vec3(0), glm::normalize(P));
}


CastResult intersectScene(const SceneNode* node, 
						  const Ray & ray, 
						  const mat4 invtrans=mat4(), 
						  const mat4 trans=mat4() ) 
{
	double t_min = INT_MAX;
	CastResult result_min;
		
	if(node->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode* gnode = (GeometryNode*) node;
		const mat4 & invT = gnode->get_inverse() * invtrans;

		Ray ray_copy = ray;
		ray_copy.setTransform(invT);
		CastResult result = gnode->m_primitive->intersect(ray_copy);

		if(result.isHit()){

#ifndef DRAW_BBOX
			if(result.type == HitType::BBox){
				Mesh* mesh = (Mesh*) gnode->m_primitive;
				result = mesh->intersectTriangles(ray_copy);
				if(!result.isHit()) goto _NO_HIT_;
			}
#endif
			result.geoNode = gnode;
			result.parentTrans = trans;

			if(result.t < t_min) {
				t_min = result.t;
				result_min = result;
			}
		}
	}
_NO_HIT_:
	for (SceneNode* child : node->children){
		const CastResult & result = 
			intersectScene(child, ray, node->get_inverse()*invtrans, trans*node->get_transform());
		if(result.isHit() && result.t < t_min) {
			t_min = result.t;
			result_min = result;
		}
	}
	if(t_min != INT_MAX) return result_min;
	else return CastResult();
}


glm::vec3 Render_Pixel(
		int x, 
		int y, 
		int w, 
		int h,

		// What to render  
		const SceneNode * root,

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
   
    vec3 color = shading(ray, 1, root, ambient, lights);
    if(color == vec3(0)){
		// background color
		color = vec3(y / float(h), y*0.1 / float(h), y*0.4 / float(h));
    }
    return color;
}

// http://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/reflection_refraction.pdf
glm::vec3 shading(
		const Ray & ray, 
		const int recursionDepth,

		// What to render  
		const SceneNode * root,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

	/** intersect(scene, ray) **/
	const CastResult & result = intersectScene(root, ray);

	if(result.isHit()){

		GeometryNode* gnode = result.geoNode;

		DASSERT(gnode != nullptr, "gnode is Null.");

		DASSERT(result.geoNode != nullptr, "null");

		const CastResult & result_Worldsp = result.transform();

		/** Phong illumination **/
		PhongMaterial* phong = (PhongMaterial*) gnode->m_material;
		const vec3 & kd = phong->m_kd;
		const vec3 & ks = phong->m_ks;
		const double & p = phong->m_shininess;

		const vec3 & n = result_Worldsp.surface_normal;
		const vec3 & intersection = result_Worldsp.intersection;

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
			const float & distance = glm::distance(l->position, intersection);
			const vec3 & I = l->colour 
				/ (l->falloff[0] + l->falloff[1]*distance + l->falloff[2]*distance*distance);

			const vec3 & light_dir = glm::normalize(l->position - intersection);

			const vec3 & r = -glm::reflect(light_dir, n);
			// const vec3 & h = (v + light_dir) / absVec3(v + light_dir);

			Ray shadowRay(intersection, light_dir);
			const CastResult & shadowRay_result = intersectScene(root, shadowRay);

			if(shadowRay_result.isHit()) continue;	// discard;

			color += kd*I*std::max(float(0), glm::dot(light_dir, n))
					+ ks*I*std::max(double(0), std::pow(glm::dot(r, v), p) );
					// + ks*std::pow(glm::dot(h, n), p)*I;

		} // for lights
//--------------------------  Lighting  -----------------------------//
_SKIP_LIGHTING_:

#ifdef REFLECTION
		if(recursionDepth < MAX_SHADE_RECURSION){
			// if(shiny)
			const Ray & ray_reflection = Ray(intersection, glm::reflect(ray.dir, n));
			const vec3 & reflect_color = 
				0.5 * ks * shading(ray_reflection, recursionDepth + 1, root, ambient, lights);
			color += reflect_color;
		}

#endif		
		return color;

	} else {	// if not hit

		// background color
		// vec3 color(y / float(h), y*0.1 / float(h), y*0.4 / float(h));
		return vec3(0, 0, 0);
	}
}