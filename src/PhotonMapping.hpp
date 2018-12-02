#include <glm/glm.hpp>
#include <vector>

#include "Ray.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Light.hpp"

extern int photon_w;
extern int photon_h;
extern int photon_dw;
extern int photon_dh;

void Photon_Mapping(
		int x, 
		int z,

		// Lighting parameters  
		const SceneNode* target,
		const std::list<Light *> lights
);

glm::vec2 world_2_photonMapping(double x, double z);

glm::vec2 photonMapping_2_world(int x, int z);
