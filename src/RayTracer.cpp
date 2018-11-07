// Fall 2018

#include <glm/ext.hpp>

#include <cmath>
#include <numeric>
#include <thread>
#include <mutex>

#include "RayTracer.hpp"
#include "DEBUG.hpp"
#include "SceneNode.hpp"

using namespace glm;
using namespace std;

// ---------------------GLOBALS------------------------//
int global_x;
int global_y;
std::mutex xy_mutx;

int progress = 0;
std::mutex progress_mutx;


vector<vector<vec3>> color_chart;

// ---------------------GLOBALS------------------------//

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
) {
    std::cout << "Calling A4_Render(\n" <<
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
		l->position = vec3( translate(-eye) * vec4(l->position, 1) );
	}


	int h = image.height();
	int w = image.width();

#ifdef SSAA
	h++;
	w++;
	cout << "Super Sampling Enabled." << endl;
#endif

	for (register uint y = 0; y < h; ++y) {
		color_chart.emplace_back();
		for (register uint x = 0; x < w; ++x){
			color_chart[y].emplace_back();
		} 
	}

#ifdef MTHREAD
	int num_cores = std::thread::hardware_concurrency();
	cout <<"Multi-threading..." << endl;
	cout << "Using " << num_cores << " cores,"<< "\t"
		<< NUM_THREADS << " threads." <<endl;
	vector<std::thread> threads(NUM_THREADS);
#endif


#define loop
#ifndef loop
	// 256 X 256
	int x = 64;
	int y = 64;
	vec3 color = Render_Pixel(x, y, w, h, root, eye, view, up, fovy, ambient, lights);
	DPRINT("Color is ");
	DPRINTVEC(color);
	DPRINT("......................");
	color_chart[y][x] = color;	// should be redundant
#else

 #ifdef MTHREAD
	for(int i=0; i<NUM_THREADS; ++i){
		threads[i] = std::thread(Render_Thread, 
			 w, h, root, eye, view, up, fovy, ambient, lights);
		if(threads[i].get_id() == std::thread::id()) {
		     std::cerr << "Abort: Failed to create thread " << i << std::endl;
			 exit(EXIT_FAILURE);
		}
	}

	for(int i=0; i<NUM_THREADS; ++i){
		threads[i].join();
	}

 #else 
	cout <<"Single thread." << endl;

	for(register int y=0; y < h; ++y){
		for(register int x=0; x < w; ++x){
			
			color_chart[y][x] = Render_Pixel(x, y, w, h, root, eye, view, up, fovy, ambient, lights);

#ifdef PRINT_PROGRESS
			progress ++;
			int new_percent = int(progress / float(w*h) *100.0f);
			cout << "[               " << new_percent << " %               ]\r";
#endif
		}
	}

 #endif


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


		vec3 color = Render_Pixel(x, y, w, h, root, eye, view, up, fovy, ambient, lights);
		color_chart[y][x] = color;

		/* xxxxxxxxxxxxxxxxxxxxx Critical section begin xxxxxxxxxxxxxxxxxxxxx */
	#ifdef PRINT_PROGRESS
		std::lock_guard<std::mutex> progressBar_lck (progress_mutx);
		
			progress ++;
			int new_percent = int(progress / float(w*h) *100.0f);
			cout << "[               " << new_percent << " %               ]\r";
	#endif
		/* xxxxxxxxxxxxxxxxxxxxx Critical section end xxxxxxxxxxxxxxxxxxxxxxx */

	} // while true
}
