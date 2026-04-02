#include "fetchInput.h"

void ProcessInput(GLFWwindow* window, Camera* camera, float deltaTime) {
	float moveAmount = CAMERA_MOVE_SPEED * deltaTime;
	float rotateAmount = CAMERA_ROTATE_SPEED * deltaTime;

	// WASD — forward/backward and strafe left/right
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->MoveForward(moveAmount);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->MoveBackward(moveAmount);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->moveLeft(moveAmount);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->MoveRight(moveAmount);
	}

	// Space — move up, Left Shift — move down
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->MoveUp(moveAmount * 2);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera->MoveDown(moveAmount * 2);
	}

	// Arrow keys — pitch up/down and yaw (look) left/right
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera->Pitch(-rotateAmount * 30);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera->Pitch(rotateAmount * 30);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera->Yaw(-rotateAmount * 30);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera->Yaw(rotateAmount * 30);
	}

	// Q/E — roll left/right
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera->Roll(rotateAmount * 10);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera->Roll(-rotateAmount * 10);
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Quit on Escape press
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_J)
			enableAmbient = !enableAmbient;

		if (key == GLFW_KEY_K)
			enableDiffuse = !enableDiffuse;

		if (key == GLFW_KEY_L)
			enableSpecular = !enableSpecular;

		if (key == GLFW_KEY_U)
			enableWaterAmbient = !enableWaterAmbient;

		if (key == GLFW_KEY_I)
			enableWaterDiffuse = !enableWaterDiffuse;

		if (key == GLFW_KEY_O)
			enableWaterSpecular = !enableWaterSpecular;
	}
}