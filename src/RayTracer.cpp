// Fall 2018

#include <glm/ext.hpp>

#include <cmath>
#include <numeric>
#include <thread>
#include <mutex>

#include "RayTracer.hpp"
#include "RenderPixel.hpp"
#include "PhotonMapping.hpp"
#include "DEBUG.hpp"
#include "SceneNode.hpp"
#include "Intersect.hpp"

using namespace glm;
using namespace std;

// ---------------------GLOBALS------------------------//
int global_x;
int global_y;
std::mutex xy_mutx;

int progress = 0;
std::mutex progress_mutx;

SceneNode * root;
vector<vector<vec3>> color_chart;
vector<vector<vec3>> photonMap_chart;

int obj_w;
int obj_h;
int photon_distw;
int photon_disth;
int photon_mapw;
int photon_maph;
// ---------------------GLOBALS------------------------//

void Render(
		// What to render  
		SceneNode * root_node,

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
) {


	root = root_node;


    std::cout << "Calling Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

//----------------------------------------------------------------------------------

	// apply viewMatrix this way
	root->translate(-eye);
	for(Light* l : lights){
		// l->position = vec3( translate(-eye) * root->get_transform()* vec4(l->position, 1) );
		l->position = vec3( translate(-eye) * vec4(l->position, 1) );	
	}


	int h = image.height();
	int w = image.width();

#ifdef SSAA
	h++;
	w++;
	cout << "Super Sampling Enabled." << endl;
#endif	//SSAA

	for (register uint y = 0; y < h; ++y) {
		color_chart.emplace_back();
		for (register uint x = 0; x < w; ++x){
			color_chart[y].emplace_back();
		} 
	}


	int num_cores = std::thread::hardware_concurrency();
	cout << "Using " << num_cores << " cores,"<< "\t"
		<< NUM_THREADS << " threads." <<endl;
	vector<std::thread> threads(NUM_THREADS);


#ifdef PHOTON_MAPPING

	mat4 trans;
	const SceneNode* photon_surface = findNode(root, "ground", trans);
	DASSERT(photon_surface!=nullptr, "null");

	obj_w = 1000;
	obj_h = 1000;
	photon_distw = 10000;
	photon_disth = 10000;
	photon_mapw = 500;
	photon_maph = 500;

	/*
		Note: possible hit point range from 0 to photon_dw/photon_dh, 
				hence plus one is crucial
	*/
	for (register uint y = 0; y < photon_maph+1; ++y) {
		photonMap_chart.emplace_back();
		for (register uint x = 0; x < photon_mapw+1; ++x){
			photonMap_chart[y].emplace_back();
			photonMap_chart[y][x] = vec3(0);
		}
	}

	std::cout << "Compute Photon Mapping ..."<<std::endl;


	for(int i=0; i<NUM_THREADS; ++i){
		threads[i] = std::thread(PhotonMap_Thread, trans, lights);
		if(threads[i].get_id() == std::thread::id()) {
		     std::cerr << "Abort: Failed to create thread " << i << std::endl;
			 exit(EXIT_FAILURE);
		}
	}

	for(int i=0; i<NUM_THREADS; ++i){
		threads[i].join();
	}

	global_x = 0;
	global_y = 0;
	progress = 0;

#endif	// PHOTON_MAPPING

	std::cout << std::endl;
	std::cout << "Rendering ..."<<std::endl;


#define loop
#ifndef loop
	// 256 X 256
	int x = 64;
	int y = 64;
	vec3 color = Render_Pixel(x, y, w, h, root, fovy, ambient, lights);
	DPRINT("Color is ");
	DPRINTVEC(color);
	DPRINT("......................");
	color_chart[y][x] = color;	// should be redundant
#else

	for(int i=0; i<NUM_THREADS; ++i){
		threads[i] = std::thread(Render_Thread, 
			 w, h, eye, view, up, fovy, ambient, lights);
		if(threads[i].get_id() == std::thread::id()) {
		     std::cerr << "Abort: Failed to create thread " << i << std::endl;
			 exit(EXIT_FAILURE);
		}
	}

	for(int i=0; i<NUM_THREADS; ++i){
		threads[i].join();
	}

#endif

	w = image.width();
	h = image.height();

#ifdef SSAA
	for (register uint y = 0; y < h; ++y) {
		for (register uint x = 0; x < w; ++x) {
			image(x, y, 0) = (color_chart[y][x].r + 
							color_chart[y][x+1].r +
							color_chart[y+1][x].r +
							color_chart[y+1][x+1].r) / 4.0f;
			image(x, y, 1) = (color_chart[y][x].g + 
							color_chart[y][x+1].g +
							color_chart[y+1][x].g +
							color_chart[y+1][x+1].g) / 4.0f;
			image(x, y, 2) = (color_chart[y][x].b + 
							color_chart[y][x+1].b +
							color_chart[y+1][x].b +
							color_chart[y+1][x+1].b) / 4.0f;
		}
	}
#else
	for (register uint y = 0; y < h; ++y) {
		for (register uint x = 0; x < w; ++x) {
			image(x, y, 0) = color_chart[y][x].r;
			image(x, y, 1) = color_chart[y][x].g;
			image(x, y, 2) = color_chart[y][x].b;
		}
	}
#endif

	std::cout<<"\nComplete!"<<std::endl;

}

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
) {

	// Work, until no work available
	while(true){	

		int x, y;

		/* xxxxxxxxxxxxxxxxxxxxx Critical section begin xxxxxxxxxxxxxxxxxxxxx */
		{
			// Multi-threading
			//  Use global x and y to assign task to each thread.
			//  In every loop, fetch next x, y and move them one pixel to the right
			//  As result, each thread will always be busy. 
			std::lock_guard<std::mutex> xy_lck (xy_mutx);

			if(global_y >= h) return;

			x = global_x;
			y = global_y;
			global_x++;
			if(global_x == w){
				global_x = 0;
				global_y++;
			}
			if(global_y >= h) return;
		}
		/* xxxxxxxxxxxxxxxxxxxxx Critical section end xxxxxxxxxxxxxxxxxxxxxxx */


		const vec3 & color = Render_Pixel(x, y, w, h, fovy, ambient, lights);
		color_chart[y][x] = color;

		int new_percent;
		int old_percent;

	#ifdef PRINT_PROGRESS
		/* xxxxxxxxxxxxxxxxxxxxx Critical section begin xxxxxxxxxxxxxxxxxxxxx */
		{
			std::lock_guard<std::mutex> progressBar_lck (progress_mutx);
		
			progress ++;
			new_percent = int(progress / float(w*h) *100.0f);		

		// This has potential sychornization problem, but it's okay; we just want to see percentage going up... 
			cout << "[               " << new_percent << " %               ]\r";
		}
		/* xxxxxxxxxxxxxxxxxxxxx Critical section end xxxxxxxxxxxxxxxxxxxxxxx */	
	#endif

	} // while true
}

void PhotonMap_Thread(
		// Lighting parameters  
		const glm::mat4 & trans,
		const std::list<Light *> lights
) {

	// Work, until no work available
	while(true){	

		int x, z;

		/* xxxxxxxxxxxxxxxxxxxxx Critical section begin xxxxxxxxxxxxxxxxxxxxx */
		{
			// Multi-threading
			//  Use global x and y to assign task to each thread.
			//  In every loop, fetch next x, y and move them one pixel to the right
			//  As result, each thread will always be busy. 
			std::lock_guard<std::mutex> xy_lck (xy_mutx);

			if(global_y >= photon_disth) return;

			x = global_x;
			z = global_y;
			global_x++;
			if(global_x == photon_distw){
				global_x = 0;
				global_y++;
			}
			if(global_y >= photon_disth) return;
		}
		/* xxxxxxxxxxxxxxxxxxxxx Critical section end xxxxxxxxxxxxxxxxxxxxxxx */


		Photon_Mapping(x, z, trans, lights);

		int new_percent;
		int old_percent;

	#ifdef PRINT_PROGRESS
		/* xxxxxxxxxxxxxxxxxxxxx Critical section begin xxxxxxxxxxxxxxxxxxxxx */
		{
			std::lock_guard<std::mutex> progressBar_lck (progress_mutx);
		
			progress ++;
			new_percent = int(progress / float(photon_distw*photon_disth) *100.0f);		

		// This has potential sychornization problem, but it's okay; we just want to see percentage going up... 
			cout << "[               " << new_percent << " %               ]\r";
		}
		/* xxxxxxxxxxxxxxxxxxxxx Critical section end xxxxxxxxxxxxxxxxxxxxxxx */	
	#endif

	} // while true
}
