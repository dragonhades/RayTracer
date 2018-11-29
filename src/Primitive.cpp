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

NonhierCylinder::~NonhierCylinder()
{
}

CastResult NonhierCylinder::intersect(const Ray & ray) {
    const vec3 & origin = ray.org - m_pos;
    const vec3 & direction = ray.dir;

    double A = (direction.x * direction.x) + (direction.z * direction.z);
    double B = 2*(origin.x*direction.x + origin.z*direction.z);
    double C = (origin.x * origin.x) + (origin.z * origin.z) - (m_radius*m_radius);
    double roots[2];

    const double ymin = -m_length/2.0f;
    const double ymax = m_length/2.0f;

    size_t num_roots = quadraticRoots(A, B, C, roots);
    CastResult result;
    if (num_roots == 0){

	float t1 = (m_length/2.0f-origin.y)/direction.y;
	float t2 = (-m_length/2.0f-origin.y)/direction.y;

	if(t1>EPSILON && t1 < t2){
	    // Top cap
	    const vec3 & intersection = origin + t1*direction;
	    double r2 = intersection.x*intersection.x + intersection.z*intersection.z; 
	    if(r2 <= m_radius*m_radius) {
		result.type = HitType::Primitive;
		result.intersection = intersection;
		result.surface_normal = vec3(0, 1, 0);
		result.t = t1;
		result.pos = m_pos;
	    }
	}
	else if(t2>EPSILON && t2 < t1){
	    // Bot cap
	    const vec3 & intersection = origin + t2*direction;
	    double r2 = intersection.x*intersection.x + intersection.z*intersection.z;
	    if(r2 <= m_radius*m_radius) {
		result.type = HitType::Primitive;
		result.intersection = intersection;
		result.surface_normal = vec3(0, -1, 0);
		result.t = t2;
		result.pos = m_pos;
	    }
	}

	return result;

    } else if (num_roots == 1){

	double t = roots[0];
	double y1 = origin.y + t * direction.y;

	if (y1 >= ymin && y1 <= ymax){
	    result.type = HitType::Primitive;
	    result.intersection = origin + (float)t*direction;
	    result.surface_normal = vec3(result.intersection.x, 0, result.intersection.z);
	    result.t = t;
	    result.pos = m_pos;
	    return result;
	} else {
	    return CastResult();
	}
    }else{
	//cout << "2 root: " << roots[0] << " " << roots[1] << endl;
	
	if (roots[0] < EPSILON && roots[1] < EPSILON) return result;
	
	double t;

	if (roots[0] < EPSILON){
	    t = roots[1];
	    double y1 = origin.y + t * direction.y;

	    if (y1 >= ymin && y1 <= ymax){
		result.type = HitType::Primitive;
		result.intersection = origin + (float)t*direction;
		result.surface_normal = vec3(result.intersection.x, 0, result.intersection.z);
		result.t = t;
		result.pos = m_pos;
		return result;
	    } else {
		return CastResult();
	    }

	}else if (roots[1] < EPSILON){
	    t = roots[0];
	    double y1 = origin.y + t * direction.y;

	    if (y1 >= ymin && y1 <= ymax){
		result.type = HitType::Primitive;
		result.intersection = origin + (float)t*direction;
		result.surface_normal = vec3(result.intersection.x, 0, result.intersection.z);
		result.t = t;
		result.pos = m_pos;
		return result;
	    } else {
		return CastResult();
	    }
	}else {
	    double t=INT_MAX;
	    double y1 =origin.y + roots[0] * direction.y;
	    if (y1 >= ymin && y1 <= ymax){
		if(roots[0]<t) {
		    t = roots[0];
		    result.intersection = origin + (float)t * direction;
		    result.surface_normal = vec3(result.intersection.x, 0, result.intersection.z);
		}
	    }
	    double y2 = origin.y + roots[1] * direction.y;
	    if (y2 >= ymin && y2 <= ymax){
		if(roots[1] < t) {
		    t = roots[1];
		    result.intersection = origin + (float)t * direction;
		    result.surface_normal = vec3(result.intersection.x, 0, result.intersection.z);
		}
	    }
	    if (y1 < ymin && y2 > ymin){
		if (roots[0]<roots[1]){
		    // look up
		    double ty = (ymin-origin.y)/direction.y;
		    if (ty > EPSILON && t > ty){
			t = ty;
			result.intersection = origin + (float)t * direction;
			result.surface_normal = vec3(0, -1, 0);
		    }
		} 
	    }
	    else if (y2 < ymin && y1 > ymin){
		if (roots[1] < roots[0]){
		    // look up
		    double ty = (ymin-origin.y)/direction.y;
		    if (ty > EPSILON && t > ty){
			t = ty;
			result.intersection = origin + (float)t * direction;
			result.surface_normal = vec3(0, -1, 0);
		    }
		}
	    }
	    else if (y1 > ymax && y2 < ymax){
		if (roots[0]<roots[1]){
		    // look down
		    double ty = (ymax-origin.y)/direction.y;
		    if (ty > EPSILON && t > ty){
			t = ty;
			result.intersection = origin + (float)t * direction;
			result.surface_normal = vec3(0, 1, 0);
		    }
		}
	    }
	    else if (y2 > ymax && y1 < ymax){
		if (roots[1] < roots[0]){
		    // look down
		    double ty = (ymax-origin.y)/direction.y;
		    if (ty > EPSILON && t > ty){
			t = ty;
			result.intersection = origin + (float)t * direction;
			result.surface_normal = vec3(0, 1, 0);
		    }
		}
	    }

	    if(t!=INT_MAX){
		result.type = HitType::Primitive;
		result.t = t;
		result.pos = m_pos;
		return result;
	    } else {
		return CastResult();
	    }
	}
    }
    return result;
}
