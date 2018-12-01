// Fall 2018

#include "Texture.hpp"

#include <lodepng.h>

#include "DEBUG.hpp"

using namespace glm;

Texture::Texture(const std::string & textureName)
{
    unsigned error = lodepng::decode(m_image, m_width, m_height, textureName);
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    
    DASSERT(m_nmap.size()>0, "empty normal map");
}

Texture::~Texture()
{}


vec3 Texture::color(double u, double v){
	DASSERT(m_image.size()>0, "normal map does not exist");

    DASSERT(std::abs(u) <= 1, "abs(u) <= 1");
    DASSERT(std::abs(v) <= 1, "abs(v) <= 1");
    
    int pixel_x = (u + 1) / 2 * (m_width - 1);
    int pixel_y = (v + 1) / 2 * (m_height - 1);
    
    unsigned long position = pixel_y * m_width + pixel_x;
    position = position * 4;
    
	return glm::vec3(m_image[position] / 255.0, m_image[position + 1] / 255.0, m_image[position + 2] / 255.0);
}