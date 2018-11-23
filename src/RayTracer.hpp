// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

#define NUM_THREADS 8
#define MTHREAD
#define PRINT_PROGRESS

// #define SSAA
// #define REFLECTION
// #define REFRACTION
#define MAX_SHADE_RECURSION 6

// #define DRAW_BBOX
#define USE_LIGHT_FRONT
#define USE_LIGHT_BACK
class Ray;
class CastResult;

extern std::vector<std::vector<glm::vec3>> color_chart;
extern SceneNode * root;

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

glm::vec3 Render_Pixel(
		int x, 
		int y, 
		int w, 
		int h,

		// Viewing parameters
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
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

CastResult intersectScene(const SceneNode* node, 
						  const Ray & ray
);