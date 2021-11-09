#include "Simulation.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1000;
    const int screenHeight = 500;

    InitWindow(screenWidth, screenHeight, "terraform");

    Simulation simulation = Simulation(screenWidth, screenHeight);

    SetTargetFPS(60);                       // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())            // Detect window close button or ESC key
    {
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

        simulation.Update();
        simulation.Draw();

        
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}