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
	direction = atan2f(directionalVector.x, directionalVector.y)* RAD2DEG;

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
	// to shift model for better rotation
	model.transform = MatrixTranslate(-25.f, 0, -25.f);
	DrawModelEx(model, position, { 0, 1.f, 0 }, direction, { 0.05f, 0.05f, 0.05f }, WHITE);
	DrawSphere(position, 1, RED);
	
}



Brigadier::Brigadier(Vector3 position, Map* map, Model model)
	:Unit(position, map, model)
{
	/*for(int i = 0; i<maxNumOfDiggers; i++)
		siblings[i] = Digger(position, map, )*/
}

Brigadier::~Brigadier()
{
}

void Brigadier::Update()
{
}

void Brigadier::Draw()
{
}

TileIndex Brigadier::getTileToTerraform(bool state)
{
	TileIndex result = { -1, -1 };
	TileIndex center = positionTile;
	TileIndex mapSize = map->getMapSize();
	int maxRadius = std::max<int>(
		std::max<int>((center.x), (mapSize.x - center.x - 1)),
		std::max<int>((center.z), (mapSize.z - center.z - 1))
		);

	TileIndex leftUp, rightDown;

	//TODO: check current TileIndex

	for (int r = 1; r < maxRadius; r++)
	{
		//TODO: checks bounds of map IN LOOPS INITIALISATIONS - USE CLAMPS?
		for (int x = center.x - r, int z = center.z-r; x < center.x + r; x++)
		{
			
			//TODO: check for tile with desired properties
		}

		for (int x = center.x + r, int z = center.z - r + 1; z < center.z + r; z++)
		{
			//TODO: checks bounds of map
			//TODO: check for tile with desired properties
		}


	}



	return result;
}
