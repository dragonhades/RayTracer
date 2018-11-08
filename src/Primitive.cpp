// Fall 2018

#include "Primitive.hpp"
#include "polyroots.hpp"
#include "Ray.hpp"
#include "GeometryNode.hpp"

#include "DEBUG.hpp"

#include <vector>
#include <algorithm>

using namespace glm;
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

// #define EPSILON std::numeric_limits<double>::epsilon()
#define EPSILON 0.1

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

CastResult Sphere::intersect(const Ray & ray) {
	NonhierSphere sphere(vec3(0,0,0), 1);
	return sphere.intersect(ray);
}

Cube::~Cube()
{
}

CastResult Cube::intersect(const Ray & ray) {
	NonhierBox box(vec3(0,0,0), 1);
	return box.intersect(ray);
}

NonhierSphere::~NonhierSphere()
{
}

CastResult NonhierSphere::intersect(const Ray & ray) {
	// ray => org + t(dir)
	// sphere => (x^2 + y^2 + z^2) - r^2 = 0
	// Intersect => orgX^2 + 2*orgX*t(dirX) + t^2*(dirX)^2 +
	//              orgY^2 + 2*orgY*t(dirY) + t^2*(dirY)^2 +
	//              orgZ^2 + 2*orgZ*t(dirZ) + t^2*(dirZ)^2 -
	//              r^2 = 0
	//           => (dirX^2 + dirY^2 + dirZ^2) * t^2 +
	//              2*(orgX*dirX + orgY*dirY + orgZ*dirZ) * t + 
	//              orgX^2 + orgY^2 + orgZ^2 - r^2 
	//              = 0

	vec3 origin = ray.org - m_pos;
	const vec3 & direction = ray.dir;

	double A = glm::dot(direction, direction);
	double B = 2*glm::dot(vec3(origin), direction);
	double C = glm::dot(origin, origin) - m_radius*m_radius;

	double roots[2];

	size_t rtv = quadraticRoots(A, B, C, roots);

	if(rtv == 0) {
		return CastResult();
	} else if(rtv == 1) {
		if(roots[0] < EPSILON) return CastResult();
		else {
			CastResult result;
			result.type = HitType::Primitive;
			result.intersection = origin + (float)(roots[0])*direction;
			result.surface_normal = result.intersection;	// - (0,0,0)

			result.t = roots[0];
			result.pos = m_pos;
			return result;
		}
	} else {//rtv == 2
		if(roots[0] < EPSILON && roots[1] < EPSILON) return CastResult();
		
		double t;
		if(roots[0] < EPSILON) t = roots[1];
		else if(roots[1] < EPSILON) t = roots[0];
		else 
			t = std::min(roots[0], roots[1]);

		CastResult result;
		result.type = HitType::Primitive;
		result.intersection = origin + (float)t*direction;
		result.surface_normal = result.intersection;	// - (0,0,0)

		result.t = t;
		result.pos = m_pos;
		return result;
	}
};


NonhierBox::~NonhierBox()
{
}

  // glm::vec3 m_pos;
  // double m_size;
CastResult NonhierBox::intersect(const Ray & ray) {
	// ray => org + t(dir)
	// front side => (x, y, 1)

	vec3 origin = ray.org - m_pos;
	const vec3 & direction = ray.dir;

	// Algorithm simply is : test for plane intersection, then test for box surfaces; 
	//  Pick the shortest ray... 

	float t_min = INT_MAX;
	vec3 normal;

	// test back side z = 0
	{
		float t = (0 - origin.z) / direction.z;
		if(t >= EPSILON){
			vec3 intersection = origin + direction*t;
			if(intersection.x >= 0 && intersection.x <= m_size && 
				intersection.y >= 0 && intersection.y <= m_size)
			{
				// update smallest t; update normal
				if(t < t_min){
					t_min = t;
					normal = vec3(0, 0, -1);
				}
			}
		}
	}

	// front side z = m_size
	{ 
		float t = (m_size - origin.z) / direction.z;
		if(t >= EPSILON){
			vec3 intersection = origin + direction*t;
			if(intersection.x >= 0 && intersection.x <= m_size && 
				intersection.y >= 0 && intersection.y <= m_size)
			{
				// update smallest t; update normal
				if(t < t_min){
					t_min = t;
					normal = vec3(0, 0, 1);
				}
			}
		}
	}

	// left side x = 0
	{ 
		float t = (0 - origin.x) / direction.x;
		if(t >= EPSILON){
			vec3 intersection = origin + direction*t;
			if(intersection.z >= 0 && intersection.z <= m_size && 
				intersection.y >= 0 && intersection.y <= m_size)
			{
				// update smallest t; update normal
				if(t < t_min){
					t_min = t;
					normal = vec3(-1, 0, 0);
				}
			}
		}
	}

	// right side x = m_size
	{ 
		float t = (m_size - origin.x) / direction.x;
		if(t >= EPSILON){
			vec3 intersection = origin + direction*t;
			if(intersection.z >= 0 && intersection.z <= m_size && 
				intersection.y >= 0 && intersection.y <= m_size)
			{
				// update smallest t; update normal
				if(t < t_min){
					t_min = t;
					normal = vec3(1, 0, 0);
				}
			}
		}
	}

	// bottom side y = 0
	{ 
		float t = (0 - origin.y) / direction.y;
		if(t >= EPSILON){
			vec3 intersection = origin + direction*t;
			if(intersection.x >= 0 && intersection.x <= m_size && 
				intersection.z >= 0 && intersection.z <= m_size)
			{
				// update smallest t; update normal
				if(t < t_min){
					t_min = t;
					normal = vec3(0, -1, 0);
				}
			}
		}
	}

	// top side y = 1
	{ 
		float t = (m_size - origin.y) / direction.y;
		if(t >= EPSILON){
			vec3 intersection = origin + direction*t;
			if(intersection.x >= 0 && intersection.x <= m_size && 
				intersection.z >= 0 && intersection.z <= m_size)
			{
				// update smallest t; update normal
				if(t < t_min){
					t_min = t;
					normal = vec3(0, 1, 0);
				}
			}
		}
	}

	if(t_min == INT_MAX) return CastResult();
	
	CastResult result;
	result.type = HitType::Primitive;
	result.intersection = origin + (float)t_min*direction;
	result.surface_normal = normal;
	result.t = t_min;
	result.pos = m_pos;
	return result;
}

void CastResult::transform() {
	
	glm::mat4 trans = parentTrans * geoNode->get_transform();
	glm::mat4 invtrans = inverse(trans);

	trans *= glm::translate(pos);

	this->intersection = glm::vec3( trans*glm::vec4(intersection, 1) );
	this->surface_normal = glm::normalize(glm::vec3( glm::vec4(surface_normal, 0)*invtrans ));

}
