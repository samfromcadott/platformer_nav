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
		}
	}

	nodes.shrink_to_fit();
	edges.shrink_to_fit();

	std::cout << edges.size() << '\n';
}

void NavMesh::render() {
	// Draw a line between each node, lines are color coded based on type
	for (auto& edge : edges) {
		auto pa = nodes[edge.a].position * world_scale;
		auto pb = nodes[edge.b].position * world_scale;
		DrawLine(pa.x, pa.y, pb.x, pb.y, GREEN);
	}

	// Draw a black dot at each node
	for (auto& node : nodes) {
		b2Vec2 p = node.position * world_scale;
		DrawCircle(p.x, p.y, 2.0, BLACK);
	}
}

bool NavMesh::has_connection(int a, int b) {
	for (auto edge : nodes[a].edges) {
		if (edges[edge].a == a || edges[edge].a == b) return true;
		if (edges[edge].b == a || edges[edge].b == b) return true;
	}

	for (auto edge : nodes[b].edges) {
		if (edges[edge].a == a || edges[edge].a == b) return true;
		if (edges[edge].b == a || edges[edge].b == b) return true;
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
