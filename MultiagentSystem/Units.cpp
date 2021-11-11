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

Digger::Digger(Vector3 position, Map* map, Model model, Brigadier *brigadier)
	:Unit(position, map, model)
{
	isVisible = false;
	parent = brigadier;
	state = State::IDLE;
	task = Task::GRAB;
	operationTicksLasts = operationTicksMax;
}

Digger::~Digger()
{
}

void Digger::Scan()
{
	TileIndex tile;
	if (task == Task::GRAB)
	{	
		tile = parent->getTileToTerraform(true, false);
		if (tile.x == -1) // if cant find - try again with unplanned terrain
		{
			tile = parent->getTileToTerraform(true, true);
			if (tile.x == -1) // if cant find - return to parent
			{
				task = Task::RETURN;
				setTargetPosition(parent->getPosition());
				return;
			}
		}
		//if tile has found:
		task = Task::GRAB; //previous task remains
		setTargetPosition(map->getVectorFromTileIndex(tile));
	}
	if(task == Task::PUT)
	{
		tile = parent->getTileToTerraform(false, false);
		if (tile.x == -1) // if cant find - try again with unplanned terrain
		{
			tile = parent->getTileToTerraform(false, true);
			if (tile.x == -1) // if cant find - return to parent
			{
				task = Task::RETURN;
				setTargetPosition(parent->getPosition());
				return;
			}
		}
		//if tile has found:
		task = Task::PUT; //previous task remains
		setTargetPosition(map->getVectorFromTileIndex(tile));
	}
	isVisible = true; //becomes visible if found tile
}

void Digger::Update()
{
	//in any state change y-axis (height)
	position.y = map->getActualHeight(positionTile.x, positionTile.z) * 1.1;

	switch (state)
	{
	case State::ATTACHED:
		position = parent->getPosition();
		positionTile = map->getTileIndexFromVector(position);
		Scan();
		break;
	case State::IDLE:
		Scan();
		//scan for tiles to terraform (OR IN State::ATTACHED?
		break;
	case State::MOVING:
		if (positionTile == targetPositionTile)
		{
			switch (task)
			{
			case Task::GRAB:
			case Task::PUT:
				state = State::TERRAFORMING;
				break;
			case Task::RETURN:
				state = State::ATTACHED;
				isVisible = false;
				break;
			}
		}
		else
		{
			Move();
		}
		break;
	case State::TERRAFORMING:
		if (operationTicksLasts == 0)
		{
			operationTicksLasts = operationTicksMax;
			switch (task)
			{
			case Task::GRAB:
				short height = map->getHeight(positionTile.x, positionTile.z);
				if(height )
				break;
			case Task::PUT:
				break;
			}
			break;
		}
		else
		{
			operationTicksLasts--;
		}
	}
}

void Digger::Draw()
{
	// to shift model for better rotation
	if (isVisible)
	{
		model.transform = MatrixTranslate(-25.f, 0, -25.f);
		DrawModelEx(model, position, { 0, 1.f, 0 }, direction, { 0.05f, 0.05f, 0.05f }, WHITE);
		//DrawSphere(position, 1, RED);
	}
}



Brigadier::Brigadier(Vector3 position, Map* map, Model model, Model diggerModel)
	:Unit(position, map, model)
{
	state = State::IDLE;
	siblings = new Digger * [maxNumOfDiggers];
	for (int i = 0; i < maxNumOfDiggers; i++)
		siblings[i] = new Digger(position, map, diggerModel, this);
}

Brigadier::~Brigadier()
{
	delete[] siblings;
}

void Brigadier::Update()
{
	position.y = map->getActualHeight(positionTile.x, positionTile.z) * 1.1;
	switch (state)
	{
	case State::IDLE:
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
	default:
		break;
	}
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
			if (zeroLayerLevel < map->getHeight(x, z) && map->getHeight(x, z) != 1) // tiles with 1 height cant be digged
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
