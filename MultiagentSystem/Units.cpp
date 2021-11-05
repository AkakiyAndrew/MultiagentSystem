#include "Units.h"

Unit::Unit(Vector3 position, Map* map, Model model)
	:position(position), map(map), model(model), speed(0.25f), targetPosition(position)
{
	positionTile = map->getTileIndexFromVector(position);
	targetPositionTile = positionTile;
}

Unit::~Unit()
{
}

Vector3 Unit::getPosition()
{
	return position;
}

void Unit::setTargetPosition(Vector3 newTarget)
{
	targetPosition = newTarget;
	targetPositionTile = map->getTileIndexFromVector(newTarget);

	Vector2 buf = { targetPosition.x - position.x, targetPosition.z - position.z };
	float vectorLength = sqrtf(pow(buf.x, 2) + pow(buf.y, 2));
	directionalVector = { buf.x / vectorLength, buf.y / vectorLength };
	direction = Vector2Angle(position, targetPosition);

	state = State::MOVING;
}

void Unit::Move()
{
	position.x += directionalVector.x * speed;
	position.z += directionalVector.y * speed;

	positionTile = map->getTileIndexFromVector(position);
	
	
}

Digger::Digger(Vector3 position, Map* map, Model model)
	:Unit(position, map, model)
{

}

Digger::~Digger()
{
}

void Digger::Update()
{
	//in any state change y-axis (height)
	position.y = map->getActualHeight(positionTile.x, positionTile.z) * 1.1;

	switch (state)
	{
	case State::IDLE:
		if (positionTile != targetPositionTile)
		{
			//get directional vector
			
		}
		else
		{
			//checking for tiles around Brigadier to terraform
		}
		break;
	case State::MOVING:
		if (positionTile == targetPositionTile)
		{
			state = State::IDLE;
		}
		else
		{
			Move();
		}
		break;
	case State::TERRAFORMING:
		break;
	default:
		break;
	}

}

void Digger::Draw()
{
	DrawModelEx(model, position, { 0, 1.f, 0 }, direction, { 0.05f, 0.05f, 0.05f}, WHITE);
	//DrawModel(model, position, 0.25f, WHITE);
}
