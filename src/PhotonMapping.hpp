#include <glm/glm.hpp>
#include <vector>

#include "Ray.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Light.hpp"

void Photon_Mapping(
		int x, 
		int z, 

		// Lighting parameters  
		const SceneNode* target,
		const std::list<Light *> lights,
		std::vector<std::vector<glm::vec3>> & photon_mapping
);