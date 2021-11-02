#include "include/raylib.h"
#include "Map.h"
#include "CustomCamera.h"

constexpr float FOVY_PERSPECTIVE = 45.0f;
constexpr float WIDTH_ORTHOGRAPHIC = 10.0f;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - heightmap loading and drawing");

    //ToggleFullscreen();

    Map map = Map();

    // Define our custom camera to look into our 3d world
    Vector2 mapSize = map.getMapSize();
    //Camera camera = { { 18.0f, 18.0f, 18.0f }, { mapSize.x/2, 0.0f, mapSize.y/2 }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };
    
    CustomCamera camera = CustomCamera({ 18.0f, 18.0f, 18.0f }, { mapSize.x / 2, 0.0f, mapSize.y / 2 }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE, { screenWidth, screenHeight});
    //SetCameraMode(camera, CAMERA_ORBITAL);  // Set an orbital camera mode CAMERA_THIRD_PERSON
    //SetCameraMode(camera.getCamera(), CAMERA_FREE);

    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };
    Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };

    Ray ray = { 0 };                    // Picking line ray

    RayCollision collisionBox = { 0 };
    RayCollision collisionMesh = { 0 };
    int radius = 2;

    SetTargetFPS(60);                       // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())            // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //UpdateCamera(&camera.getCamera());              // Update camera
        camera.Update();

        /*if (IsKeyPressed(KEY_SPACE))
        {
            if (camera.getCamera().projection == CAMERA_PERSPECTIVE)
            {
                camera.fovy = WIDTH_ORTHOGRAPHIC;
                camera.projection = CAMERA_ORTHOGRAPHIC;
            }
            else
            {
                camera.fovy = FOVY_PERSPECTIVE;
                camera.projection = CAMERA_PERSPECTIVE;
            }
        }*/
         
        if (IsKeyPressed(KEY_Q))
        {
            map.setHeight(0, 2, 100);
        }
        if (IsKeyPressed(KEY_E))
        {
            map.setHeight(0, 0, 55);
        }

        if (IsKeyPressed(KEY_TAB))
        {
            map.switchTerraformDraw();
        }

        if (IsKeyPressed(KEY_KP_ADD))
        {
            radius++;
        }
        if (IsKeyPressed(KEY_KP_SUBTRACT))
        {
            radius--;
        }

        if (IsKeyPressed(KEY_X))
        {
            
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            ray = GetMouseRay(GetMousePosition(), camera.camera);

            // Check collision between ray and box
            collisionBox = GetRayCollisionBox(ray,
                BoundingBox{
                    Vector3{ cubePosition.x - cubeSize.x / 2, cubePosition.y - cubeSize.y / 2, cubePosition.z - cubeSize.z / 2 },
                    Vector3{ cubePosition.x + cubeSize.x / 2, cubePosition.y + cubeSize.y / 2, cubePosition.z + cubeSize.z / 2 }
                });
            
            if (!collisionBox.hit)
            {
                collisionBox.hit = false;

                // if box isnt hitted - check collision with mesh
                map.planTerraform(ray, radius, true);
                //map.renderTerraformPlot();
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        //if (camera.projection == CAMERA_ORTHOGRAPHIC) DrawText("ORTHOGRAPHIC", 10, 40, 20, BLACK);
        //else if (camera.projection == CAMERA_PERSPECTIVE) DrawText("PERSPECTIVE", 10, 40, 20, BLACK);

        BeginMode3D(camera.camera);
            /*DrawLine3D(Vector3{ 0.f, 0.f, 0.f }, Vector3{1.f, 0.f, 0.f}, DARKBLUE);*/
            map.Draw();
            if (collisionBox.hit)
            {
                DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, RED);
                DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);

                DrawCubeWires(cubePosition, cubeSize.x + 0.2f, cubeSize.y + 0.2f, cubeSize.z + 0.2f, GREEN);
            }
            else
            {
                DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, GRAY);
                DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, DARKGRAY);
            }

            DrawRay(ray, MAROON);

        EndMode3D();

        //minimap
        Texture minimap = map.getMinimapTexture();
        DrawTextureRec(minimap, Rectangle { 0, 0, (float)minimap.width, (float)-minimap.height }, Vector2 { 0, 0 }, WHITE);

        
        /*DrawTexture(texture, screenWidth - width - 20, 20, WHITE);
        DrawRectangleLines(screenWidth - width - 20, 20, width, height, GREEN);*/

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}