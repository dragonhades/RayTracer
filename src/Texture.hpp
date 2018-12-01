// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class Texture : public Material {
public:
  Texture(const std::string & textureName);
  virtual ~Texture();

  glm::vec3 color(double u, double v);
    
  std::vector<unsigned char> m_image;
    
  uint m_width;
  uint m_height;
};
