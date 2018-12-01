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
	std::string fname = std::string("./Assets/").append(filename);
	unsigned error = lodepng::decode(m_nmap, m_nmap_width, m_nmap_height, fname);
 	if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	DASSERT(m_nmap.size()>0, "empty normal map");

	m_has_normal = true;
}

vec3 Material::normal(double u, double v){
	DASSERT(m_nmap.size()>0, "normal map does not exist");

    DASSERT(std::abs(u) <= 1, "abs(u) <= 1");
    DASSERT(std::abs(v) <= 1, "abs(v) <= 1");
    
    int pixel_x = (u + 1) / 2 * (m_nmap_width - 1);
    int pixel_y = (v + 1) / 2 * (m_nmap_height - 1);
    
    int position = pixel_y * m_nmap_width + pixel_x;
    position = position * 4;
    vec3 nmap = vec3(m_nmap[position], m_nmap[position + 1], m_nmap[position + 2]);
    nmap = vec3( (nmap.x-127.0f)/255.0f, (nmap.y-127.0f)/255.0f, (nmap.x-255.0f)/255.0f);

	return glm::normalize(nmap);
}