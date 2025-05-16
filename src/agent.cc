#include <iostream>
#include <raylib.h>

#include "agent.hh"
#include "flow_map.hh"

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

	this->radius = radius;
}

b2Vec2 Agent::get_position() const {
	return b2Body_GetPosition(body);
}

void Agent::set_position(float x, float y) {
	b2Body_SetTransform( body, b2Vec2 {x, y}, b2Body_GetRotation(body) );
}

b2Vec2 Agent::get_velocity() const {
	return b2Body_GetLinearVelocity(body);
}

void Agent::set_velocity(float x, float y) {
	b2Vec2 dv = b2Vec2 {x, y} - get_velocity();
	b2Vec2 impulse = dv * b2Body_GetMass(body);
	b2Body_ApplyLinearImpulseToCenter(body, impulse, true);
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
}

void Agent::update() {

}
