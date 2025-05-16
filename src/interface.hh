#pragma once

#include <raylib.h>
#include <raymath.h>

#include "tilemap.hh"
#include "agent.hh"
#include "nav_mesh.hh"

extern Camera2D camera;

void update_camera();
Vector2 get_target();
bool inside_map(const Tilemap& tilemap);
void get_input(Tilemap& tilemap, NavMesh& nav_mesh, Agent& agent);
