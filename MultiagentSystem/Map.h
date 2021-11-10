#pragma once
#include "Enums.h"

class Map
{
	int length;
	int width;
	
	RenderTexture2D textureTerraformPlan;
	Texture2D texture;
	Mesh mesh;
	Model model;
	Vector3 position;
	int maxHeight; //only visual - dont affect heightMap
	float sizeMultiplier; //how quads are big 

	short** heightMap = nullptr;
	bool** terraformPlanMap = nullptr;
	bool terraformPlanDraw = false;
	short zeroLayerLevel;

public:
	Map(Shader shader, short zeroLayerLevel);
	//sets height in specified position
	void setHeight(int x, int z, short height);
	//set terraform plan to desired state
	void planTerraform(Ray ray, int radius, bool state);

	short getHeight(int x, int z);
	float getActualHeight(int x, int z);
	bool getTerraformPlanState(int x, int z);
	TileIndex getMapSize();
	Vector2 getActualMapSize();
	Texture getMinimapTexture();
	TileIndex getTileIndexFromVector(Vector3 position);
	RayCollision getRayCollision(Ray ray);

	void switchTerraformDraw();
	void Draw();
	~Map();
};

