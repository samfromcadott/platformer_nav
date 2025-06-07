#pragma once

#include <vector>
#include <deque>

#include "nav_mesh.hh"

class Agent;

struct PathSegment {
	b2Vec2 start;
	b2Vec2 velocity;
};

typedef std::deque<PathSegment> Path;

class Pathfinder {
private:
	Agent& agent;
	NavMesh& nav_mesh;

	struct PathNode {
		int node; // Index of node in nav_mesh
		int parent; // Index of parent in closed list
		int edge; // Edge by which node connects to parent
		float cost; // Time to get to this node from start
		float distance; // Linear distance from goal
	};

	bool in_list(const std::vector<PathNode>& list, const PathNode& node) const;
	int lowest_cost(const std::vector<PathNode>& list) const;
	float compute_cost(const int edge, const EdgeDirection direction) const;
	std::vector<PathNode> get_adjacent(const int node) const;
	bool can_connect(const int edge, const EdgeDirection direction) const;
	Path build_path(const std::vector<PathNode>& list, int goal);

public:
	Path path;

	Pathfinder(Agent& agent, NavMesh& nav_mesh);

	Path set_goal(b2Vec2 p);
	void render();
};
