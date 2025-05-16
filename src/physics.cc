#include <raylib.h>

#include "physics.hh"

b2WorldId init_world() {
	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = b2Vec2 {0.0f, 10.0f};

	b2WorldId world = b2CreateWorld(&world_def);
	return world;
}

void update_world(b2WorldId world) {
	b2World_Step(world, GetFrameTime(), sub_steps);
}
