#pragma once

enum class State
{
	IDLE,
	MOVING,
	TERRAFORMING
};

struct TileIndex
{
	int x = 0;
	int z = 0;

	TileIndex()
		:x(0), z(0)
	{
	}

	TileIndex(int x, int z)
		:x(x), z(z)
	{
	}

	bool operator==(TileIndex right)
	{
		return x == right.x && z == right.z;
	}

	bool operator!=(TileIndex right)
	{
		return x != right.x || z != right.z;
	}
};

inline int Vector2Angle(Vector3 v1, Vector3 v2)
{
	int result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / PI);
	if (result < 0) result += 360.0f;
	return result;
}