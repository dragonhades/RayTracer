// Fall 2018

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

#include <vector>
#include <algorithm>

using namespace glm;
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "DEBUG.hpp"


Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( string("./Assets/").append(fname).c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	m_bboxTrans = getBBoxTrans();
}

CastResult Mesh::intersect(const Ray & ray){
#define USE_BBOX
#ifdef USE_BBOX
	// get bbox
	if(m_vertices.empty()) {
		DASSERT(m_vertices.empty() == false, "Mesh is empty.");
		return CastResult();
	}
	// DPRINTVEC(m_bboxTrans);

	Ray ray_copy = ray;
	ray_copy.setTransform(m_bboxTrans);
	Cube cube;
	CastResult result = cube.intersect(ray_copy);
	if(result.isHit()){
		result.type = HitType::BBox;
		// DCOUNT("hit bbox");
		return result;
	} else {
		return CastResult();
	}
#else
	return intersectTriangles(ray);
#endif
}

CastResult Mesh::intersectTriangles(const Ray & ray){

	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	
	const float EPSILON = 0.01;

	const vec3 & rayOrigin = ray.org; 
    const vec3 & rayVector = ray.dir; 

    float t_min = INT_MAX;
   	Triangle * tri_min;

	for(Triangle & tri : m_faces){
		const vec3 & vertex0 = m_vertices[tri.v1];
	    const vec3 & vertex1 = m_vertices[tri.v2];  
	    const vec3 & vertex2 = m_vertices[tri.v3];

	    vec3 edge1, edge2, h, s, q;
	    float a,f,u,v;
	    edge1 = vertex1 - vertex0;
	    edge2 = vertex2 - vertex0;
	    h = cross(rayVector, edge2);
	    a = dot(edge1, h);
	    if (a > -EPSILON && a < EPSILON)
	        continue;    // This ray is parallel to this triangle.
	    f = 1.0/a;
	    s = rayOrigin - vertex0;
	    u = f * dot(s, h);
	    if (u < 0.0 || u > 1.0)
	        continue;
	    q = cross(s, edge1);
	    v = f * dot(rayVector, q);
	    if (v < 0.0 || u + v > 1.0)
	        continue;
	    // At this stage we can compute t to find out where the intersection point is on the line.
	    float t = f * dot(edge2, q);
	    if (t > EPSILON) // ray intersection
	    {
	        if(t < t_min){
				t_min = t;
				tri_min = &tri;
				// DCOUNT("HIT");
			}
	    }
	    else // This means that there is a line intersection but not a ray intersection.
	        continue;
	}

    if(t_min == INT_MAX) return CastResult();

	CastResult result;
	result.type = HitType::Triangle;
	result.intersection = rayOrigin + (float)t_min*rayVector;
	result.surface_normal = normalize(getNormal(*tri_min));
	result.t = t_min;

	return result;

}

vec3 Mesh::getNormal(const Triangle & tri) {

	const vec3 & p1 = m_vertices[tri.v1];
	const vec3 & p2 = m_vertices[tri.v2];
	const vec3 & p3 = m_vertices[tri.v3];

	const vec3 U = p2 - p1;
	const vec3 V = p3 - p1;

	vec3 normal;
	normal.x = U.y * V.z - U.z * V.y;
	normal.y = U.z * V.x - U.x * V.z;
	normal.z = U.x * V.y - U.y * V.x;

	return normal;
}


mat4 Mesh::getBBoxTrans() const {
	float x_max = std::max_element(m_vertices.begin(), m_vertices.end(),
		[](const vec3 & v1, const vec3 & v2) { return v1.x < v2.x; } )->x;

	float x_min = std::min_element(m_vertices.begin(), m_vertices.end(),
		[](const vec3 & v1, const vec3 & v2) { return v1.x < v2.x; } )->x;

	float y_max = std::max_element(m_vertices.begin(), m_vertices.end(),
		[](const vec3 & v1, const vec3 & v2) { return v1.y < v2.y; } )->y;

	float y_min = std::min_element(m_vertices.begin(), m_vertices.end(),
		[](const vec3 & v1, const vec3 & v2) { return v1.y < v2.y; } )->y;

	float z_max = std::max_element(m_vertices.begin(), m_vertices.end(),
		[](const vec3 & v1, const vec3 & v2) { return v1.z < v2.z; } )->z;
	
	float z_min = std::min_element(m_vertices.begin(), m_vertices.end(),
		[](const vec3 & v1, const vec3 & v2) { return v1.z < v2.z; } )->z;

	//clamp to >0 when == 0
	const float EPSILON = 0.0001;
	if(x_max == x_min) {
		float center = x_max;
		x_max = center + EPSILON;
		x_min = center - EPSILON;
	}
	if(y_max == y_min) {
		float center = y_max;
		y_max = center + EPSILON;
		y_min = center - EPSILON;
	} 
	if(z_max == z_min) {
		float center = z_max;
		z_max = center + EPSILON;
		z_min = center - EPSILON;
	} 
	
	vec3 center(x_min+(x_max-x_min)*0.5, 
				y_min+(y_max-y_min)*0.5, 
				z_min+(z_max-z_min)*0.5);
	// DPRINTVEC(center);

	mat4 trans = scale(vec3(1/(x_max-x_min), 1/(y_max-y_min), 1/(z_max-z_min))) * translate(-center);
	// DPRINTVEC(trans);

	trans = glm::translate(vec3(0.5, 0.5, 0.5)) * trans;	// move to top left far corner

	return trans;
}


std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
