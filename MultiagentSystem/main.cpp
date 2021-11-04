#include "include/raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "include/rlights.h"
#include "Map.h"
#include "CustomCamera.h"
#include "Units.h"

//constexpr float FOVY_PERSPECTIVE = 45.0f;
//constexpr float WIDTH_ORTHOGRAPHIC = 10.0f;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "terraform");

    // Load shader and set up some uniforms
    Shader shader = LoadShader("base_lighting.vs", "lighting.fs");

    // Get some required shader loactions
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading, 
    // no need to get the location again if using that uniform name
    //shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    const float buf[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    SetShaderValue(shader, ambientLoc, buf, SHADER_UNIFORM_VEC4);

    Light lights[1] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, Vector3{ 10, 100, 10 }, Vector3Zero(), WHITE, shader);

    Map map = Map(shader);

    Model diggerModel = LoadModel("Digger.vox");
    diggerModel.materials[0].shader = shader;
    diggerModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(GenImageColor(10, 10, BLUE));
    Digger digger = Digger(Vector3{ 0.f,0.f,0.f }, &map, diggerModel);

    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };
    Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };
    Mesh cubeMesh = GenMeshCube(cubeSize.x, cubeSize.x, cubeSize.x);
    Model cube = LoadModelFromMesh(cubeMesh);
    cube.materials[0].shader = shader;

    Ray ray = { 0 };                    // Picking line ray
    RayCollision collisionBox = { 0 };
    RayCollision collisionMesh = { 0 };

    int radius = 2;
    // Define our custom camera
    Vector2 mapSize = map.getMapSize();
    CustomCamera camera = CustomCamera({ 18.0f, 18.0f, 18.0f }, { mapSize.x / 2, 0.0f, mapSize.y / 2 }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE, { screenWidth, screenHeight });

    SetTargetFPS(60);                       // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())            // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //UpdateCamera(&camera.getCamera());              // Update camera
        camera.Update();
        float cameraPos[3] = { camera.camera.position.x, camera.camera.position.y, camera.camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }

        // Update light values (actually, only enable/disable them)
        UpdateLightValues(shader, lights[0]);

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
        if (IsKeyPressed(KEY_F))
        {
            digger.setTargetPosition({ 5,5,5 });
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

                // if box isnt hitted - check collision with plane
                map.planTerraform(ray, radius, true);
            }
        }

        digger.Update();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        //if (camera.projection == CAMERA_ORTHOGRAPHIC) DrawText("ORTHOGRAPHIC", 10, 40, 20, BLACK);
        //else if (camera.projection == CAMERA_PERSPECTIVE) DrawText("PERSPECTIVE", 10, 40, 20, BLACK);

        BeginMode3D(camera.camera);
            /*DrawLine3D(Vector3{ 0.f, 0.f, 0.f }, Vector3{1.f, 0.f, 0.f}, DARKBLUE);*/
            map.Draw();
            digger.Draw();
            
            if (collisionBox.hit)
            {
                DrawModel(cube, cubePosition, 1.f, RED);
                DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);

                DrawCubeWires(cubePosition, cubeSize.x + 0.2f, cubeSize.y + 0.2f, cubeSize.z + 0.2f, GREEN);
            }
            else
            {
                DrawModel(cube, cubePosition, 1.f, GRAY);
                DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, DARKGRAY);
            }

            DrawRay(ray, MAROON);

        EndMode3D();

        //minimap
        Texture minimap = map.getMinimapTexture();
        DrawTextureRec(minimap, Rectangle { 0, 0, (float)minimap.width, (float)minimap.height }, Vector2 { 0, 0 }, WHITE);

        DrawFPS(screenWidth-80, 30);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);       // Unload shader

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}