#include "include/raylib.h"
#include <random>
#include "Map.h"

constexpr float FOVY_PERSPECTIVE = 45.0f;
constexpr float WIDTH_ORTHOGRAPHIC = 10.0f;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - heightmap loading and drawing");

    // Define our custom camera to look into our 3d world
    Camera camera = { { 18.0f, 18.0f, 18.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
    SetCameraMode(camera, CAMERA_THIRD_PERSON);  // Set an orbital camera mode CAMERA_THIRD_PERSON

    Map map = Map();

    SetTargetFPS(60);                       // Set our game to run at 60 frames-per-second
    

    // Main game loop
    while (!WindowShouldClose())            // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera);              // Update camera

        if (IsKeyPressed(KEY_SPACE))
        {
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
                camera.fovy = WIDTH_ORTHOGRAPHIC;
                camera.projection = CAMERA_ORTHOGRAPHIC;
            }
            else
            {
                camera.fovy = FOVY_PERSPECTIVE;
                camera.projection = CAMERA_PERSPECTIVE;
            }
        }
         
        if (IsKeyPressed(KEY_Q))
        {
            map.setMeshPixelHeight(0, 2, 10);
        }
        if (IsKeyDown(KEY_E))
        {
            map.setMeshPixelHeight(0, 0, 5);
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (camera.projection == CAMERA_ORTHOGRAPHIC) DrawText("ORTHOGRAPHIC", 10, 40, 20, BLACK);
        else if (camera.projection == CAMERA_PERSPECTIVE) DrawText("PERSPECTIVE", 10, 40, 20, BLACK);

        BeginMode3D(camera);

        map.Draw();

        EndMode3D();

        //minimap
        /*DrawTexture(texture, screenWidth - texture.width - 20, 20, WHITE);
        DrawRectangleLines(screenWidth - texture.width - 20, 20, texture.width, texture.height, GREEN);*/

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