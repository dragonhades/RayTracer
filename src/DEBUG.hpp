#include <iostream>
#include <glm/gtx/string_cast.hpp>


#ifndef DEBUG
#define DEBUG 1
#endif

#define DASSERT(b, str) \
if(DEBUG==1){ \
if(b == false){ \
  std::cerr << "[DEBUG] ASSERTION FAIL: " << str << std::endl; \
    assert(b); \
  } \
}

#define DPRINT(str) if(DEBUG==1){ std::cerr << "[DEBUG] " << str << std::endl; }

#define DCOUNT(msg) static uint count=0; if(DEBUG==1){ count++; std::cerr << "[DEBUG] " << msg << " " << count << std::endl; }

#define STR(str) glm::to_string(str)

#define DPRINTVEC(str) if(DEBUG==1){ std::cerr << "[DEBUG] " << STR(str) << std::endl; }