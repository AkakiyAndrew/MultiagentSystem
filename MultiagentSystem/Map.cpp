#include "Map.h"

Map::Map(Shader shader)
{
    Image image = LoadImage("heightmap.png");

    length = image.height;
    width = image.width;
    maxHeight = 1;
    sizeMultiplier = 1.f;

    textureTerraformPlan = LoadRenderTexture(width, length);
    BeginTextureMode(textureTerraformPlan);
        ClearBackground(RED);
    EndTextureMode();

    texture = LoadTextureFromImage(image);                
    mesh = GenMeshHeightmap(image, Vector3{ sizeMultiplier*width, maxHeight*sizeMultiplier, sizeMultiplier*length });    // Generate heightmap mesh (RAM and VRAM)
    position = Vector3{ 0.f, 0.0f, 0.f};

    model = LoadModelFromMesh(mesh);
    model.materials[0].shader = shader;
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture
    
    //memory allocation
    heightMap = new short* [length];
    for (int i = 0; i < length; i++)
    {
        heightMap[i] = new short[width];
    }
    terraformPlanMap = new bool* [length];
    for (int i = 0; i < length; i++)
    {
        terraformPlanMap[i] = new bool[width];
    }

    //heightMap
    int offset = 0;
    Color *pixels = LoadImageColors(image);
    for (int z = 0; z < length; z++)
    {
        for (int x = 0; x < width; x++)
        {
            heightMap[z][x] = (pixels[offset].r + pixels[offset].g + pixels[offset].b) / 3;
            offset += 1;
        }   
    }

    //set terraform planing default values
    for (int i = 0; i < length; i++)
        for (int j = 0; j < width; j++)
            terraformPlanMap[i][j] = false;


    //textureTerraformPlot - texture for highlighting
    //Color* colorPixels = new Color[width * length];
    //int index;
    //for (int z = 0; z < length; z++)
    //{
    //    for (int x = 0; x < width; x++)
    //    {
    //        index = z * width + x;
    //        if (terraformPlanMap[z][x] == true)
    //        {
    //            /*if (heightMap[z][x] == zeroLayerLevel)
    //                colorPixels[index] = GREEN;
    //            else*/
    //            colorPixels[index] = BLUE;
    //        }
    //        else
    //        {
    //            colorPixels[index] = RED;
    //        }
    //    }
    //}
    //Image colorImage = {
    //    colorPixels,
    //    width,
    //    length,
    //    1,
    //    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    //};
    //textureTerraformPlot.texture = LoadTextureFromImage(colorImage);
    //delete[] colorPixels;

    UnloadImage(image);
}

Map::~Map()
{
    for (int i = 0; i < length; i++)
    {
        delete[] heightMap[i];
    }
    delete[] heightMap;

    for (int i = 0; i < length; i++)
    {
        delete[] terraformPlanMap[i];
    }
    delete[] terraformPlanMap;

    UnloadTexture(texture);
    UnloadRenderTexture(textureTerraformPlan);
}

void Map::Draw()
{
    DrawModel(model, position, 1.0f, GRAY);
    DrawModelWires(model, position, 1, DARKGRAY);

    //DrawGrid(20, 1.0f);
}

void Map::setHeight(int x, int z, short height)
{
    int xMax = width - 1;
    int zMax = length - 1;

    float actualHeight = height * (maxHeight * sizeMultiplier / 255.f);

    if (x >= 0 && x <= xMax && z >= 0 && z <= zMax)
    {
        heightMap[z][x] = height;

        BeginTextureMode(textureTerraformPlan);
        if (terraformPlanMap[z][x] == true)
        {
            if (height == zeroLayerLevel)
                DrawPixel(x, length - z - 1, GREEN);
            else
                DrawPixel(x, length - z - 1, BLUE);
        }
        else
            DrawPixel(x, length - z, RED);
        EndTextureMode();

        int vCounter = 0;

        //left-up
        vCounter = ((z - 1) * xMax + x - 1) * 18;
        if (vCounter >= 0 && x > 0 && z > 0)
        {
            mesh.vertices[vCounter + 16] = actualHeight;
        }

        //right-up
        vCounter = ((z - 1) * xMax + x) * 18;
        if (vCounter >= 0 && x < xMax && z > 0)
        {
            mesh.vertices[vCounter + 4] = actualHeight;
            mesh.vertices[vCounter + 13] = actualHeight;
        }

        //left-down
        vCounter = (z * xMax + x - 1) * 18;
        if (vCounter >= 0 && x > 0 && z < zMax)
        {
            mesh.vertices[vCounter + 7] = actualHeight;
            mesh.vertices[vCounter + 10] = actualHeight;
        }

        //right-down
        vCounter = (z * xMax + x) * 18;
        if (vCounter >= 0 && x < xMax && z < zMax )
        {
            mesh.vertices[vCounter + 1] = actualHeight;
        }

        UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float) * mesh.vertexCount * 3, 0);
    }
    else
    {
        throw "Map mesh coordinats out of limit.";
    }
}

void Map::switchTerraformDraw()
{
    if (terraformPlanDraw)
    {
        terraformPlanDraw = false;
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    }
    else
    {
        terraformPlanDraw = true;
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureTerraformPlan.texture;
    }
}

bool Map::getTerraformPlanState(int x, int z)
{
    return terraformPlanMap[z][x];
}

short Map::getHeight(int x, int z)
{
    return heightMap[z][x];
}

float Map::getActualHeight(int x, int z)
{
    return heightMap[z][x] * (maxHeight * sizeMultiplier / 255.f);
}

Vector2 Map::getMapSize()
{
    return Vector2{ static_cast<float>(width), static_cast<float>(length) };
}

Vector2 Map::getActualMapSize()
{
    return Vector2{ width * sizeMultiplier, length * sizeMultiplier };
}

Texture Map::getMinimapTexture()
{
    if (terraformPlanDraw)
        return textureTerraformPlan.texture;
    else
        return texture;
}

TileIndex Map::getTileIndexFromVector(Vector3 position)
{
    TileIndex result = {
        static_cast<int>(position.x / sizeMultiplier),
        static_cast<int>(position.z / sizeMultiplier)
    };
    return result;
}

RayCollision Map::getRayCollision(Ray ray)
{
    return GetRayCollisionQuad(ray,
        position, //left-up
        { position.x, position.y, length * sizeMultiplier }, //left-down
        { width * sizeMultiplier, position.y, length * sizeMultiplier }, //right-down
        { width * sizeMultiplier, position.y, position.z } //right-up 
    );
}

void Map::planTerraform(Ray ray, int radius, bool state)
{
    RayCollision collisionMesh = getRayCollision(ray);
    
    if (collisionMesh.hit)
    {
        int centerX = (collisionMesh.point.x * width) / (sizeMultiplier * width - position.x);
        int centerZ = (collisionMesh.point.z * length) / (sizeMultiplier * length - position.z);

        Vector2 upLeftCorner = { std::clamp<float>(centerX - radius, 0, centerX), std::clamp<float>(centerZ - radius, 0, centerZ) };
        Vector2 downRightCorner = { std::clamp<float>(centerX + radius, centerX, width - 1), std::clamp<float>(centerZ + radius, centerZ, this->length - 1) };
        //reducing up limit `cause of max index of maps (not the actual indices)

        BeginTextureMode(textureTerraformPlan);

        for (int z = upLeftCorner.y; z <= downRightCorner.y; z++)
        {
            for (int x = upLeftCorner.x; x <= downRightCorner.x; x++)
            {
                if (CheckCollisionPointCircle(Vector2{ static_cast<float>(x), static_cast<float>(z) }, Vector2{ static_cast<float>(centerX), static_cast<float>(centerZ) }, radius))
                {
                    terraformPlanMap[z][x] = state;
                    if (state)
                    {
                        if (heightMap[z][x] == zeroLayerLevel)
                            DrawPixel(x, length - z - 1, GREEN);
                        else
                            DrawPixel(x, length - z - 1, BLUE);
                    }
                    else
                        DrawPixel(x, length - z, RED);
                    
                }
            }
        }

        EndTextureMode();
    }
}