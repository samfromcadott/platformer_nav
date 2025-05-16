#pragma once

#include <tuple>
#include <raylib.h>
#include <raymath.h>

#include "physics.hh"

enum class Tile {
	EMPTY,
	WALL,
};

class Tilemap {
private:
	b2WorldId world;
	b2BodyId body;

	Tile* tiles;

	unsigned int width, height;

	void add_collider(unsigned int x, unsigned int y);
	void render_tile(unsigned int x, unsigned int y);

	friend class FlowMap;

public:
	static const int tile_size = 32;

	Tilemap(b2WorldId world, unsigned int width, unsigned int height);
	~Tilemap();

	int tile_index(const unsigned int x, const unsigned int y) const;
	std::tuple<int, int> tile_coord(const int i) const;
	Tile operator()(const unsigned int x, const unsigned int y) const;
	Tile& operator()(const unsigned int x, const unsigned int y);
	int get_width() const;
	int get_height() const;

	Vector2 tile_to_world(unsigned int x, unsigned int y);

	void toggle_tile(int x, int y);
	void generate_collision();

	void render();
};
