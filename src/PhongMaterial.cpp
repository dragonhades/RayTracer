// Fall 2018

#include "PhongMaterial.hpp"
#include "Material.hpp"

#include <lodepng.h>

#include "DEBUG.hpp"

using namespace glm;

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double opacity )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_opacity(opacity)
{
	Material::m_type = MaterialType::Phong;
	if(m_opacity > 1) m_opacity = 1;
	if(m_opacity < 0) m_opacity = 0;
}

PhongMaterial::~PhongMaterial()
{}

void PhongMaterial::addTexture(const std::string & textureName)
{
	std::string fname = std::string("./Assets/").append(textureName);
    unsigned error = lodepng::decode(m_image, m_img_width, m_img_height, fname);
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    
    DASSERT(m_image.size()>0, "empty normal map");

    m_type = MaterialType::Texture;
}

glm::vec3 PhongMaterial::get_kd(double u, double v) const {
  	if(m_type == MaterialType::Phong) return m_kd;

  	// Texture
	DASSERT(m_image.size()>0, "normal map does not exist");

    // DASSERT(std::abs(u) <= 1, "abs(u) <= 1");
    // DASSERT(std::abs(v) <= 1, "abs(v) <= 1");
    if(std::abs(u) > 1 || std::abs(v) > 1) return m_kd;

    // std::cout<<u<<", "<<v<<std::endl;
    
    int pixel_x = (u + 1) / 2 * (m_img_width - 1);
    int pixel_y = (v + 1) / 2 * (m_img_height - 1);
    
    unsigned long position = pixel_y * m_img_width + pixel_x;
    position = position * 4;

    if(position >= m_image.size()) return m_kd;
    
	return glm::vec3(m_image[position], m_image[position + 1], m_image[position + 2])/255.0f;
}