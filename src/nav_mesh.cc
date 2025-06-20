#include <iostream>
#include <algorithm>

#include "nav_mesh.hh"
#include "tilemap.hh"

using namespace std;

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

	for (int node = 0; node < nodes.size(); node++)
	for (int other = 0; other < nodes.size(); other++) {
		if (node == other) continue; // You can't connect to yourself
		if ( has_connection(node, other) ) continue; // Check if already connected

		if ( can_walk(node, other) ) add_walk_edge(node, other);
		else if ( can_fall(node, other) ) add_fall_edge(node, other);
		else if ( can_jump(node, other) ) add_jump_edge(node, other);
	}

	nodes.shrink_to_fit();
	edges.shrink_to_fit();
}

void NavMesh::render() const {
	// Draw a line between each node, lines are color coded based on type
	for (const auto& edge : edges) {
		// Color based on edge type
		Color color;
		switch (edge.type) {
			case EdgeType::WALK:
				color = GREEN;
				break;
			case EdgeType::FALL:
				color = RED;
				break;
			case EdgeType::JUMP:
				color = BLUE;
				break;
		}

		if (edge.type == EdgeType::JUMP) {
			auto pa = nodes[edge.a].position;
			auto pb = nodes[edge.b].position;

			float lowest = min(pa.x, pb.x);
			float highest = max(pa.x, pb.x);
			float d = 0.1;

			for (float x = lowest+d; x < highest+d; x+=d) {
				b2Vec2 p0;
				p0.x = x-d;
				p0.y = projectile(edge.vel_ab, pa, p0.x);
				p0 *= world_scale;

				b2Vec2 p1;
				p1.x = x;
				p1.y = projectile(edge.vel_ab, pa, x);
				p1 *= world_scale;

				DrawLine(p0.x, p0.y, p1.x, p1.y, color);
			}
		}

		else {
			auto pa = nodes[edge.a].position * world_scale;
			auto pb = nodes[edge.b].position * world_scale;
			DrawLine(pa.x, pa.y, pb.x, pb.y, color);
		}
	}

	// Draw a black dot at each node
	for (const auto& node : nodes) {
		b2Vec2 p = node.position * world_scale;
		DrawCircle(p.x, p.y, 2.0, BLACK);
	}
}

int NavMesh::closest(b2Vec2 position) const {
	float dist = INFINITY;
	int n = -1; // Index of closest node

	for (int i = 0; i < nodes.size(); i++) {
		float d = b2Distance(position, nodes[i].position);

		if (d > dist) continue; // Move on if this isn't closer than dist

		dist = d;
		n = i;
	}

	return n;
}

const Node& NavMesh::get_closest(b2Vec2 position) const {
	return nodes[ closest(position) ];
}

bool NavMesh::valid() const {
	return nodes.size() > 0;
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

bool NavMesh::can_jump(int a, int b) {
	// Get their integer coordinates
	b2Vec2 pa = nodes[a].position;
	b2Vec2 pb = nodes[b].position;
	TileCoord ta = pa;
	TileCoord tb = pb;

	if (ta.x == tb.x) return false; // Check if they are directly overhead
	if (b2Distance(pa, pb) > max_jump_dist) return false; // Check if they're too far apart

	// Check if both points are the end of a platform
	bool a_on_end = tilemap(ta.x+1, ta.y+1) == Tile::EMPTY || tilemap(ta.x-1, ta.y+1) == Tile::EMPTY;
	bool b_on_end = tilemap(tb.x+1, tb.y+1) == Tile::EMPTY || tilemap(tb.x-1, tb.y+1) == Tile::EMPTY;
	if ( !a_on_end || !b_on_end ) return false;

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

b2Vec2 NavMesh::best_jump(b2Vec2 a, b2Vec2 b) {
	b2Vec2 velocity = {INFINITY,INFINITY};
	for (float s = 0.1; s < 2.0; s+=0.1) {
		auto v = jump_velocity(a, b, s); // Find velocity for s

		// Check if apex is between a and b
		auto apex = jump_apex(a, v);
		if ( apex.x < min(a.x, b.x) || apex.x > max(a.x, b.x) ) continue;

		// Check if apex is above higher point
		if ( abs(apex.y - min(a.y, b.y)) < 0.5 ) continue;

		// Check if v has a smaller length than velocity
		if ( b2Length(v) < b2Length(velocity) ) velocity = v;
	}

	return velocity;
}

b2Vec2 NavMesh::jump_velocity(b2Vec2 a, b2Vec2 b, float s) {
	b2Vec2 velocity = b2Vec2 {0,0};
	velocity.x = 1.0 / s;
	velocity.x = b.x - a.x < 0? -velocity.x : velocity.x;

	float duration = s * (b.x - a.x);
	velocity.y = (b.y - (0.5 * gravity * pow(duration, 2)) - a.y) / duration;
	velocity.y = -abs(velocity.y);

	return velocity;
}

b2Vec2 NavMesh::jump_apex(b2Vec2 a, b2Vec2 velocity) {
	b2Vec2 apex = b2Vec2 {0,0};

	apex.x = -(velocity.y/gravity) * velocity.x + a.x;
	apex.y = projectile(velocity, a, apex.x);

	return apex;
}

bool NavMesh::jump_collides(b2Vec2 a, b2Vec2 b, b2Vec2 velocity) {
	float lowest = min(a.x, b.x);
	float highest = max(a.x, b.x);

	for (float x = lowest; x <= highest; x+=0.1) {
		b2Vec2 p;
		p.x = x;
		p.y = projectile(velocity, a, x); // Get the hight of the jump arc at x
		TileCoord tile = p; // Convert coordinates to TileCoord

		// Check if there is a wall here the jump collides
		if (tilemap(tile.x, tile.y) == Tile::WALL) return true;
	}

	return false;
}

float NavMesh::projectile(b2Vec2 v, b2Vec2 p0, float x) const {
	float t = (x - p0.x) / v.x;
	float y = 0.5 * gravity * pow(t,2) + v.y * t + p0.y;
	return y;
}

void NavMesh::add_walk_edge(int a, int b) {
	Edge e;
	e.a = a;
	e.b = b;
	e.type = EdgeType::WALK;
	e.vel_ab = {1,0}; // TODO: Proper velocity
	e.vel_ba = {-1,0};

	edges.push_back(e);
	nodes[a].edges.push_back(edges.size() - 1);
	nodes[b].edges.push_back(edges.size() - 1);
}

void NavMesh::add_jump_edge(int a, int b) {
	b2Vec2 pa = nodes[a].position;
	b2Vec2 pb = nodes[b].position;

	// Calculate velocity
	b2Vec2 vel_ab = best_jump(pa, pb);
	b2Vec2 vel_ba = best_jump(pb, pa);

	if ( jump_collides(pa, pb, vel_ab) ) return;
	if ( jump_collides(pb, pa, vel_ba) ) return;

	// If the jump is good then add an edge
	Edge e;
	e.a = a;
	e.b = b;
	e.type = EdgeType::JUMP;
	e.vel_ab = vel_ab;
	e.vel_ba = vel_ba;

	edges.push_back(e);
	nodes[a].edges.push_back(edges.size() - 1);
	nodes[b].edges.push_back(edges.size() - 1);
}

void NavMesh::add_fall_edge(int a, int b) {
	b2Vec2 pa = nodes[a].position;
	b2Vec2 pb = nodes[b].position;

	Edge e;
	e.a = a;
	e.b = b;
	e.type = EdgeType::FALL;
	e.vel_ab = pa.y < pb.y? b2Vec2 {1,0} : best_jump(pa, pb);
	e.vel_ba = pb.y < pa.y? b2Vec2 {-1,0} : best_jump(pb, pa);

	edges.push_back(e);
	nodes[a].edges.push_back(edges.size() - 1);
	nodes[b].edges.push_back(edges.size() - 1);
}
