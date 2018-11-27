// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

class Ray;

CastResult intersectNode(const SceneNode* node, 
						  const Ray & ray, 
						  const glm::mat4 & trans);

CastResult intersectScene(const Ray & ray, 
						const SceneNode* target=nullptr);