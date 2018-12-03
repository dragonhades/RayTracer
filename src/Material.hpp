// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

enum class MaterialType {
	None,
	Phong,
	Texture
};

class Material {
public:
  virtual ~Material();

  void add_normalmap(const std::string & name);
  bool has_normalmap() { return m_has_normal; }
  glm::vec3 normal(double u, double v);

  MaterialType m_type = MaterialType::None;

protected:
  Material();

  bool m_has_normal = false;
  std::vector<unsigned char> m_nmap;

  uint m_nmap_width;
  uint m_nmap_height;
};
