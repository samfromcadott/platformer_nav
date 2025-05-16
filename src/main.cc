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
#include "nav_mesh.hh"

using namespace std;

int main(int argc, char const *argv[]) {
	// Create window
	InitWindow(1280, 720, "Platformer Navigation Test");
	SetTargetFPS(60);

	auto world = init_world();

	// Create the tilemap
	Tilemap tilemap(world, 30, 30);
	NavMesh nav_mesh(tilemap);
	Agent agent(world, 10.0, 10.0);

	// nav_mesh.nodes.push_back( Node { .position = b2Vec2{0.5,0.5} } );

	while ( !WindowShouldClose() ) {
		update_world(world);
		update_camera();
		get_input(tilemap, nav_mesh, agent);

		agent.update();

		BeginDrawing();

			BeginMode2D(camera);

			ClearBackground(RAYWHITE);

			tilemap.render();
			nav_mesh.render();
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
