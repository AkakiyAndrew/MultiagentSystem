#pragma once
#include "Map.h"
#include "Units.h"
#include "CustomCamera.h"
#include "rlight.h"

class Simulation
{
private:
	Shader lightShader;
	CustomCamera *customCamera;
	Light lights[1] = { 0 };
	int screenWidth;
	int screenHeight;

	Map *map;
	bool toolMode = true;
	int toolRadius = 3;

	Model diggerModel;
	Model brigadierModel;

	Brigadier* brigadier;

public:
	Simulation(int screenWidth, int screenHeight);
	~Simulation();

	void Update();
	void Draw();
};

