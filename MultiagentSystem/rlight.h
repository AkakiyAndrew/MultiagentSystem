#pragma once
#include "include/raylib.h"

#define         MAX_LIGHTS            4         // Max dynamic lights supported by shader

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Light data
typedef struct {
    int type;
    Vector3 position;
    Vector3 target;
    Color color;
    bool enabled;

    // Shader locations
    int enabledLoc;
    int typeLoc;
    int posLoc;
    int targetLoc;
    int colorLoc;
} Light;

// Light type
typedef enum {
    LIGHT_DIRECTIONAL,
    LIGHT_POINT
} LightType;



//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);   // Create a light and get shader locations
void UpdateLightValues(Shader shader, Light light);         // Send light properties to shader

