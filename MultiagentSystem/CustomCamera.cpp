#include "CustomCamera.h"

CustomCamera::CustomCamera(Vector3 position, Vector3 target, Vector3 up, float fovy, int projection, Vector2 screenSize)
	:screenSize(screenSize)
{
	camera = { 0 };
	camera.fovy = fovy;
	camera.position = position;
	camera.target = target;
	camera.up = up;
	camera.projection = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_THIRD_PERSON);
}

void CustomCamera::Update() 
{
	UpdateCamera(&camera);

	Vector2 mousePosition = GetMousePosition();

	if (IsKeyDown(KEY_W)) 
	{
		camera.position = Vector3Add(camera.position, Vector3Scale(Vector3Normalize(Vector3Negate(Vector3Subtract(camera.position, camera.target))), PLAYER_SPEED * GetFrameTime()));
		camera.target = Vector3Add(camera.target, Vector3Scale(Vector3Normalize(Vector3Negate(Vector3Subtract(camera.position, camera.target))), PLAYER_SPEED * GetFrameTime()));
	}
	if (IsKeyDown(KEY_S)) 
	{
		camera.position = Vector3Add(camera.position, Vector3Scale(Vector3Normalize(Vector3Subtract(camera.position, camera.target)), PLAYER_SPEED * GetFrameTime()));
		camera.target = Vector3Add(camera.target, Vector3Scale(Vector3Normalize(Vector3Subtract(camera.position, camera.target)), PLAYER_SPEED * GetFrameTime()));
	}
	if (IsKeyDown(KEY_A)) 
	{
		camera.position = Vector3Subtract(camera.position, Vector3Scale(Vector3Normalize(Vector3CrossProduct(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), camera.up)), PLAYER_SPEED * GetFrameTime()));
		camera.target = Vector3Subtract(camera.target, Vector3Scale(Vector3Normalize(Vector3CrossProduct(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), camera.up)), PLAYER_SPEED * GetFrameTime()));
	}
	if (IsKeyDown(KEY_D)) 
	{
		camera.position = Vector3Add(camera.position, Vector3Scale(Vector3Normalize(Vector3CrossProduct(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), camera.up)), PLAYER_SPEED * GetFrameTime()));
		camera.target = Vector3Add(camera.target, Vector3Scale(Vector3Normalize(Vector3CrossProduct(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), camera.up)), PLAYER_SPEED * GetFrameTime()));
	}
	if (IsKeyDown(KEY_Q))
	{
		camera.target.y -= TURN_DEG * 20.0f;
		camera.position.y -= TURN_DEG * 20.0f;
	}
	if (IsKeyDown(KEY_E))
	{
		camera.target.y += TURN_DEG * 20.0f;
		camera.position.y += TURN_DEG * 20.0f;
	}
};

//void CustomCamera::Render()
//{
//	BeginMode3D(camera);
//	DrawGrid(100, 1.0f);
//	EndMode3D();
//
//	DrawText(TextFormat("%i", GetMonitorHeight(0)), 100, 100, 50, RED);
//	DrawText(TextFormat("%i", GetMouseY()), 100, 160, 50, RED);
//	DrawText(TextFormat("%i", GetFPS()), 100, 210, 50, RED);
//};
