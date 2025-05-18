#pragma once

#include <vector>

#include "physics.hh"

class Tilemap;

enum class EdgeType {
	WALK,
	JUMP,
	FALL,
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

public:
	NavMesh(Tilemap& tilemap);

	void generate();
	void render();
};
