#include <iostream>
#include <vector>
#include <random>
#include <box2d/box2d.h>
#include <raylib.h>
#include <raymath.h>

#include "physics.hh"
#include "agent.hh"
#include "interface.hh"
#include "tilemap.hh"
#include "flow_map.hh"

using namespace std;

int main(int argc, char const *argv[]) {
	// Create window
	InitWindow(800, 600, "RTS Movement Test");
	SetTargetFPS(60);

	auto world = init_world();

	// Create the tilemap
	Tilemap tilemap(world, 30, 30);
	Agent agent(world, 10.0, 10.0);

	while ( !WindowShouldClose() ) {
		update_world(world);
		update_camera();
		get_input(tilemap, agent);

		agent.update();

		BeginDrawing();

			BeginMode2D(camera);
			ClearBackground(RAYWHITE);
			tilemap.render();
			agent.render();
			EndMode2D();

			DrawFPS(10, 10);
		EndDrawing();
	}

	// Cleanup
	b2DestroyWorld(world);
	CloseWindow();
	return 0;
}
