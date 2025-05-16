#pragma once

#include <vector>

#include "physics.hh"
#include "tilemap.hh"

struct Direction {
	int x : 4 = 0;
	int y : 4 = 0;

	bool null() {
		return x == 0 && y == 0;
	}
};

class FlowMap {
private:
	std::vector<Direction> data;

	int width, height;

	std::vector<int> expand(const Tilemap& tilemap, const int i); // Returns indecies of the neighbors of tile i that are reachable
	void generate(Tilemap& tilemap, int x, int y);

public:
	FlowMap() = default;
	FlowMap(Tilemap& tilemap, int x, int y);
	FlowMap(Tilemap& tilemap);
	~FlowMap();

	int tile_index(const unsigned int x, const unsigned int y) const;
	b2Vec2 direction(b2Vec2 position); // Returns the direction at a world position
	void render();
};
