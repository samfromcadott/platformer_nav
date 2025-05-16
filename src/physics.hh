#pragma once

#include <box2d/box2d.h>

const float world_scale = 32.0;
const int sub_steps = 16;

b2WorldId init_world();
void update_world(b2WorldId world);
