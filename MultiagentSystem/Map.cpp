#include "Map.h"

#define GRAY_VALUE(c) ((c.r+c.g+c.b)/3)

Map::Map()
{
    Image image = LoadImage("heightmap.png");

    height = image.height;
    width = image.width;
    texture = LoadTextureFromImage(image);                // Convert image to texture (VRAM)
    float sizeMultiplier = 1.f;
    mesh = GenMeshHeightmap(image, Vector3{ sizeMultiplier, 1, sizeMultiplier });    // Generate heightmap mesh (RAM and VRAM)
    position = Vector3{ 0.f, 0.0f, 0.f};

    model = LoadModelFromMesh(mesh);                          // Load model from generated mesh
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;         // Set map diffuse texture
    
    //memory allocation
    heightMap = new unsigned short* [height];
    for (int i = 0; i < height; i++)
    {
        heightMap[i] = new unsigned short[width];
    }

    terraformPlanMap = new bool* [height];
    for (int i = 0; i < height; i++)
    {
        terraformPlanMap[i] = new bool[width];
    }

    //heightMap
    int offset = 0;
    unsigned char* pixels = (unsigned char*)image.data;
    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            heightMap[z][x] = (pixels[offset] + pixels[offset + 1] + pixels[offset + 2]) / 3;
            offset += 4;
        }   
    }
    //set terraform planing default values
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            terraformPlanMap[i][j] = false;

    renderTerraformPlot();

    UnloadImage(image);
}

Map::~Map()
{
    for (int i = 0; i < height; i++)
    {
        delete[] heightMap[i];
    }
    delete[] heightMap;

    for (int i = 0; i < height; i++)
    {
        delete[] terraformPlanMap[i];
    }
    delete[] terraformPlanMap;

    UnloadTexture(texture);
}

void Map::Draw()
{
    DrawModel(model, position, 1.0f, GRAY);
    DrawModelWires(model, position, 1, DARKGRAY);

    DrawGrid(20, 1.0f);

    //terraform cubes planning highlighting
    /*Vector3 cubePosition;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cubePosition = { position.x + j*3.f, heightMap[i][j]/255.f + 1.f, position.z + i*3.f };
            if (terraformPlanMap[i][j])
                DrawCube(cubePosition, 1, 1, 1, GREEN);
            else
                DrawCube(cubePosition, 1, 1, 1, RED);
        }
    } */
}

void Map::setHeight(int x, int z, short height)
{
    int xMax = width - 1;
    int zMax = height - 1;
    if (x >= 0 && x <= xMax && z >= 0 && z <= zMax)
    {
        heightMap[z][x] = height;

        int vCounter = 0;

        //left-up
        vCounter = ((z - 1) * xMax + x - 1) * 18;
        if (vCounter >= 0 && x > 0 && z > 0)
        {
            mesh.vertices[vCounter + 16] = static_cast<float>(height);
        }

        //right-up
        vCounter = ((z - 1) * xMax + x) * 18;
        if (vCounter >= 0 && x < xMax && z > 0)
        {
            mesh.vertices[vCounter + 4] = static_cast<float>(height);
            mesh.vertices[vCounter + 13] = static_cast<float>(height);
        }

        //left-down
        vCounter = (z * xMax + x - 1) * 18;
        if (vCounter >= 0 && x > 0 && z < zMax)
        {
            mesh.vertices[vCounter + 7] = static_cast<float>(height);
            mesh.vertices[vCounter + 10] = static_cast<float>(height);
        }

        //right-down
        vCounter = (z * xMax + x) * 18;
        if (vCounter >= 0 && x < xMax && z < zMax )
        {
            mesh.vertices[vCounter + 1] = static_cast<float>(height);
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
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureTerraformPlot;
    }
}

bool Map::getTerraformPlotState(int x, int z)
{
    return terraformPlanMap[z][x];
}

unsigned short Map::getHeight(int x, int z)
{
    return heightMap[z][x];
}

void Map::plotTerraform(int centerX, int centerZ, int radius, bool state)
{
    Vector2 upLeftCorner = { std::clamp<float>(centerX - radius, 0, centerX), std::clamp<float>(centerZ - radius, 0, centerZ) };
    Vector2 downRightCorner = { std::clamp<float>(centerX + radius, centerX, height - 1), std::clamp<float>(centerZ + radius, centerZ, this->height - 1) };
    //reducing up limit `cause of max index of maps (not the actual indices)

    for (int z = upLeftCorner.y; z <= downRightCorner.y; z++)
    {
        for (int x = upLeftCorner.x; x <= downRightCorner.x; x++)
        {
            if (CheckCollisionPointCircle(Vector2{ static_cast<float>(x), static_cast<float>(z) }, Vector2{ static_cast<float>(centerX), static_cast<float>(centerZ) }, radius))
                terraformPlanMap[z][x] = state;
            //TODO: optimize this crap (and make parallel computaton?)
        }
    }
}

void Map::renderTerraformPlot()
{
    Color* colorPixels = new Color[width * height];
    
    int index;
    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            index = z * width + x;
            if (terraformPlanMap[z][x] == true)
            {
                if(heightMap[z][x] == zeroLayerLevel)
                    colorPixels[index] = GREEN;
                else
                    colorPixels[index] = BLUE;
            }
            else
            {
                colorPixels[index] = RED;
            }
        }
    }

    Image colorImage = {
        colorPixels,
        width,
        height,
        1,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    UnloadTexture(textureTerraformPlot);
    textureTerraformPlot = LoadTextureFromImage(colorImage);
    delete[] colorPixels;
}