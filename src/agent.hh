#pragma once

#include <memory>

#include "physics.hh"

class Agent {
private:
	b2WorldId world;
	b2BodyId body;
	float radius;

public:
	const float width = 1.0;
	const float height = 2.0;

	const float max_speed = 5.0;
	const float jump_speed = 10.0;

	Agent();
	Agent(b2WorldId world, float x, float y);

	b2Vec2 get_position() const;
	void set_position(float x, float y);

	b2Vec2 get_velocity() const;
	void set_velocity(float x, float y);

	void update();
	void render();
};
