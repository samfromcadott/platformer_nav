#include <algorithm>

#include "tilemap.hh"

Tilemap::Tilemap(b2WorldId world, unsigned int width, unsigned int height) {
	tiles = new Tile[width * height];
	std::fill_n(tiles, width * height, Tile::EMPTY);

	this->world = world;
	this->width = width;
	this->height = height;

	// b2BodyDef body_def = b2DefaultBodyDef();
	// body = b2CreateBody(world, &body_def);
	generate_collision();
}

Tilemap::~Tilemap() {
	delete[] tiles;
}

int Tilemap::tile_index(const unsigned int x, const unsigned int y) const {
	return width * y + x;
}

std::tuple<int, int> Tilemap::tile_coord(const int i) const {
	return {i%width, i/width};
}

Tile Tilemap::operator()(const unsigned int x, const unsigned int y) const {
	return tiles[ tile_index(x, y) ];
}

Tile& Tilemap::operator()(const unsigned int x, const unsigned int y) {
	return tiles[ tile_index(x, y) ];
}

int Tilemap::get_width() const {
	return width;
}

int Tilemap::get_height() const {
	return height;
}

Vector2 Tilemap::tile_to_world(unsigned int x, unsigned int y) {
	float size = static_cast<float>(tile_size);
	return Vector2 {x*size, y*size};
}

void Tilemap::toggle_tile(int x, int y) {
	(*this)(x,y) = (*this)(x,y) == Tile::WALL?  Tile::EMPTY : Tile::WALL;
}

void Tilemap::generate_collision() {
	// Destroy and recreate the body
	if ( b2Body_IsValid(body) ) b2DestroyBody(body);
	b2BodyDef body_def = b2DefaultBodyDef();
	body = b2CreateBody(world, &body_def);

	// Add shapes to the body
	for (int x = 0; x < width; x++)
	for (int y = 0; y < height; y++) {
		if ( (*this)(x,y) == Tile::WALL ) add_collider(x, y);
	}

	// Add the outer walls
	b2Segment wall_top = {{0,0}, {width, 0}};
	b2Segment wall_bottom = {{0,height}, {width, height}};
	b2Segment wall_left = {{0,0}, {0, height}};
	b2Segment wall_right = {{width,0}, {width, height}};

	auto shape_top = b2DefaultShapeDef();
	auto shape_bottom = b2DefaultShapeDef();
	auto shape_left = b2DefaultShapeDef();
	auto shape_right = b2DefaultShapeDef();

	b2CreateSegmentShape(body, &shape_top, &wall_top);
	b2CreateSegmentShape(body, &shape_bottom, &wall_bottom);
	b2CreateSegmentShape(body, &shape_left, &wall_left);
	b2CreateSegmentShape(body, &shape_right, &wall_right);
}

void Tilemap::render() {
	for (int x = 0; x < width; x++)
	for (int y = 0; y < height; y++) {
		if ( (*this)(x,y) == Tile::WALL ) render_tile(x, y);
	}

	// Draw a border around the edge
	DrawLine(0,0, width*tile_size, 0, SKYBLUE);
	DrawLine(0,height*tile_size, width*tile_size, height*tile_size, SKYBLUE);
	DrawLine(0,0, 0, height*tile_size, SKYBLUE);
	DrawLine(width*tile_size,0, width*tile_size, height*tile_size, SKYBLUE);
}

void Tilemap::add_collider(unsigned int x, unsigned int y) {
	float size = static_cast<float>(tile_size)/2.0f/world_scale;

	Vector2 center = tile_to_world(x,y);
	center = Vector2Divide( center, Vector2 {world_scale, world_scale} );

	b2Polygon box = b2MakeOffsetBox(size, size, b2Vec2 {center.x + size, center.y + size}, 0.0f);

	b2ShapeDef shape_def = b2DefaultShapeDef();
	b2CreatePolygonShape(body, &shape_def, &box);
}

void Tilemap::render_tile(unsigned int x, unsigned int y) {
	DrawRectangleV( tile_to_world(x,y), {tile_size, tile_size}, GRAY );
}
