#pragma once
#include "Map.h"

class Unit
{
protected:
	//move
	Vector3 position;
	TileIndex positionTile;
	Vector3 targetPosition;
	TileIndex targetPositionTile;
	Vector2 directionalVector = {0.f, 1.f};
	float speed;
	float direction;

	//visual
	Model model;
	bool isVisible = true;
	
	//system
	int ID; //dont need?
	State state;
	Map* map = nullptr;

public:
	Unit(Vector3 position, Map* map, Model model);
	 ~Unit();

	Vector3 getPosition();
	TileIndex getTargetPositionTile();
	void setTargetPosition(Vector3 newTarget);

	void Move();
	virtual void Update()=0;
	virtual void Draw() = 0;
};

class Brigadier;

class Digger: public Unit
{
private:
	const int capacity = 150;
	int cargoCurrent = 0;
	const int heighPerOperation = 30;
	const int operationTicksMax = 5;
	int operationTicksLasts;

	Brigadier* parent = nullptr;
	Task task;
	void Scan();

public:
	Digger(Vector3 position, Map* map, Model model, Brigadier* parent);
	~Digger();
	void Update();
	void Draw();
};

class Brigadier : public Unit
{
	const int maxNumOfDiggers = 10;

	Digger **siblings = nullptr;

	bool checkTileAssignments(TileIndex tile);
	bool checkTileForTerraform(bool checkUnplannedTerrain, bool heightState, Map* map, int x, int z, short zeroLayerLevel);
public:
	Brigadier(Vector3 position, Map* map, Model model, Model diggerModel);
	~Brigadier();
	void Update();
	void Draw();

	TileIndex getTileToTerraform(bool checkUnplannedTerrain, bool heightState);

};