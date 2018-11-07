// Fall 2018

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a ray.
struct Ray {
  Ray(){}
  Ray(const glm::vec3 & pt1, const glm::vec3 &pt2);
  
  glm::vec3 org;
  glm::vec3 dir;

  void setTransform(const glm::mat4& trans);
};

std::ostream& operator<<(std::ostream& out, const Ray& r);