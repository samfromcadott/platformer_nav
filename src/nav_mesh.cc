#include <iostream>

#include "nav_mesh.hh"
#include "tilemap.hh"

NavMesh::NavMesh(Tilemap& tilemap) : tilemap(tilemap) {
	generate();
}

void NavMesh::generate() {
	nodes.clear();
	edges.clear();

	nodes.reserve( tilemap.get_width() * tilemap.get_height() );

	for (int x = 0; x < tilemap.get_width(); x++)
	for (int y = 0; y < tilemap.get_height() - 1; y++) {
		if (tilemap(x, y) == Tile::WALL) continue; // Skip filled tiles
		if (tilemap(x, y+1) == Tile::EMPTY) continue; // Check if tile below is filled

		// If it's filled place a node there
		const b2Vec2 offset = b2Vec2 {0.5, 0.5};
		b2Vec2 p = b2Vec2 { static_cast<float>(x), static_cast<float>(y) } + offset;

		Node n = Node { .position = b2Vec2{p.x,p.y} };
		nodes.push_back(n);
	}

	nodes.shrink_to_fit();
}

void NavMesh::render() {
	// Draw a black dot at each node
	for (auto& node : nodes) {
		b2Vec2 p = node.position * world_scale;
		DrawCircle(p.x, p.y, 2.0, BLACK);
	}

	// Draw a line between each node, lines are color coded based on type
	for (auto& edge : edges) {

	}
}
