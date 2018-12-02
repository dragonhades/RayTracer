// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

#define NUM_THREADS 32
#define PRINT_PROGRESS

#define SSAA
#define REFLECTION
#define REFRACTION
#define PHOTON_MAPPING
#define MAX_SHADE_RECURSION 3

// #define DRAW_BBOX
#define LIGHT_TURN_ON

class Ray;
class CastResult;

extern std::vector<std::vector<glm::vec3>> color_chart;
extern std::vector<std::vector<glm::vec3>> photonMap_chart;

extern SceneNode * root;

extern int photon_w;
extern int photon_h;
extern int photon_dw;
extern int photon_dh;

void Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);

void Render_Thread(
		int w, 
		int h,

		// Viewing parameters  
		const glm::vec3 eye,
		const glm::vec3 view,
		const glm::vec3 up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 ambient,
		const std::list<Light *> lights
);

void PhotonMap_Thread(
		// Lighting parameters  
		const SceneNode* target,
		const std::list<Light *> lights
);

glm::vec3 shading(
		const Ray & ray, 
		const int recursionDepth,

		// What to render  
		const SceneNode * root,
		
		int x, 
		int y, 
		int w, 
		int h,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);
