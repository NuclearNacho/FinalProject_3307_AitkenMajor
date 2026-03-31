#include <iostream>
#include <stdexcept>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>
#include "fetchInput.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

// Path to shader directory
#define DIRECTORY "C:/Users/ethan/OneDrive/University Work/Computer Science/3D Computer Graphics/Project/FinalProject_3307_AitkenMajor/T8/"

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Window settings
const std::string window_title_g = "FinalProject_3307_AitkenMajor";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 background(0.0, 0.0, 0.0);

// Camera clip planes + FOV
float camera_near_clip_distance_g = 0.01f;
float camera_far_clip_distance_g = 1000.0f;
float camera_fov_g = 60.0f;

// Global matrices
glm::mat4 view_matrix, projection_matrix;

GLFWwindow* window;
Camera* camera;


//-----------------------------------------------
// Utility: Load text file (used for shaders)
//-----------------------------------------------
std::string LoadTextFile(std::string filename) {
	std::ifstream f(filename);
	if (f.fail()) {
		throw(std::ios_base::failure(std::string("Error opening file ") + filename));
	}
	std::string content, line;
	while (std::getline(f, line)) content += line + "\n";
	f.close();
	return content;
}


//-----------------------------------------------
// Compile + link shaders
//-----------------------------------------------
GLuint LoadShaders(std::string shaderName) {

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::string fragment_shader = LoadTextFile(std::string(DIRECTORY) + "resource\\" + shaderName + ".frag");
	std::string vertex_shader = LoadTextFile(std::string(DIRECTORY) + "resource\\" + shaderName + ".vert");

	const char* vsrc = vertex_shader.c_str();
	glShaderSource(vs, 1, &vsrc, NULL);
	glCompileShader(vs);

	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vs, 512, NULL, buffer);
		throw std::ios_base::failure(std::string("Vertex compile error: ") + buffer);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fsrc = fragment_shader.c_str();
	glShaderSource(fs, 1, &fsrc, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fs, 512, NULL, buffer);
		throw std::ios_base::failure(std::string("Fragment compile error: ") + buffer);
	}

	GLuint shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);

	// Bind attribute locations BEFORE linking so they match VAO layout
	glBindAttribLocation(shader, 0, "vertex");
	glBindAttribLocation(shader, 1, "color");
	glBindAttribLocation(shader, 2, "normal");

	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetProgramInfoLog(shader, 512, NULL, buffer);
		throw std::ios_base::failure(std::string("Link error: ") + buffer);
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return shader;
}


//-----------------------------------------------------------
// Create terrain mesh with positions, colors AND normals
//-----------------------------------------------------------
GLuint CreateBumpyPlaneVAO(
	float width, float height,
	int rows, int cols,
	const std::vector<float>& heights,
	GLuint& outIndexCount)
{
	if ((int)heights.size() != rows * cols)
		throw std::runtime_error("Heightmap size mismatch");

	//-------------------------------------------------------
	// Compute vertex normals from heightmap
	// Central-difference method gives smooth shading
	//-------------------------------------------------------
	std::vector<glm::vec3> normals(rows * cols);

	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {

			float hL = (c > 0) ? heights[r * cols + c - 1] : heights[r * cols + c];
			float hR = (c < cols - 1) ? heights[r * cols + c + 1] : heights[r * cols + c];
			float hD = (r > 0) ? heights[(r - 1) * cols + c] : heights[r * cols + c];
			float hU = (r < rows - 1) ? heights[(r + 1) * cols + c] : heights[r * cols + c];

			glm::vec3 dx = glm::vec3(2.0f, 0.0f, hR - hL);
			glm::vec3 dy = glm::vec3(0.0f, 2.0f, hU - hD);

			normals[r * cols + c] = glm::normalize(glm::cross(dx, dy));
		}
	}

	//-------------------------------------------------------
	// Build vertex buffer: position + color + normal
	//-------------------------------------------------------
	std::vector<GLfloat> vertexData;
	vertexData.reserve(rows * cols * 9);

	float halfW = width * 0.5f;
	float halfH = height * 0.5f;

	float hmin = heights[0], hmax = heights[0];
	for (float h : heights) {
		hmin = std::min(hmin, h);
		hmax = std::max(hmax, h);
	}
	float range = (hmax - hmin == 0) ? 1.0f : (hmax - hmin);

	for (int r = 0; r < rows; ++r) {
		float v = (float)r / (rows - 1);
		float y = v * height - halfH;

		for (int c = 0; c < cols; ++c) {
			float u = (float)c / (cols - 1);
			float x = u * width - halfW;
			float z = heights[r * cols + c];

			// position
			vertexData.push_back(x);
			vertexData.push_back(y);
			vertexData.push_back(z);

			// color based on height
			float t = (z - hmin) / range;
			glm::vec3 col = glm::mix(glm::vec3(0, 0.3, 0.7), glm::vec3(0.2, 1.0, 0.2), t);
			vertexData.push_back(col.r);
			vertexData.push_back(col.g);
			vertexData.push_back(col.b);

			// normal
			glm::vec3 n = normals[r * cols + c];
			vertexData.push_back(n.x);
			vertexData.push_back(n.y);
			vertexData.push_back(n.z);
		}
	}

	//-------------------------------------------------------
	// Generate triangle indices
	//-------------------------------------------------------
	std::vector<GLuint> indices;
	for (int r = 0; r < rows - 1; ++r) {
		for (int c = 0; c < cols - 1; ++c) {
			GLuint i0 = r * cols + c;
			GLuint i1 = i0 + 1;
			GLuint i2 = (r + 1) * cols + c;
			GLuint i3 = i2 + 1;

			indices.insert(indices.end(), { i0,i2,i3, i0,i3,i1 });
		}
	}

	outIndexCount = (GLuint)indices.size();

	//-------------------------------------------------------
	// Upload to GPU
	//-------------------------------------------------------
	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(GLfloat),
		vertexData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(GLuint),
		indices.data(), GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);

	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glBindVertexArray(0);
	return vao;
}


//-----------------------------------------------------------
// Create a flat water plane at a constant z height
// Uses same vertex format: position + color + normal
//-----------------------------------------------------------
GLuint CreateWaterPlaneVAO(float width, float height, float zHeight, GLuint& outIndexCount)
{
	float halfW = width * 0.5f;
	float halfH = height * 0.5f;

	// flat quad covering the terrain
	GLfloat vertexData[] = {
		// position                  // color              // normal
		-halfW, -halfH, zHeight,     0.0f, 0.35f, 0.9f,    0.0f, 0.0f, 1.0f,
		 halfW, -halfH, zHeight,     0.0f, 0.35f, 0.9f,    0.0f, 0.0f, 1.0f,
		 halfW,  halfH, zHeight,     0.0f, 0.35f, 0.9f,    0.0f, 0.0f, 1.0f,
		-halfW,  halfH, zHeight,     0.0f, 0.35f, 0.9f,    0.0f, 0.0f, 1.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	outIndexCount = 6;

	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);

	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glBindVertexArray(0);
	return vao;
}


// Window resize callback
void ResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	float top = tan((camera_fov_g / 2.0f) * (glm::pi<float>() / 180.0f)) * camera_near_clip_distance_g;
	float right = top * width / height;
	projection_matrix = glm::frustum(-right, right, -top, top, camera_near_clip_distance_g, camera_far_clip_distance_g);
}

// Print OpenGL info
void PrintOpenGLInfo() {
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

//-----------------------------------------------------------
// Generate deterministic terrain heightmap
// Uses layered sine/cosine functions with radial inversion
// to create a central basin surrounded by hilly terrain
//-----------------------------------------------------------
void GenerateTerrainHeights(
	int rows, int cols,
	std::vector<float>& heights)
{
	heights.resize(rows * cols);

	// Deterministic height generation parameters
	const float maxPeak = 14.0f;    // deeper trench
	const float falloff = 2.0f;     // slower falloff = hills extend further out
	const float ridgeAmp = 7.0f;    // much more pronounced ridges
	const float freqX = 5.0f;       // higher frequency = more bumps
	const float freqY = 11.0f;

	// Radial inversion setup
	const float maxRadius = std::sqrt(2.0f);

	for (int r = 0; r < rows; ++r) {
		float v = (rows == 1) ? 0.5f : (float)r / (float)(rows - 1);
		float yNorm = v * 2.0f - 1.0f;

		for (int c = 0; c < cols; ++c) {
			float u = (cols == 1) ? 0.5f : (float)c / (float)(cols - 1);
			float xNorm = u * 2.0f - 1.0f;

			// original deterministic height
			float radial = std::exp(-((xNorm * xNorm + yNorm * yNorm) * falloff));
			float base = maxPeak * radial;
			float ridges = ridgeAmp * std::sin(xNorm * freqX) * std::cos(yNorm * freqY) * (0.4f + 0.6f * radial);
			float waves = 1.5f * (std::sin(xNorm * 3.0f) + std::cos(yNorm * 2.5f)) * (1.0f - 0.6f * radial);

			// Extra bumpy layers for more severe terrain variation
			float bumps1 = 1.6f * std::sin(xNorm * 15.0f + yNorm * 7.0f) * (0.3f + 0.7f * radial);
			// float bumps2 = 0.0f * std::cos(xNorm * 5.0f - yNorm * 13.0f) * (0.5f + 0.5f * radial);
			// float bumps3 = 0.0f * std::sin((xNorm + yNorm) * 20.0f);  // high-freq detail everywhere

			float h_orig = base + ridges + waves + bumps1; // + bumps2 + bumps3;

			// normalized radial distance from center
			float rDist = std::sqrt(xNorm * xNorm + yNorm * yNorm);
			float rNorm = rDist / maxRadius;
			if (rNorm < 0.0f) rNorm = 0.0f;
			if (rNorm > 1.0f) rNorm = 1.0f;

			// invert in center, keep edge mostly unchanged
			float factor = 2.0f * rNorm - 1.0f;
			float h = h_orig * factor;

			heights[r * cols + c] = h;
		}
	}
}

//-----------------------------------------------------------
// Initialize GLFW window, GLEW, and OpenGL state
// Creates the application window and sets up the GL context
//-----------------------------------------------------------
void InitWindow() {
	if (!glfwInit()) {
		throw(std::runtime_error("Could not initialize the GLFW library"));
	}

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw(std::runtime_error("Could not create window"));
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		throw(std::runtime_error(std::string("Could not initialize GLEW: ") + (const char*)glewGetErrorString(err)));
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//-----------------------------------------------------------
// Initialize camera position, orientation and perspective
// Places camera high enough to see the full terrain
//-----------------------------------------------------------
void InitCamera() {
	camera = new Camera();
	camera->SetCamera(glm::vec3(0.0f, -80.0f, 40.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	camera->SetPerspectiveView(camera_fov_g, (float)window_width_g / (float)window_height_g, camera_near_clip_distance_g, camera_far_clip_distance_g);

	glViewport(0, 0, window_width_g, window_height_g);
	projection_matrix = camera->GetProjectionMatrix(NULL);
}

//-----------------------------------------------------------
// Set common shader uniforms for a given frame
// Uploads model/view/projection matrices and sun direction
//-----------------------------------------------------------
void SetFrameUniforms(GLuint shader, const glm::mat4& model, float current_time) {
	// model / view / projection (legacy names)
	GLint locModel = glGetUniformLocation(shader, "model");
	if (locModel != -1) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
	GLint locView = glGetUniformLocation(shader, "view");
	if (locView != -1) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view_matrix));
	GLint locProj = glGetUniformLocation(shader, "projection");
	if (locProj != -1) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	// model / view / projection (textureShader names)
	GLint locWorld = glGetUniformLocation(shader, "world_mat");
	if (locWorld != -1) glUniformMatrix4fv(locWorld, 1, GL_FALSE, glm::value_ptr(model));
	GLint locViewMat = glGetUniformLocation(shader, "view_mat");
	if (locViewMat != -1) glUniformMatrix4fv(locViewMat, 1, GL_FALSE, glm::value_ptr(view_matrix));
	GLint locProjMat = glGetUniformLocation(shader, "projection_mat");
	if (locProjMat != -1) glUniformMatrix4fv(locProjMat, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	// Sun directional light — revolves around the scene like a day/night cycle
	float sunSpeed = 0.5f;
	float sunAngle = current_time * sunSpeed;

	glm::vec3 sunDirWorld = glm::normalize(glm::vec3(
		std::cos(sunAngle),
		0.3f,
		std::abs(std::sin(sunAngle))
	));

	glm::vec3 sunDirView = glm::vec3(view_matrix * glm::vec4(sunDirWorld, 0.0f));
	GLint locSunDir = glGetUniformLocation(shader, "sunDirection");
	if (locSunDir != -1) glUniform3fv(locSunDir, 1, glm::value_ptr(sunDirView));
}


// Main function
int main(void) {
	try {
		InitWindow();
		InitCamera();

		// Set callbacks
		glfwSetWindowUserPointer(window, (void*)&projection_matrix);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetFramebufferSizeCallback(window, ResizeCallback);

		PrintOpenGLInfo();

		// Load shaders
		GLuint myShader = LoadShaders("shaderGouraud");

		// Create terrain mesh from deterministic heightmap
		const float planeWidth = 50.0f;
		const float planeHeight = 50.0f;
		const int rows = 81;
		const int cols = 81;

		std::vector<float> heights;
		GenerateTerrainHeights(rows, cols, heights);

		GLuint planeIndexCount = 0;
		GLuint planeVAO = CreateBumpyPlaneVAO(planeWidth, planeHeight, rows, cols, heights, planeIndexCount);

		// Create water plane halfway into the pit
		GLuint waterIndexCount = 0;
		float waterHeight = -2.5f;
		float waterWidth = 30.0f;
		float waterHeightSize = 30.0f;
		GLuint waterVAO = CreateWaterPlaneVAO(waterWidth, waterHeightSize, waterHeight, waterIndexCount);

		// Main loop
		float last_time = glfwGetTime();
		while (!glfwWindowShouldClose(window)) {
			float current_time = glfwGetTime();
			float delta = current_time - last_time;
			last_time = current_time;

			ProcessInput(window, camera, delta);

			glClearColor(background.r, background.g, background.b, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			view_matrix = camera->GetViewMatrix(NULL);
			projection_matrix = camera->GetProjectionMatrix(NULL);

			// Use shader
			glUseProgram(myShader);

			glm::mat4 model = glm::mat4(1.0f);
			SetFrameUniforms(myShader, model, current_time);

			// Draw terrain
			GLint locShapeColor = glGetUniformLocation(myShader, "shape_color");
			if (locShapeColor != -1) glUniform3f(locShapeColor, 0.5f, 1.0f, 0.2f);

			glBindVertexArray(planeVAO);
			glDrawElements(GL_TRIANGLES, planeIndexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Draw water plane
			glm::mat4 waterModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.02f));
			SetFrameUniforms(myShader, waterModel, current_time);

			if (locShapeColor != -1) glUniform3f(locShapeColor, 0.0f, 0.35f, 0.9f);

			glBindVertexArray(waterVAO);
			glDrawElements(GL_TRIANGLES, waterIndexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glUseProgram(0);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		delete camera;
		glfwTerminate();
	}
	catch (std::exception& e) {
		PrintException(e);
	}

	return 0;
}