#pragma once
#include "include/raylib.h"
#include <algorithm>

class Map
{
	int length;
	int width;
	int maxHeight;
	float sizeMultiplier;

	RenderTexture2D textureTerraformPlan;
	Texture2D texture;
	//Texture2D textureTerraformPlot;
	
	Mesh mesh;
	Model model;
	Vector3 position;

	short** heightMap = nullptr;
	bool** terraformPlanMap = nullptr;
	bool terraformPlanDraw = false;
	short zeroLayerLevel = 125;

public:
	Map();
	//sets height in specified position
	void setHeight(int x, int z, short height);
	//set terraform plan to desired state
	void planTerraform(Ray ray, int radius, bool state);

	unsigned short getHeight(int x, int z);
	bool getTerraformPlanState(int x, int z);
	Vector2 getMapSize();
	Texture getMinimapTexture();

	void switchTerraformDraw();
	void Draw();
	~Map();
};

