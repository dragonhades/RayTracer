// Fall 2018

#pragma once

#include <glm/glm.hpp>

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
  glm::vec3 intersection_old;
  glm::vec3 surface_normal;
  GeometryNode* gnode = nullptr;
  double t = 0;
  glm::vec3 pos;
  glm::mat4 parentTrans;

  bool isHit() const;

  void transform();

};