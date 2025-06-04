#include <iostream>
#include <algorithm>
#include <raylib.h>

#include "pathfinder.hh"

using namespace std;

Pathfinder::Pathfinder(Agent& agent, NavMesh& nav_mesh) : agent(agent), nav_mesh(nav_mesh) {

}

void Pathfinder::render() {
	if ( path.size() == 0 ) return;

	for (int i = 0; i < path.size() - 1; i++) {
		b2Vec2 p0 = path[i].start * world_scale;
		b2Vec2 p1 = path[i+1].start * world_scale;

		DrawCircle(p0.x, p0.y, 2, ORANGE);
		DrawLine(p0.x, p0.y, p1.x, p1.y, ORANGE);
	}
}

void Pathfinder::set_goal(b2Vec2 p) {
	path.clear();

	PathNode start;
	int start_node = nav_mesh.closest( agent.get_position() );
	int goal;

	start = PathNode { start_node, -1, -1, 0.0, b2Distance(agent.get_position(), p) };
	goal = nav_mesh.closest(p);

	std::vector<PathNode> open;
	std::vector<PathNode> closed;
	bool goal_reached = false;

	open.reserve( nav_mesh.nodes.size() ); closed.reserve( nav_mesh.nodes.size() );

	open.push_back(start);

	// A* search algorithm
	while ( !open.empty() ) {
		// Find the cheapest node
		int cheapest = lowest_cost(open);

		// Move cheapest to closed
		closed.push_back( open[cheapest] );
		open.erase(open.begin() + cheapest);
		int current = closed.size() - 1; // Index of current in closed list

		// If the goal has been reached search is complete
		if (closed[current].node == goal) {
			goal_reached = true;
			break;
		}

		// Search through connected nodes
		auto neighbors = get_adjacent(closed[current].node); // Get adjacent to current

		for (auto node : neighbors) {
			if ( in_list(open, node) ) continue;
			if ( in_list(closed, node) ) continue;

			node.parent = current;
			node.distance = b2Distance(nav_mesh.nodes[node.node].position, p);
			node.cost += closed[current].cost;
			open.push_back(node);
		}
	}

	// Build the path
	int end = goal_reached? closed.size() - 1 : [&]{
		// Find node closest to the goal
		float dist = INFINITY;
		int n = -1; // Index of closest node

		for (int i = 0; i < closed.size(); i++) {
			if (closed[i].distance > dist) continue; // Move on if this is farther than dist

			dist = closed[i].distance;
			n = i;
		}

		return n;
	}();

	path = build_path(closed, end);
}

std::vector<PathSegment> Pathfinder::build_path(const std::vector<PathNode>& list, int goal) {
	std::vector<PathSegment> p;

	int node = goal;
	while (true) {
		b2Vec2 start = nav_mesh.nodes[ list[node].node ].position;
		b2Vec2 velocity = {0,0};

		PathSegment segment = {start,velocity};

		p.push_back(segment);

		node = list[node].parent;
		if (node == -1) break;
	}

	reverse(p.begin(), p.end());

	return p;
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
		float c = list[i].cost + list[i].distance;

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

		PathNode new_node;
		new_node.node = other;
		new_node.edge = edge;
		new_node.cost = compute_cost(edge, direction);
		adjacent.push_back(new_node);
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
