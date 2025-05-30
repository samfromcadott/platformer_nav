#pragma once

#include <vector>

#include "nav_mesh.hh"
#include "agent.hh"

struct PathSegment {
	b2Vec2 start;
	b2Vec2 velocity;
};

class Pathfinder {
private:
	Agent& agent;
	NavMesh& nav_mesh;

	struct PathNode {
		int node; // Index of node in nav_mesh
		int parent; // Index of parent in nav_mesh
		int edge; // Edge by which node connects to parent
		float cost;
	};

	bool in_list(const std::vector<PathNode>& list, const PathNode& node) const;
	int lowest_cost(const std::vector<PathNode>& list) const;
	float compute_cost(const int edge, const EdgeDirection direction) const;
	std::vector<PathNode> get_adjacent(const int node) const;
	bool can_connect(const int edge, const EdgeDirection direction) const;

public:
	std::vector<PathSegment> path;

	Pathfinder(Agent& agent, NavMesh& nav_mesh);

	void set_goal(b2Vec2 p);
};
