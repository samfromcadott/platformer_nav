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
};

struct Edge {
	int a = -1, b = -1;
	EdgeType type;
};

class NavMesh {
private:
	Tilemap& tilemap;
	std::vector<Node> nodes;
	std::vector<Edge> edges;

public:
	NavMesh(Tilemap& tilemap);

	void generate();
	void render();
};
