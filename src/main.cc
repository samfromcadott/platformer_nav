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
#include "pathfinder.hh"

using namespace std;

int main(int argc, char const *argv[]) {
	// Create window
	InitWindow(1280, 720, "Platformer Navigation Test");
	SetTargetFPS(60);

	auto world = init_world();

	Tilemap tilemap(world, 30, 30);
	NavMesh nav_mesh(tilemap);
	Agent agent(world, 10.0, 10.0);
	Pathfinder pathfinder(agent, nav_mesh);

	b2Vec2 closest = {-1000, -1000};

	while ( !WindowShouldClose() ) {
		update_world(world);
		update_camera();
		get_input(tilemap, nav_mesh, agent);

		agent.update();

		if ( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && nav_mesh.valid() ) {
			auto target = get_target();
			auto node = nav_mesh.get_closest( b2Vec2{target.x, target.y} );
			closest = node.position;
			agent.path = pathfinder.set_goal( b2Vec2{target.x, target.y} );
		}

		BeginDrawing();

			BeginMode2D(camera);

			ClearBackground(RAYWHITE);

			tilemap.render();
			nav_mesh.render();
			agent.render();
			pathfinder.render();
			DrawCircle(closest.x*world_scale, closest.y*world_scale, 4.0, ORANGE);

			EndMode2D();

			DrawFPS(10, 10);
		EndDrawing();
	}

	// Cleanup
	b2DestroyWorld(world);
	CloseWindow();
	return 0;
}
