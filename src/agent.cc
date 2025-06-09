#include <iostream>
#include <raylib.h>

#include "agent.hh"
#include "pathfinder.hh"

Agent::Agent(b2WorldId world, float x, float y) {
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.position = {x, y};
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;

	body = b2CreateBody(world, &bodyDef);

	b2Polygon box = b2MakeBox(width/2.0, height/2.0);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.friction = 0.0f;
	b2CreatePolygonShape(body, &shapeDef, &box);
}

b2Vec2 Agent::get_position() const {
	return b2Body_GetPosition(body);
}

void Agent::set_position(float x, float y) {
	b2Body_SetTransform( body, b2Vec2 {x, y}, b2Body_GetRotation(body) );
}

void Agent::set_position(b2Vec2 v) {
	set_position(v.x, v.y);
}

b2Vec2 Agent::get_velocity() const {
	return b2Body_GetLinearVelocity(body);
}

void Agent::set_velocity(float x, float y) {
	b2Vec2 dv = b2Vec2 {x, y} - get_velocity();
	b2Vec2 impulse = dv * b2Body_GetMass(body);
	b2Body_ApplyLinearImpulseToCenter(body, impulse, true);
}

void Agent::set_velocity(b2Vec2 v) {
	set_velocity(v.x, v.y);
}

void Agent::render() {
	auto position = get_position();

	DrawRectangle(
		(position.x-width/2.0)*world_scale,
		(position.y-height/2.0)*world_scale,
		width*world_scale,
		height*world_scale,
		PURPLE
	);

	// Render Path
	if ( path.size() == 0 ) return;

	for (int i = 0; i < path.size() - 1; i++) {
		b2Vec2 p0 = path[i].start * world_scale;
		b2Vec2 p1 = path[i+1].start * world_scale;

		DrawCircle(p0.x, p0.y, 2, ORANGE);
		DrawLine(p0.x, p0.y, p1.x, p1.y, ORANGE);
	}
}

void Agent::update() {
	if (path.size() == 0) return;

	// If current segment is not a jump
	if (path[0].velocity.y == 0.0) {
		float vx = path[0].velocity.x * max_speed;
		float vy = get_velocity().y;
		set_velocity(vx, vy); // Move in the segment's direction
	}

	// If the agent gets to the next segment
	if ( path.size() > 1 && at(path[1].start) ) {
		path.pop_front();
		set_velocity(path[0].velocity);
	}
}

bool Agent::at(b2Vec2 p) {
	float dx = abs(get_position().x - p.x);
	float dy = abs(get_position().y - p.y);

	return dx < 0.1 && dy < 0.75;
}
