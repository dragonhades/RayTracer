// Fall 2018

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );

  virtual CastResult intersect(const Ray & ray) override;
  virtual PrimitiveType type() override { return PrimitiveType::Mesh; }

  CastResult intersectTriangles(const Ray & ray);

  glm::mat4 getBBoxTrans() const;

  glm::vec3 getNormal(const Triangle & tri);
  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

	glm::mat4 m_bboxTrans;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
