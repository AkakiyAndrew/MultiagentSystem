#include "Map.h"

#define GRAY_VALUE(c) ((c.r+c.g+c.b)/3)

Map::Map()
{
    Image image = LoadImage("heightmap_0.png");
    float sizeMultiplier = 16.f;
    texture = LoadTextureFromImage(image);                // Convert image to texture (VRAM)
    mesh = GenMeshHeightmap(image, Vector3{ sizeMultiplier, 1, sizeMultiplier });    // Generate heightmap mesh (RAM and VRAM)
    position = Vector3{ 0.f, 0.0f, 0.f};

    model = LoadModelFromMesh(mesh);                          // Load model from generated mesh
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;         // Set map diffuse texture
    
    //memory allocation
    heightMap = new unsigned short* [texture.height];
    for (int i = 0; i < texture.height; i++)
    {
        heightMap[i] = new unsigned short[texture.width];
    }

    terraformPlanMap = new bool* [texture.height];
    for (int i = 0; i < texture.height; i++)
    {
        terraformPlanMap[i] = new bool[texture.width];
    }

    //heightMap
    int offset = 0;
    unsigned char* pixels = (unsigned char*)image.data;
    for (int z = 0; z < texture.height; z++)
    {
        for (int x = 0; x < texture.width; x++)
        {
            heightMap[z][x] = (pixels[offset] + pixels[offset + 1] + pixels[offset + 2]) / 3;
            offset += 4;
        }   
    }
    //set terraform planing default values
    for (int i = 0; i < texture.height; i++)
        for (int j = 0; j < texture.width; j++)
            terraformPlanMap[i][j] = false;

    UnloadImage(image);
}

Map::~Map()
{
    for (int i = 0; i < texture.height; i++)
    {
        delete[] heightMap[i];
    }
    delete[] heightMap;

    for (int i = 0; i < texture.height; i++)
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
    Vector3 cubePosition;
    for (int i = 0; i < texture.height; i++)
    {
        for (int j = 0; j < texture.width; j++)
        {
            cubePosition = { position.x + j*3.f, heightMap[i][j]/255.f + 1.f, position.z + i*3.f };
            if (terraformPlanMap[i][j])
                DrawCube(cubePosition, 1, 1, 1, RED);
            else
                DrawCube(cubePosition, 1, 1, 1, GREEN);
        }
    } 
}

void Map::setMeshPixelHeight(int x, int z, int height)
{
    int xMax = texture.width - 1;
    int zMax = texture.height - 1;
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