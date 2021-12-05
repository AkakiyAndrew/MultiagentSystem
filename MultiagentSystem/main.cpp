#include "Simulation.h"

int main(void)
{
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "terraform");

    Simulation simulation = Simulation(screenWidth, screenHeight);

    SetTargetFPS(60);

    // Main loop
    while (!WindowShouldClose())
    {
        simulation.Update();
        simulation.Draw();
    }

    CloseWindow();

    return 0;
}