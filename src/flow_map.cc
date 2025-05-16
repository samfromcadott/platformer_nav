#include "flow_map.hh"

FlowMap::FlowMap(Tilemap& tilemap, int x, int y) {
	width = tilemap.get_width();
	height = tilemap.get_height();

	data.resize(width * height);

	generate(tilemap, x, y);
}

FlowMap::FlowMap(Tilemap& tilemap) {
	width = tilemap.get_width();
	height = tilemap.get_height();

	data.resize(width * height);
}

FlowMap::~FlowMap() {

}

int FlowMap::tile_index(const unsigned int x, const unsigned int y) const {
	return width * y + x;
}

void FlowMap::generate(Tilemap& tilemap, int x, int y) {
	const int goal = tile_index(x, y); // Index of goal tile

	std::vector<int> open; // Queue of tiles that need to be evaluated
	open.reserve(width*height);
	open.push_back(goal); // Add the goal tile

	for (int i = 0; i < open.size(); i++) {
		int node = open[i]; // Get next node

		// Add neighbor tiles
		std::vector<int> neighbors = expand(tilemap, node);

		for (auto neighbor : neighbors) {
			if ( !data[neighbor].null() ) continue; // Skip tile if already evaluated
			if ( tilemap.tiles[neighbor] == Tile::WALL ) continue; // Skip walls
			if ( neighbor == goal ) continue; // Don't revealuate the goal

			open.push_back(neighbor);

			// Get the direction from node to neighbor
			auto [ax, ay] = tilemap.tile_coord(node);
			auto [bx, by] = tilemap.tile_coord(neighbor);
			data[neighbor].x = ax - bx;
			data[neighbor].y = ay - by;
		}
	}
}

std::vector<int> FlowMap::expand(const Tilemap& tilemap, const int i) {
	auto [x, y] = tilemap.tile_coord(i);
	std::vector<int> neighbors;
	neighbors.reserve(8);

	// Check if tile has empty neighbor in each cardinal direction
	bool north = y > 0 && tilemap(x, y-1) == Tile::EMPTY;
	bool south = y < height-1 && tilemap(x, y+1) == Tile::EMPTY;
	bool west = x > 0 && tilemap(x-1, y) == Tile::EMPTY;
	bool east = x < width-1 && tilemap(x+1, y) == Tile::EMPTY;

	if (north) neighbors.push_back( tile_index(x, y-1) );
	if (south) neighbors.push_back( tile_index(x, y+1) );
	if (west) neighbors.push_back( tile_index(x-1, y) );
	if (east) neighbors.push_back( tile_index(x+1, y) );

	// Check if diagonal neighbors are open
	if (north && west) neighbors.push_back( tile_index(x-1, y-1) );
	if (south && east) neighbors.push_back( tile_index(x+1, y+1) );
	if (north && east) neighbors.push_back( tile_index(x+1, y-1) );
	if (south && west) neighbors.push_back( tile_index(x-1, y+1) );

	return neighbors;
}

b2Vec2 FlowMap::direction(b2Vec2 position) {
	const float scale = world_scale / Tilemap::tile_size;

	Vector2 pos = Vector2Scale( Vector2 {position.x, position.y}, scale );
	if (( pos.x < 0 || pos.x > width ) || ( pos.y < 0 || pos.y > height ))
		return b2Vec2 {0,0};

	Direction d = data[ tile_index(pos.x,pos.y) ];

	Vector2 dir = {d.x, d.y};
	dir = Vector2Normalize(dir);

	return b2Vec2 {dir.x, dir.y};
}

void FlowMap::render() {

	for (int x = 0; x < width; x++)
	for (int y = 0; y < height; y++) {
		float half_size = Tilemap::tile_size / 2;
		Vector2 origin = {x * Tilemap::tile_size + half_size, y * Tilemap::tile_size + half_size};
		Direction d = data[tile_index(x,y)];
		Vector2 dir = {d.x, d.y};
		dir = Vector2Normalize(dir);
		dir = Vector2Scale(dir, 10);
		Vector2 end = Vector2Add(origin, dir);
		DrawLineV(origin, end, GREEN);
	}
}
