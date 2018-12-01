// Fall 2018

#include "Material.hpp"

#include <lodepng.h>

#include "DEBUG.hpp"

using namespace glm;

Material::Material()
{}

Material::~Material()
{}

void Material::add_normalmap(const std::string & filename) {
	unsigned error = lodepng::decode(m_nmap, m_nmap_width, m_nmap_height, filename);
	DASSERT(error==0, lodepng_error_text(error));
}

vec3 Material::normal(double u, double v){
	DASSERT(m_nmap.size()>0, "normal map does not exist");

    DASSERT(std::abs(u) <= 1, "abs(u) <= 1");
    DASSERT(std::abs(v) <= 1, "abs(v) <= 1");
    
    int pixel_x = (u + 1) / 2 * (m_nmap_width - 1);
    int pixel_y = (v + 1) / 2 * (m_nmap_height - 1);
    
    int position = pixel_y * m_nmap_width + pixel_x;
    position = position * 4;
    
	return glm::vec3(m_nmap[position] / 255.0, m_nmap[position + 1] / 255.0, m_nmap[position + 2] / 255.0);
}