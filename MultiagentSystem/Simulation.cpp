#include "Simulation.h"

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
    Vector2 mapSize = map->getMapSize();
    customCamera = new CustomCamera(
        { 18.0f, 18.0f, 18.0f },
        { 9.f, 0.0f, 9.f },
        { 0.0f, 1.0f, 0.0f },
        45.0f,
        CAMERA_PERSPECTIVE,
        { static_cast<float>(screenWidth), static_cast<float>(screenHeight) }
    );

    diggerModel = LoadModel("Digger.obj");
    diggerModel.materials[0].shader = lightShader;
    diggerModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(GenImageColor(10, 10, BLUE));
    //Digger digger = Digger(Vector3{ 0.f,0.f,0.f }, &map, diggerModel);

    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };
    Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };
    Mesh cubeMesh = GenMeshCube(cubeSize.x, cubeSize.x, cubeSize.x);
    Model cube = LoadModelFromMesh(cubeMesh);
    cube.materials[0].shader = lightShader;

    Ray ray = { 0 };                    // Picking line ray
    RayCollision collisionBox = { 0 };
    RayCollision collisionMesh = { 0 };
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
    // Update
    customCamera->Update();
    float cameraPos[3] = { customCamera->camera.position.x, customCamera->camera.position.y, customCamera->camera.position.z };
    SetShaderValue(lightShader, lightShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    // Check key inputs to enable/disable lights
    if (IsKeyPressed(KEY_Y))
    {
        lights[0].enabled = !lights[0].enabled;
        UpdateLightValues(lightShader, lights[0]);
    }

    if (IsKeyPressed(KEY_TAB))
    {
        map->switchTerraformDraw();
    }

    if (IsKeyPressed(KEY_KP_ADD))
    {
        toolRadius++;
    }
    if (IsKeyPressed(KEY_KP_SUBTRACT))
    {
        toolRadius--;
    }
    //if (IsKeyPressed(KEY_F))
    //{
    //    digger.setTargetPosition({ 5,5,5 });
    //}
    //if (IsKeyPressed(KEY_G))
    //{
    //    digger.setTargetPosition({ 0,0,0 });
    //}

    //if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    //{
    //    //ray = GetMouseRay(GetMousePosition(), customCamera->camera);
    //    Ray ray = getRayFromCamera(customCamera->camera);
    //    // получить луч из позиции камеры и её цели
    //    // Check collision between ray and box
    //    collisionBox = GetRayCollisionBox(ray,
    //        BoundingBox{
    //            Vector3{ cubePosition.x - cubeSize.x / 2, cubePosition.y - cubeSize.y / 2, cubePosition.z - cubeSize.z / 2 },
    //            Vector3{ cubePosition.x + cubeSize.x / 2, cubePosition.y + cubeSize.y / 2, cubePosition.z + cubeSize.z / 2 }
    //        });

    //    if (!collisionBox.hit)
    //    {
    //        collisionBox.hit = false;

    //        // if box isnt hitted - check collision with plane
    //        map->planTerraform(ray, toolRadius, toolMode);
    //    }
    //}

    //if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    //{
    //    //ray = GetMouseRay(GetMousePosition(), customCamera->camera);
    //    Ray ray = getRayFromCamera(customCamera->camera);

    //    RayCollision collisionMesh = map->getRayCollision(ray);

    //    if (collisionMesh.hit)
    //    {
    //        digger.setTargetPosition({ collisionMesh.point.x, 0, collisionMesh.point.z });
    //    }
    //}
}

void Simulation::Draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    //if (camera.projection == CAMERA_ORTHOGRAPHIC) DrawText("ORTHOGRAPHIC", 10, 40, 20, BLACK);
    //else if (camera.projection == CAMERA_PERSPECTIVE) DrawText("PERSPECTIVE", 10, 40, 20, BLACK);

    BeginMode3D(customCamera->camera);
    /*DrawLine3D(Vector3{ 0.f, 0.f, 0.f }, Vector3{1.f, 0.f, 0.f}, DARKBLUE);*/
    map->Draw();

    //digger.Draw();
    //if (collisionBox.hit)
    //{
    //    DrawModel(cube, cubePosition, 1.f, RED);
    //    DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);

    //    DrawCubeWires(cubePosition, cubeSize.x + 0.2f, cubeSize.y + 0.2f, cubeSize.z + 0.2f, GREEN);
    //}
    //else
    //{
    //    DrawModel(cube, cubePosition, 1.f, GRAY);
    //    DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, DARKGRAY);
    //}
    //DrawRay(ray, MAROON);

    EndMode3D();

    //minimap
    Texture minimap = map->getMinimapTexture();
    DrawTextureRec(minimap, Rectangle{ 0, 0, (float)minimap.width, (float)minimap.height }, Vector2{ 0, 0 }, WHITE);

    DrawFPS(screenWidth - 80, 30);

    EndDrawing();
}
