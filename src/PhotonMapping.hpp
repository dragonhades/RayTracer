#include <glm/glm.hpp>
#include <vector>

#include "Ray.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Light.hpp"

extern int obj_w;
extern int obj_h;
extern int photon_distw;
extern int photon_disth;
extern int photon_mapw;
extern int photon_maph;

void Photon_Mapping(
		int x, 
		int z,

		// Lighting parameters  
		const glm::mat4 & trans,
		const std::list<Light *> lights
);

glm::vec2 world_2_photonMap(double x, double z);

glm::vec2 raydist_2_world(int x, int z);