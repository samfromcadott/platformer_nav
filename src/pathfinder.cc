#include "pathfinder.hh"

using namespace std;

Pathfinder::Pathfinder(Agent& agent, NavMesh& nav_mesh) : agent(agent), nav_mesh(nav_mesh) {

}

void Pathfinder::set_goal(b2Vec2 p) {
	path.clear();

	PathNode start;
	int goal;

	std::vector<PathNode> open = {start};
	std::vector<PathNode> closed;

	while ( !open.empty() ) {
		int current = lowest_cost(open);
		if (open[current].node == goal) break;

		// Move current to closed
		closed.push_back( open[current] );
		open.erase(open.begin() + current);

		auto neighbors = get_adjacent(open[current].node); // Get adjacent to current

		for (auto& node : neighbors) {
			if ( in_list(open, node) ) continue;
			if ( in_list(closed, node) ) continue;

			open.push_back(node);
		}
	}
}

bool Pathfinder::in_list(const std::vector<PathNode>& list, const PathNode& node) const {
	for (const auto& n : list)
		if (n.node == node.node) return true;

	return false;
}

int Pathfinder::lowest_cost(const std::vector<PathNode>& list) const {
	float cost = INFINITY;
	int n = -1; // Index of cheapest node

	for (int i = 0; i < list.size(); i++) {
		float c = list[i].cost;

		if (c > cost) continue; // Move on if this isn't cheaper than cost

		cost = c;
		n = i;
	}

	return n;
}

float Pathfinder::compute_cost(const int edge, const EdgeDirection direction) const {
	const Edge& e = nav_mesh.edges[edge];
	const b2Vec2 pa = nav_mesh.nodes[e.a].position;
	const b2Vec2 pb = nav_mesh.nodes[e.b].position;

	float dx = abs(pa.x - pb.x);
	float dy = abs(pa.y - pb.y);

	float time, vx;

	switch (e.type) {
		case EdgeType::WALK:
			time = dx / agent.max_speed;
			break;
		case EdgeType::JUMP:
			vx = direction == EdgeDirection::A_TO_B? e.vel_ab.x : e.vel_ba.x;
			time = dx / abs(vx);
			break;
		case EdgeType::FALL:
			time = sqrt( 2 *  dy/abs(nav_mesh.gravity) );
			break;
	}

	return time;
}

std::vector<Pathfinder::PathNode> Pathfinder::get_adjacent(const int node) const {
	vector<PathNode> adjacent;

	for (const int edge : nav_mesh.nodes[node].edges) {
		EdgeDirection direction = node == nav_mesh.edges[edge].a? EdgeDirection::A_TO_B : EdgeDirection::B_TO_A;
		if ( !can_connect(edge, direction) ) continue;

		int other = direction == EdgeDirection::A_TO_B? nav_mesh.edges[edge].b : nav_mesh.edges[edge].a;

		adjacent.push_back( PathNode {other, node, edge, compute_cost(edge, direction) });
	}

	return adjacent;
}

bool Pathfinder::can_connect(const int edge, const EdgeDirection direction) const {
	const Edge& e = nav_mesh.edges[edge];

	if (direction == EdgeDirection::A_TO_B) {
		if (abs(e.vel_ab.x) <= agent.max_speed && e.vel_ab.y <= agent.jump_speed)
			return true;
	}

	else if (direction == EdgeDirection::B_TO_A) {
		if (abs(e.vel_ba.x) <= agent.max_speed && e.vel_ba.y <= agent.jump_speed)
			return true;
	}

	return false;
}
