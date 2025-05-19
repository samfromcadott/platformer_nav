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

	// Create nodes
	for (int x = 0; x < tilemap.get_width(); x++)
	for (int y = 0; y < tilemap.get_height() - 1; y++) {
		if (tilemap(x, y) == Tile::WALL) continue; // Skip filled tiles
		if (tilemap(x, y+1) == Tile::EMPTY) continue; // Check if tile below is filled

		// If it's filled place a node there
		const b2Vec2 offset = b2Vec2 {0.5, 0.5};
		b2Vec2 p = b2Vec2 { static_cast<float>(x), static_cast<float>(y) } + offset;

		Node n;
		n.position = p;
		nodes.push_back(n);
	}

	// Create edges
	edges.reserve(nodes.size() * 4);

	for (int node = 0; node < nodes.size(); node++) {
		for (int other = 0; other < nodes.size(); other++) {
			if (node == other) continue; // You can't connect to yourself
			if ( has_connection(node, other) ) continue; // Check if already connected

			// Check for possible connections
			if ( can_walk(node, other) ) {
				Edge e;
				e.a = node;
				e.b = other;
				e.type = EdgeType::WALK;

				edges.push_back(e);
				nodes[node].edges.push_back(edges.size() - 1);
			}

			else if ( can_fall(node, other) ) {
				Edge e;
				e.a = node;
				e.b = other;
				e.type = EdgeType::FALL;

				edges.push_back(e);
				nodes[node].edges.push_back(edges.size() - 1);
			}
		}
	}

	nodes.shrink_to_fit();
	edges.shrink_to_fit();
}

void NavMesh::render() {
	// Draw a line between each node, lines are color coded based on type
	for (auto& edge : edges) {
		// Color based on edge type
		Color color;
		switch (edge.type) {
			case EdgeType::WALK:
				color = GREEN;
				break;
			case EdgeType::FALL:
				color = RED;
				break;
		}

		auto pa = nodes[edge.a].position * world_scale;
		auto pb = nodes[edge.b].position * world_scale;
		DrawLine(pa.x, pa.y, pb.x, pb.y, color);
	}

	// Draw a black dot at each node
	for (auto& node : nodes) {
		b2Vec2 p = node.position * world_scale;
		DrawCircle(p.x, p.y, 2.0, BLACK);
	}
}

bool NavMesh::has_connection(int a, int b) {
	for (auto edge : nodes[a].edges) {
		if (edges[edge].a == a && edges[edge].b == b) return true;
		if (edges[edge].b == a && edges[edge].a == b) return true;
	}

	for (auto edge : nodes[b].edges) {
		if (edges[edge].a == a && edges[edge].b == b) return true;
		if (edges[edge].b == a && edges[edge].a == b) return true;
	}

	return false;
}

bool NavMesh::can_walk(int a, int b) {
	auto pa = nodes[a].position;
	auto pb = nodes[b].position;

	if (pa.y != pb.y) return false;
	if ( abs(pa.x - pb.x) != 1.0 ) return false;

	return true;
}

bool NavMesh::can_fall(int a, int b) {
	// Get their integer coordinates
	TileCoord ta = nodes[a].position;
	TileCoord tb = nodes[b].position;

	if (ta.x == tb.x) return false; // Check if they are directly overhead
	if (ta.y == tb.y) return false; // Check if they are the same elevation
	if ( abs(ta.x-tb.x) > 1 ) return false; // Check if they are too far apart

	// Determine which is higher
	// Start is at x of lower tile and y of higher tile
	// End is at x and y of lower tile
	int x, start_y, end_y;
	if (ta.y < tb.y) { // Lower number is physically higher
		start_y = ta.y;
		end_y = tb.y;
		x = tb.x;
	} else {
		start_y = tb.y;
		end_y = ta.y;
		x = ta.x;
	}

	// Go from start to end checking for filled tiles
	for (int y = start_y; y < end_y; y++) {
		if (tilemap(x,y) == Tile::WALL) return false;
	}

	return true;
}
