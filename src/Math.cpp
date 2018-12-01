#include <glm/ext.hpp>

#include <cmath>
#include <numeric>

#include "DEBUG.hpp"

using namespace glm;
using namespace std;

// http://graphicscodex.com
//    Material - Transmission and Refraction
// glm::vec3 get_refract(
//               const vec3 & normal, 
//               const vec3 & direction,
//               double n1, 
//               double n2
// ) {
//   float q = n1 / n2;
//   vec3 w = -direction;
//   vec3 first_term = -q*(w - dot(w, normal)*normal);
//   float second_term = q*q*(1.0 - pow(dot(w, normal), 2));

//   if (second_term > 1.0) {
//     cerr << "Bad refraction vector!" << endl;
//     exit(EXIT_FAILURE);
//   }

//    return normalize(first_term - sqrt(1.0 - second_term)*normal);
// }

// http://graphicscodex.com
//    Material - Transmission and Refraction
glm::vec3 get_refract(
              const vec3 & normal, 
              const vec3 & direction,
              double n1, 
              double n2
) {

   double q = n1 / n2;
   double cos = -dot(normal, direction);
   double root_term = q*q * (1.0 - cos*cos);

   if (1.0 - root_term < 0) {
      return vec3(0);
   }

   double root = sqrt(1.0 - root_term);
   return direction * q + normal * (q * cos - root);
}

// https://stackoverflow.com/questions/42218704/how-to-properly-handle-refraction-in-raytracing
// glm::vec3 get_refract(
//               const vec3 & normal, 
//               const vec3 & direction,
//               double n1, 
//               double n2
// )
// {
//   double eta = n1 / n2;
//     eta = 2.0f - eta;
//     float cosi = dot(normal, direction);
//     return direction * eta - normal * (-cosi + eta * cosi);
// }

// https://github.com/chinux23/CS488-Ray-Tracer/blob/master/A4/A4.cpp
// glm::vec3 get_refract(
//               const vec3 & normal, 
//               const vec3 & direction,
//               double n1, 
//               double n2
// ){
  
//     double nr = n1 / n2;

//     double cosineTheta_i = -glm::dot(normal, direction);
//     double sineTheta_i_t2 = 1 - cosineTheta_i * cosineTheta_i;
//     double sineTheta_t_t2 = nr * nr * sineTheta_i_t2;
  
//     if (sineTheta_t_t2 > 1) {
//         // Total internal reflection.
//       return vec3(0);
//     }
  
// //    auto direction = (nr * cosineTheta_i - sqrt(1 - sineTheta_t_t2)) * intersection.normal - nr * ray.direction;
//   return nr * direction + (nr * cosineTheta_i - sqrt(1 - sineTheta_t_t2)) * normal;
// }

float absVec3(const vec3 & vec){
   float value = 0.0;
   for(int i = 0; i < 3; i++) value += vec[i] * vec[i];
   return sqrt(value);
}