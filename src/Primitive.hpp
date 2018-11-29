// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <string>
#include "Ray.hpp"
#include "Primitive.hpp"
#include "polyroots.hpp"
#include "Ray.hpp"
#include "GeometryNode.hpp"
#include "CastResult.hpp"

enum class PrimitiveType {
    Sphere,
    Cube,
    Triangle,
    NonhierSphere,
    NonhierBox,
    NonhierCylinder,
    Mesh
};


class Primitive {
    public:
	virtual ~Primitive();
	virtual CastResult intersect(const Ray & ray) = 0;
	virtual PrimitiveType type() = 0;
};

class Sphere : public Primitive {
    public:
	virtual ~Sphere();
	virtual CastResult intersect(const Ray & ray) override;
	virtual PrimitiveType type() override { return PrimitiveType::Sphere; }
};

class Cube : public Primitive {
    public:
	virtual ~Cube();
	virtual CastResult intersect(const Ray & ray) override;
	virtual PrimitiveType type() override { return PrimitiveType::Cube; }
};

class NonhierSphere : public Primitive {
    public:
	NonhierSphere(const glm::vec3& pos, double radius)
	    : m_pos(pos), m_radius(radius)
	{
	}
	virtual ~NonhierSphere();
	virtual CastResult intersect(const Ray & ray) override;
	virtual PrimitiveType type() override { return PrimitiveType::NonhierSphere; }

    private:
	glm::vec3 m_pos;
	double m_radius;
};

class NonhierBox : public Primitive {
    public:
	NonhierBox(const glm::vec3& pos, double size)
	    : m_pos(pos), m_size(size)
	{
	}

	virtual ~NonhierBox();
	virtual CastResult intersect(const Ray & ray) override;
	virtual PrimitiveType type() override { return PrimitiveType::NonhierBox; }

    private:
	glm::vec3 m_pos;
	double m_size;
};

class NonhierCylinder : public Primitive {
    public:
	NonhierCylinder(const glm::vec3& pos, double radius, double length)
	    : m_pos(pos), m_radius(radius), m_length(length)
	{
	}
	virtual ~NonhierCylinder();
	virtual CastResult intersect(const Ray & ray) override;
	virtual PrimitiveType type() override { return PrimitiveType::NonhierCylinder; }

    private:
	glm::vec3 m_pos;
	double m_radius;
	double m_length;
};

