#pragma once
#include "include/raylib.h"
#include <algorithm>

class Map
{
	int length;
	int width;
	int maxHeight;
	float sizeMultiplier;

	Texture2D texture;
	//Texture2D textureTerraformPlot;
	
	Mesh mesh;
	Model model;
	Vector3 position;

	unsigned short** heightMap = nullptr;
	bool** terraformPlanMap = nullptr;
	bool terraformPlanDraw = false;
	unsigned short zeroLayerLevel = 125;

public:
	RenderTexture2D textureTerraformPlot;
	Map();
	//sets height in specified position
	void setHeight(int x, int z, short height);
	//set terraform plan to desired state
	void plotTerraform(Ray ray, int radius, bool state);
	void renderTerraformPlot();

	unsigned short getHeight(int x, int z);
	bool getTerraformPlotState(int x, int z);
	Vector2 getMapSize();

	void switchTerraformDraw();
	void Draw();
	~Map();
};

