#include "Simulation.h"

int main(void)
{
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "terraform");

    Simulation simulation = Simulation(screenWidth, screenHeight);

    SetTargetFPS(60);                       // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())
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
    }


    CloseWindow();

    return 0;
}