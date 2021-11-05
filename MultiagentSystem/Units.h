#pragma once
#include "Map.h"
#include "include/raylib.h"
#include "Enums.h"

class Unit
{
protected:
	//move
	Vector3 position;
	TileIndex positionTile;
	Vector3 targetPosition;
	TileIndex targetPositionTile;
	Vector2 directionalVector;
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
	void setTargetPosition(Vector3 newTarget);

	void Move();
	virtual void Update()=0;
	virtual void Draw() = 0;
};

class Digger: public Unit
{
private:
	int capacityMax;
	int capacityCurrent;
	
public:
	Digger(Vector3 position, Map* map, Model model);
	~Digger();

	void Update();
	void Draw();

};