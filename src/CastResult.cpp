#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "GeometryNode.hpp"
#include "SolidNode.hpp"
#include "DEBUG.hpp"

#include "CastResult.hpp"

using namespace glm;

bool CastResult::isHit() const {
	return type != HitType::None; 
}

void CastResult::transform() {

    DASSERT(this->isHit(), "Attempt to transform empty CastResult.");
    DASSERT(this->gnode!=nullptr, "Attempt to transform empty CastResult.");

    glm::mat4 trans = parentTrans * gnode->get_transform();
    glm::mat4 invtrans = inverse(trans);

    trans *= glm::translate(pos);

    if(gnode->m_material && gnode->m_material->has_normalmap()){
		const vec3 & n_map = gnode->m_material->normal(intersection.x, intersection.z);
		const vec3 & subtract = glm::normalize(surface_normal) - vec3(0, 0, -1);
		this->surface_normal = glm::normalize(n_map + subtract);
    }

    this->intersection_old = intersection;

    this->intersection = glm::vec3( trans*glm::vec4(intersection, 1) );
    this->surface_normal = glm::normalize(glm::vec3( glm::vec4(surface_normal, 0)*invtrans ));

}