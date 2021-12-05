#include "Simulation.h"

//returns ray from camera pos to camera target
Ray getRayFromCamera(Camera camera)
{
    Ray result = { 0 };
    Vector3 buf = Vector3Subtract(camera.target, camera.position);
    float distance = Vector3Distance(camera.target, camera.position);
    result.position = camera.position;
    result.direction = Vector3{ buf.x / distance, buf.y / distance, buf.z / distance };

    return result;
}

Simulation::Simulation(int screenWidth, int screenHeight)
    :screenWidth(screenWidth), screenHeight(screenHeight)
{
    lightShader = LoadShader("base_lighting.vs", "lighting.fs");
    lightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightShader, "viewPos");

    // Ambient light level
    int ambientLoc = GetShaderLocation(lightShader, "ambient");
    const float buf[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    SetShaderValue(lightShader, ambientLoc, buf, SHADER_UNIFORM_VEC4);

    lights[0] = CreateLight(LIGHT_POINT, Vector3{ 10, 100, 10 }, Vector3Zero(), WHITE, lightShader);

    map = new Map(lightShader, 125);
    // Define our custom camera
    
    customCamera = new CustomCamera(
        { 12.0f, 12.0f, 12.0f },
        { 9.f, 0.0f, 9.f },
        { 0.0f, 1.0f, 0.0f },
        45.0f,
        CAMERA_PERSPECTIVE,
        { static_cast<float>(screenWidth), static_cast<float>(screenHeight) }
    );

    diggerModel = LoadModel("Digger.obj");
    diggerModel.materials[0].shader = lightShader;
    diggerModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(GenImageColor(10, 10, RED));

    brigadierModel = LoadModel("Brigadier.obj");
    brigadierModel.materials[0].shader = lightShader;
    brigadierModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(GenImageColor(10, 10, YELLOW));

    Vector3 cubePosition = { 0.0f, 2.0f, 0.0f };
    Vector3 cubeSize = { 1.0f, 1.0f, 1.0f };
    Mesh cubeMesh = GenMeshCube(cubeSize.x, cubeSize.x, cubeSize.x);
    Model cube = LoadModelFromMesh(cubeMesh);
    cube.materials[0].shader = lightShader;

    brigadier = new Brigadier({ 0,0,0 }, map, brigadierModel, diggerModel);
}

Simulation::~Simulation()
{
    delete map;
    delete customCamera;

    UnloadShader(lightShader);
    UnloadModel(diggerModel);
    UnloadModel(brigadierModel);
}

void Simulation::Update()
{
    customCamera->Update();
    float cameraPos[3] = { customCamera->camera.position.x, customCamera->camera.position.y, customCamera->camera.position.z };
    SetShaderValue(lightShader, lightShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    //INPUT FOR CONTROLS
    if (IsKeyPressed(KEY_Y))
    {
        lights[0].enabled = !lights[0].enabled;
        UpdateLightValues(lightShader, lights[0]);
    }

    if (IsKeyPressed(KEY_TAB))
        map->switchTerraformDraw();

    if (IsKeyPressed(KEY_F1))
        map->switchZerolayerDraw();

    if (IsKeyPressed(KEY_KP_ADD))
        toolRadius++;
    if (IsKeyPressed(KEY_KP_SUBTRACT))
    {
        toolRadius--;
        if (toolRadius < 1)
            toolRadius = 1;
    }
        

    if (IsKeyPressed(KEY_ONE))
        toolMode = true;
    if (IsKeyPressed(KEY_TWO))
        toolMode = false;

    if (IsKeyPressed(KEY_SPACE))
    {
        if (isPaused)
            isPaused = false;
        else
            isPaused = true;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Ray ray = getRayFromCamera(customCamera->camera);
        map->planTerraform(ray, toolRadius, toolMode);
    }

    //UNITS
    if (!isPaused)
    {
        brigadier->Update();
    }
    
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Ray ray = getRayFromCamera(customCamera->camera);
        RayCollision collisionMesh = map->getRayCollision(ray);
        if (collisionMesh.hit)
        {
            brigadier->setTargetPosition({ collisionMesh.point.x, 0, collisionMesh.point.z });
        }
    }
}

void Simulation::Draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(customCamera->camera);

    brigadier->Draw();

    map->Draw();

    EndMode3D();

    //minimap
    Texture minimap = map->getMinimapTexture();
    DrawTextureRec(minimap, Rectangle{ 0, 0, (float)minimap.width, (float)minimap.height }, Vector2{ 0, 0 }, WHITE);
    DrawText(TextFormat("Tiles to terraform: %d", map->getTilesToTerraform()), 0.f, (float)minimap.height + 20.f, 20, BLACK);
    DrawText(TextFormat("Tiles terraformed: %d", map->getTilesTerraformed()), 0.f, (float)minimap.height + 40.f, 20, BLACK);
    //if (map->isTerraformNeeded())
    //    DrawText("Terraform complete.", 0.f, (float)minimap.height + 70.f, 20, VIOLET);
    //else
    //    DrawText("Terraform initiated.", 0.f, (float)minimap.height + 70.f, 20, DARKGREEN);

    DrawFPS(screenWidth - 80, 30);

    EndDrawing();
}
