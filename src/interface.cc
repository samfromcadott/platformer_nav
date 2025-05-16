#include "interface.hh"
#include "physics.hh"

#include <iostream>

Camera2D camera = {.offset = {0,0}, .target = {0,0}, .rotation = 0, .zoom = 1};
const float camera_speed = 300.0;

void update_camera() {
	Vector2 dir = {0,0};

	if ( IsKeyDown(KEY_W) ) dir.y = -1;
	if ( IsKeyDown(KEY_S) ) dir.y = +1;
	if ( IsKeyDown(KEY_A) ) dir.x = -1;
	if ( IsKeyDown(KEY_D) ) dir.x = +1;

	dir = Vector2Normalize(dir);
	dir = Vector2Scale( dir, camera_speed * GetFrameTime() );

	camera.target = Vector2Add(camera.target, dir);
}

bool inside_map(const Tilemap& tilemap) {
	Vector2 pos = Vector2Divide( GetScreenToWorld2D( GetMousePosition(), camera ), Vector2 {Tilemap::tile_size, Tilemap::tile_size} );
	return ( pos.x > 0 && pos.x < tilemap.get_width() ) && ( pos.y > 0 && pos.y < tilemap.get_height() );
}

Vector2 get_target() {
	return Vector2Divide( GetScreenToWorld2D( GetMousePosition(), camera ), Vector2 {world_scale, world_scale} );
}

void get_input(Tilemap& tilemap, NavMesh& nav_mesh, Agent& agent) {
	if ( IsKeyPressed(KEY_R) ) {
		nav_mesh.generate();
	}

	if ( !inside_map(tilemap) ) return;

	if ( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
		Vector2 coord = Vector2Divide( GetScreenToWorld2D( GetMousePosition(), camera ), Vector2 {Tilemap::tile_size, Tilemap::tile_size} );
		tilemap.toggle_tile(coord.x, coord.y);
		tilemap.generate_collision();
	}

	if ( IsKeyPressed(KEY_ONE) ) {
		auto p = get_target();
		agent.set_position(p.x, p.y);
		agent.set_velocity(0.0,0.0);
	}
}
