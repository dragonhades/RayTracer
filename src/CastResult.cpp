#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "GeometryNode.hpp"
#include "SolidNode.hpp"
#include "DEBUG.hpp"

#include "CastResult.hpp"

bool CastResult::isHit() const {
	return type != HitType::None; 
}

void CastResult::transform() {

    DASSERT(this->isHit(), "Attempt to transform empty CastResult.");
    DASSERT(this->gnode!=nullptr, "Attempt to transform empty CastResult.");
  
    glm::mat4 trans = parentTrans * gnode->get_transform();
    glm::mat4 invtrans = inverse(trans);

    trans *= glm::translate(pos);

    this->intersection = glm::vec3( trans*glm::vec4(intersection, 1) );
    this->surface_normal = glm::normalize(glm::vec3( glm::vec4(surface_normal, 0)*invtrans ));

}