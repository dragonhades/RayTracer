// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "GeometryNode.hpp"
#include "SolidNode.hpp"

enum class HitType {
  None,
  Primitive,
  Triangle,
  BBox,
  CSG
};

struct CastResult {
  HitType type = HitType::None;
  glm::vec3 intersection;
  glm::vec3 surface_normal;
  GeometryNode* gnode = nullptr;
  double t;
  glm::vec3 pos;
  glm::mat4 parentTrans;

  bool isHit() const { return type != HitType::None; }

  void transform() {
  
    glm::mat4 trans = parentTrans * gnode->get_transform();
    glm::mat4 invtrans = inverse(trans);

    trans *= glm::translate(pos);

    this->intersection = glm::vec3( trans*glm::vec4(intersection, 1) );
    this->surface_normal = glm::normalize(glm::vec3( glm::vec4(surface_normal, 0)*invtrans ));

  }

};