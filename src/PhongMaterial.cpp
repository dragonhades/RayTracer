// Fall 2018

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double opacity )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_opacity(opacity)
{
	if(m_opacity > 1) m_opacity = 1;
	if(m_opacity < 0) m_opacity = 0;
}

PhongMaterial::~PhongMaterial()
{}
