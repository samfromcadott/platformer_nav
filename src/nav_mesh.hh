#pragma once

#include <vector>

#include "physics.hh"

class Tilemap;
class Pathfinder;

enum class EdgeType {
	WALK,
	JUMP,
	FALL,
};

enum class EdgeDirection {
	A_TO_B,
	B_TO_A,
};

struct Node {
	b2Vec2 position;
	std::vector<int> edges; // Edges that connect to this node
};

struct Edge {
	int a = -1, b = -1; // Indecies of nodes this connects
	EdgeType type;
	b2Vec2 vel_ab;
	b2Vec2 vel_ba;
};

class NavMesh {
private:
	Tilemap& tilemap;
	std::vector<Node> nodes;
	std::vector<Edge> edges;

	bool can_walk(int a, int b);
	bool can_jump(int a, int b);
	bool can_fall(int a, int b);
	bool has_connection(int a, int b);
	int closest(b2Vec2 position) const;

	b2Vec2 jump_velocity(b2Vec2 a, b2Vec2 b, float s);
	b2Vec2 jump_apex(b2Vec2 a, b2Vec2 velocity);
	bool jump_collides(b2Vec2 a, b2Vec2 b, b2Vec2 velocity);

	float projectile(b2Vec2 v, b2Vec2 p0, float x) const;

public:
	float gravity = 10.0;
	float max_jump_dist = 10.0;

	NavMesh(Tilemap& tilemap);

	void generate();
	void render() const;
	const Node& get_closest(b2Vec2 position) const;
	bool valid() const;

	friend class Pathfinder;
};
