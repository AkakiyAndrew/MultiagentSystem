#pragma once
#include "include/raylib.h"


class Map
{
	Texture2D texture;
	Mesh mesh;
	Model model;
	Vector3 position;
	unsigned short** heightMap = nullptr;
	bool** terraformPlanMap = nullptr;

public:
	Map();
	//sets height in specified position
	void setMeshPixelHeight(int x, int z, int height);
	void Update();
	void Draw();
	~Map();
};

