#include "Units.h"

Unit::Unit(Vector3 position, Map* map, Model model)
	:position(position), map(map), model(model), speed(0.5f), targetPosition(position)
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
		tile = parent->getTileToTerraform(true, true); //seek planned tile to grab
		if (tile.x == -1) // if cant find - try again with any unplanned terrain
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
		task = Task::GRAB; //previous task remains
		setTargetPosition(map->getVectorFromTileIndex(tile));
	}
	if(task == Task::PUT)
	{
		tile = parent->getTileToTerraform(true, false); //seek planned tile to put
		if (tile.x == -1) // if cant find - try again with any unplanned terrain
		{
			tile = parent->getTileToTerraform(false, false);
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
	position.y = position.y - (position.y - map->getActualHeight(positionTile.x, positionTile.z)) * 0.3f;

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

			short height, heightPossible, zeroLayerLevel;
			bool tilePlanState;

			switch (task)
			{
			case Task::GRAB:
				short heightMustDig, heightCanDig;

				height = map->getHeight(positionTile.x, positionTile.z);
				zeroLayerLevel = map->zeroLayerLevel;
				tilePlanState = map->getTerraformPlanState(positionTile.x, positionTile.z);

				if (tilePlanState)
				{
					//if this tile is in planned territory - dig down to zerolayer or capacity
					heightPossible = std::clamp<short>(height - heighPerOperation, zeroLayerLevel, height); //limit from zerolayer to current height
				}
				else
				{
					//otherwise dig down to 1 height or capacity
					heightPossible = std::clamp<short>(height - heighPerOperation, 1, height); //limit from 1 to current height
				}
				
				heightMustDig = std::clamp<short>(height - heightPossible, 0, capacity - cargoCurrent); //how much digger actually can dig (limited by capacity)
				heightCanDig = std::clamp<short>(heightMustDig, 0, heighPerOperation); //limited by heightPerOperation
				map->setHeight(positionTile.x, positionTile.z, height - heightCanDig);
				cargoCurrent += heightCanDig;

				if (tilePlanState) 
				{
					//if tile planned - checks for reaching zerolayer
					if (height - heightCanDig == map->zeroLayerLevel)
					{
						Scan();
						return;
					}
				}
				else
				{
					//if not - checks for height 1
					if (height - heightCanDig == 1)
					{
						Scan();
						return;
					}
				}

				if (cargoCurrent == capacity) //if digger is full - find tile to fill
				{
					task = Task::PUT;
					Scan();
					return;
				}
				break;
			case Task::PUT:
				short heightCanPut, heightMustPut;

				height = map->getHeight(positionTile.x, positionTile.z);
				zeroLayerLevel = map->zeroLayerLevel;
				tilePlanState = map->getTerraformPlanState(positionTile.x, positionTile.z);

				if (tilePlanState)
				{
					//if this tile is in planned territory - put up to zerolayer or capacity
					heightPossible = std::clamp<short>(height + heighPerOperation, height, zeroLayerLevel); //limit from current height to zerolayer
				}
				else
				{
					//otherwise dig down to 1 height or capacity
					heightPossible = std::clamp<short>(height + heighPerOperation, height, 255); //limit from current height to 255
				}

				heightMustPut = std::clamp<short>(heightPossible - height, 0, cargoCurrent); //how much digger actually can put (limited by capacity)
				heightCanPut = std::clamp<short>(heightMustPut, 0, heighPerOperation); //limited by heightPerOperation
				map->setHeight(positionTile.x, positionTile.z, height + heightCanPut);
				cargoCurrent -= heightCanPut;

				if (tilePlanState)
				{
					//if tile planned - checks for reaching zerolayer
					if (height + heightCanPut == map->zeroLayerLevel)
					{
						Scan();
						return;
					}
				}
				else
				{
					//if not - checks for height 255
					if (height + heightCanPut == 255)
					{
						Scan();
						return;
					}
				}

				if (cargoCurrent == 0) //if digger is empty - find new tile to grab
				{
					task = Task::GRAB;
					Scan();
					return;
				}

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
	if (isVisible)
	{
		// to shift model for rotation
		model.transform = MatrixTranslate(-25.f, 0, -25.f);
		DrawModelEx(model, position, { 0, 1.f, 0 }, direction, { 0.05f, 0.05f, 0.05f }, WHITE);
		//DrawSphere(position, 1, RED);
	}
}



Brigadier::Brigadier(Vector3 position, Map* map, Model model, Model diggerModel)
	:Unit(position, map, model)
{
	state = State::IDLE;
	isVisible = true;
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

	//Update Diggers
	for (int i = 0; i < maxNumOfDiggers; i++)
		siblings[i]->Update();

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
	if (isVisible)
	{
		// to shift model for rotation
		model.transform = MatrixTranslate(-25.f, 0, -25.f);
		DrawModelEx(model, position, { 0, 1.f, 0 }, direction, { 0.05f, 0.05f, 0.05f }, WHITE);
		//DrawSphere(position, 1, RED);
	}

	for (int i = 0; i < maxNumOfDiggers; i++)
		siblings[i]->Draw();
}

//aux function for method below
bool checkTileForTerraform(bool checkUnplannedTerrain, bool heightState, Map *map, int x, int z, short zeroLayerLevel)
{
	//если ищется в запланированной области:
		// если ищется тайл выше зерослоя:
			// проверить, выше ли тайл чем зерослой, и выдать его
		// иначе - проверить, ниже ли тайл зерослоя, и выдать его
	
	// иначе:
		//если нужно выкопать:
			// проверять, выше ли высота чем 1, и выдать тайл
		//иначе 
		    // проверить, не равна ли высота тайла 255, и выдать тайл

	//вернуть ничего


	if (checkUnplannedTerrain)
	{
		if (checkUnplannedTerrain == map->getTerraformPlanState(x, z))
		{
			if (heightState) //if tile must be higher than zerolayer
			{
				if (map->getHeight(x, z) > zeroLayerLevel)
					return true;
			}
			else
			{
				//if tile must be lower than zerolayer
				if (map->getHeight(x, z) < zeroLayerLevel)
					return true;
			}
		}
	}
	else
	{
		if (checkUnplannedTerrain == map->getTerraformPlanState(x, z))
		{
			if (heightState) //if tile must be digged
			{
				if (map->getHeight(x, z) > 1) // tiles with 1 height cant be digged
					return true;
			}
			else
			{
				//if tile must be filled
				if (map->getHeight(x, z) < 255) // cant fill tile more than 255 height
					return true;
			}
		}
	}

	return false; // if tile not fit
}

/// <summary>
/// Finds tile to terraform, which can fit to parameters
/// </summary>
/// <param name="checkUnplannedTerrain">whether is need to check planned tiles</param>
/// <param name="heightState">if checking planned tiles - seek for above-zerolayer height or below-, otherwise checks tile to dig or to put height</param>
/// <returns></returns>
TileIndex Brigadier::getTileToTerraform(bool checkUnplannedTerrain, bool heightState)
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
