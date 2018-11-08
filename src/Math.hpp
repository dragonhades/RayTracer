#include <glm/glm.hpp>

#define PI 3.1415926

glm::vec3 get_refract(
              const glm::vec3 & normal, 
              const glm::vec3 & direction,
              double n1, 
              double n2
);

float absVec3(const glm::vec3 & vec);