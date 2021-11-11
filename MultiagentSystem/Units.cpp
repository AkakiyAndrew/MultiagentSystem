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



Brigadier::Brigadier(Vector3 position, Map* map, Model model, Model diggerModel)
	:Unit(position, map, model)
{
	for (int i = 0; i < maxNumOfDiggers; i++)
		siblings[i] = Digger(position, map, diggerModel);
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

//aux function for method below
bool checkTileForTerraform(bool checkUnplannedTerrain, bool heightState, Map *map, int x, int z, short zeroLayerLevel)
{
	if (checkUnplannedTerrain == map->getTerraformPlanState(x, z))
	{
		if (heightState)
		{
			if (zeroLayerLevel > map->getHeight(x, z))
				return true;
		}
		else
		{
			if (zeroLayerLevel < map->getHeight(x, z))
				return true;
		}
	}

	return false;
}

// returns TileIndex with desired state (true for below-zeroLayer) and is tile must be in planned zone (true if yes)
TileIndex Brigadier::getTileToTerraform(bool heightState, bool checkUnplannedTerrain)
{
	TileIndex result = { -1, -1 };
	TileIndex center = positionTile;
	TileIndex mapSize = map->getMapSize();
	short zeroLayerLevel = map->getZeroLayerLevel();
	int maxRadius = std::max<int>(
		std::max<int>(center.x, mapSize.x - center.x - 1),
		std::max<int>(center.z, mapSize.z - center.z - 1)
		);

	TileIndex leftUp, rightDown;

	//checking current tile
	if (checkTileForTerraform(checkUnplannedTerrain, heightState, map, center.x, center.z, zeroLayerLevel))
		return TileIndex{ center.x, center.z };

	//TODO: add checking for assignments of other Diggers, who already got their targetTiles,
	//to prevent sending them to only one tile
	//OR
	//make bool matrix for assignments

	for (int r = 1; r <= maxRadius; r++)
	{
		//leftUp
		leftUp = {
			center.x - r,
			center.z - r
		};
		leftUp.x = std::clamp<int>(leftUp.x, 0, center.x);
		leftUp.z = std::clamp<int>(leftUp.z, 0, center.z);
		//rightDown
		rightDown = {
			center.x + r,
			center.z + r
		};
		rightDown.x = std::clamp<int>(rightDown.x, center.x, mapSize.x - 1);
		rightDown.z = std::clamp<int>(rightDown.z, center.z, mapSize.z - 1);

		//upper row
		for (int x = leftUp.x + 1, z = leftUp.z; x <= rightDown.x; x++)
		{
			if (checkTileForTerraform(checkUnplannedTerrain, heightState, map, x, z, zeroLayerLevel))
				return TileIndex{ x,z };
		}

		//right column
		for (int x = rightDown.x, z = leftUp.z + 1; z <= rightDown.z; z++)
		{
			if (checkTileForTerraform(checkUnplannedTerrain, heightState, map, x, z, zeroLayerLevel))
				return TileIndex{ x,z };
		}

		//bottom row
		for (int x = rightDown.x - 1, z = rightDown.z; x >= leftUp.x; x--)
		{
			if (checkTileForTerraform(checkUnplannedTerrain, heightState, map, x, z, zeroLayerLevel))
				return TileIndex{ x,z };
		}

		//left column
		for (int x = leftUp.x, z = rightDown.z - 1; z >= leftUp.z; z--)
		{
			if (checkTileForTerraform(checkUnplannedTerrain, heightState, map, x, z, zeroLayerLevel))
				return TileIndex{ x,z };
		}
	}



	return result;
}
