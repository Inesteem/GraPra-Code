
#ifndef __MOUSEACTIONS_H__ 
#define __MOUSEACTIONS_H__ 


#include <vector>
#include <math.h>
#include <unistd.h>

#include <camera.h>

#include <shader.h>
#include <mesh.h>
#include <texture.h>

#include <list>

#include <cairo/cairo.h>

#include "rendering.h"

#include <vector>
#include <math.h>
#include <algorithm>
#include <queue>
#include <string>
#include <sstream>
#include <cstdlib>

namespace moac {

void getScreenpos(vec2f* in,vec2f* out);
vec3f ClickWorldPosition(int screenX, int screenY);
}

#endif

