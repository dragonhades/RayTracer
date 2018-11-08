// Fall 2018

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double opacity )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_opacity(opacity)
{}

PhongMaterial::~PhongMaterial()
{}
