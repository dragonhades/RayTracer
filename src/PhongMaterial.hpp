// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

#include <glm/glm.hpp>

class PhongMaterial : public Material {
private:
  glm::vec3 m_kd;

public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double opacity);
  virtual ~PhongMaterial();

  glm::vec3 m_ks;

  virtual glm::vec3 get_kd(double u=-1, double v=-1) const;

  void addTexture(const std::string & textureName);

  double m_shininess;
  double m_opacity;

  std::vector<unsigned char> m_image;
    
  uint m_img_width;
  uint m_img_height;

};
