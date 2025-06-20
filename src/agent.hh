#pragma once

#include <memory>

#include "physics.hh"
#include "pathfinder.hh"

class Agent {
private:
	b2WorldId world;
	b2BodyId body;

	bool at(b2Vec2 p);
	bool at_x(b2Vec2 p);

public:
	const float width = 1.0;
	const float height = 2.0;

	const float max_speed = 5.0;
	const float jump_speed = 10.0;

	Path path;

	Agent();
	Agent(b2WorldId world, float x, float y);

	b2Vec2 get_position() const;
	void set_position(float x, float y);
	void set_position(b2Vec2 v);

	b2Vec2 get_velocity() const;
	void set_velocity(float x, float y);
	void set_velocity(b2Vec2 v);
	void move_towards(b2Vec2 point, float speed);

	void update();
	void render();
};
