#include <glm/ext.hpp>

#include <cmath>
#include <numeric>

#include "DEBUG.hpp"

using namespace glm;
using namespace std;

// http://graphicscodex.com
//   Material - Transmission and Refraction
glm::vec3 get_refract(
              const vec3 & surface_normal, 
              const vec3 & direction,
              double n1, 
              double n2
) {
  // invert normal if we shoot ray from inside to outside of a solid... 
  vec3 normal = surface_normal;
  if(dot(normal, direction) > 0) {
    normal = -normal;
    // std::swap(n1, n2);   // this is magically unnecessary
  }

   double q = n1 / n2;
   double cos = -dot(normal, direction);
   double root_term = q*q * (1.0 - cos*cos);

   if (1.0 - root_term < 0) {
      cerr << "sqrt negative value." << endl;
      exit(EXIT_FAILURE);
   }

   double root = sqrt(1.0 - root_term);
   return direction * q + normal * (q * cos - root);
}

float absVec3(const vec3 & vec){
   float value = 0.0;
   for(int i = 0; i < 3; i++) value += vec[i] * vec[i];
   return sqrt(value);
}