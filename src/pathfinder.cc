#include "pathfinder.hh"

Pathfinder::Pathfinder(Agent& agent, NavMesh& nav_mesh) : agent(agent), nav_mesh(nav_mesh) {

}

void Pathfinder::set_goal(b2Vec2 p) {
	// Implement A* path finding here
}

bool Pathfinder::in_list(const std::vector<PathNode>& list, const PathNode& node) const {
	return false;
}

float Pathfinder::compute_cost(const int a, const int b) const {
	return 0.0;
}

std::vector<Pathfinder::PathNode> Pathfinder::get_adjacent(const int node) const {
	return {};
}

bool Pathfinder::can_connect(const int edge, const EdgeDirection direction) const {
	return false;
}
