// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

#define PI 3.1415926

#define NUM_THREADS 8

// #define MTHREAD
#define SSAA
#define PRINT_PROGRESS
// #define DRAW_BBOX

extern std::vector<std::vector<glm::vec3>> color_chart;

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

		// What to render  
		const SceneNode * root,

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

		// What to render  
		const SceneNode * root,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);