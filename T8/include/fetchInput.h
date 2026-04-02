#ifndef FETCH_INPUT_H_
#define FETCH_INPUT_H_

#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/glfw3.h"
#include "camera.h"

// Camera control speeds
const float CAMERA_MOVE_SPEED = 15.0f;   // units per second
const float CAMERA_ROTATE_SPEED = 60.0f; // degrees per second

// Lighting toggles
extern bool enableAmbient;
extern bool enableDiffuse;
extern bool enableSpecular;

extern bool enableWaterAmbient;
extern bool enableWaterDiffuse;
extern bool enableWaterSpecular;

// Polls held keys and applies camera movement/rotation for this frame.
// Call once per frame, passing the delta time in seconds.
void ProcessInput(GLFWwindow* window, Camera* camera, float deltaTime);

// GLFW key callback for one-shot key actions (e.g. quit).
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // FETCH_INPUT_H_#pragma once
