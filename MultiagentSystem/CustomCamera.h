#pragma once

#include "include/raylib.h"
#include "include/raymath.h"
#include <math.h>

#define TURN_DEG 1 * DEG2RAD
#define PLAYER_SPEED 10.0f

class CustomCamera 
{
private:
	Vector2 screenSize;

public:
	Camera camera;
	CustomCamera(Vector3 position, Vector3 target, Vector3 up, float fovy, int projection, Vector2 screenSize);
	void Update();
};
