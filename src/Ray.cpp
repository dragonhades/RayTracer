// Fall 2018

#include <iostream>

#include <glm/ext.hpp>

#include "Ray.hpp"
#include "DEBUG.hpp"

Ray::Ray(const glm::vec3 &org, const glm::vec3 &dir)
  : org(org),
    dir(dir)
{
}


void Ray::setTransform(const glm::mat4& trans){
  glm::vec3 point2 = org + dir;
  org = glm::vec3(trans * glm::vec4(org, 1));
  dir = glm::vec3(trans * glm::vec4(point2, 1)) - org;
}


std::ostream& operator<<(std::ostream& out, const Ray& r)
{
  out << "R[" << glm::to_string(r.org) 
  	  << ", " << glm::to_string(r.dir) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    // out << l.falloff[i];
  }
  out << "]";
  return out;
}
