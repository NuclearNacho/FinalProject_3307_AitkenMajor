#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>
#include <fetchInput.h>
#include <GL/SOIL.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstring>
#include <iostream>

// Path to shader directory
#define DIRECTORY "C:/Users/nmajo/Downloads/FinalProject_3307_AitkenMajor-master/FinalProject_3307_AitkenMajor-master/T8/"

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

// Lighting toggles
bool enableAmbient = true;
bool enableDiffuse = true;
bool enableSpecular = true;

bool enableWaterAmbient = true;
bool enableWaterDiffuse = true;
bool enableWaterSpecular = true;

//-----------------------------------------------------------
// Scene geometry handles returned from initialization
//-----------------------------------------------------------
struct SceneGeometry {
	GLuint terrainVAO;
	GLuint terrainIndexCount;
	GLuint waterVAO;
	GLuint waterIndexCount;
	GLuint skyboxVAO;
	GLuint skyboxTexture;
	GLuint sunVAO;
	GLuint sunIndexCount;
	GLuint grassTexture;
};


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
	glBindAttribLocation(shader, 3, "uv");

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
// Create terrain mesh with positions, colors, normals AND UVs
//-----------------------------------------------------------
GLuint CreateBumpyPlaneVAO(float width, float height,int rows, int cols,const std::vector<float>& heights,GLuint& outIndexCount)
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
	// Build vertex buffer: position + color + normal + UV
	// 11 floats per vertex (x, y, z, r, g, b, nx, ny, nz, u, v)
	//-------------------------------------------------------
	std::vector<GLfloat> vertexData;
	vertexData.reserve(rows * cols * 11);

	float halfW = width * 0.5f;
	float halfH = height * 0.5f;

	float hmin = heights[0], hmax = heights[0];
	for (float h : heights) {
		hmin = std::min(hmin, h);
		hmax = std::max(hmax, h);
	}
	float range = (hmax - hmin == 0) ? 1.0f : (hmax - hmin);

	// UV tiling — how many times the grass texture repeats across the terrain
	float uvTile = 4.0f;

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

			// UV (tiled)
			vertexData.push_back(u * uvTile);
			vertexData.push_back(v * uvTile);
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

	// position (location 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)0);

	// color (location 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// normal (location 2)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	// UV (location 3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));

	glBindVertexArray(0);
	return vao;
}


//-----------------------------------------------------------
// Create a water plane grid mesh at a constant z height
// Uses a grid so the vertex shader can animate each vertex
// Same vertex format: position + color + normal
//-----------------------------------------------------------
GLuint CreateWaterPlaneVAO(float width, float height, float zHeight,
	int rows, int cols, GLuint& outIndexCount)
{
	float halfW = width * 0.5f;
	float halfH = height * 0.5f;

	//-------------------------------------------------------
	// Build vertex buffer: position + color + normal
	//-------------------------------------------------------
	std::vector<GLfloat> vertexData;
	vertexData.reserve(rows * cols * 9);

	for (int r = 0; r < rows; ++r) {
		float v = (float)r / (rows - 1);
		float y = v * height - halfH;

		for (int c = 0; c < cols; ++c) {
			float u = (float)c / (cols - 1);
			float x = u * width - halfW;

			// position (flat at zHeight — vertex shader will animate)
			vertexData.push_back(x);
			vertexData.push_back(y);
			vertexData.push_back(zHeight);

			// color (blue)
			vertexData.push_back(0.0f);
			vertexData.push_back(0.35f);
			vertexData.push_back(0.9f);

			// normal (pointing up — vertex shader will recalculate)
			vertexData.push_back(0.0f);
			vertexData.push_back(0.0f);
			vertexData.push_back(1.0f);
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
// Create a unit cube VAO for the skybox
// 36 vertices (no indices needed), positions only
//-----------------------------------------------------------
GLuint CreateSkyboxVAO() {
	float vertices[] = {
		// Back face
		-1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
		 // Front face
		 -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
		  // Left face
		  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,
		  -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,
		  // Right face
		   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
		   1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
		   // Top face
		   -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,
			// Bottom face
			-1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position only (location 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	return vao;
}


//-----------------------------------------------------------
// Load skybox cubemap from 3 separate images:
//   wallPath  -> 4 side faces (+X, -X, +Z, -Z)
//   topPath   -> top face (+Y)
//   floorPath -> bottom face (-Y)
//-----------------------------------------------------------
GLuint LoadSkyboxTexture(const std::string& wallPath,
	const std::string& topPath,
	const std::string& floorPath)
{
	// Helper lambda to load an image
	auto loadImage = [](const std::string& path, int& w, int& h) -> unsigned char* {
		int channels;
		unsigned char* img = SOIL_load_image(path.c_str(), &w, &h, &channels, SOIL_LOAD_RGB);
		if (!img) {
			throw std::runtime_error(
				std::string("Failed to load skybox image: ") + path +
				"\nSOIL error: " + SOIL_last_result());
		}
		return img;
		};

	// Flip vertically (swap rows top-to-bottom)
	auto flipVertical = [](unsigned char* src, int w, int h) -> std::vector<unsigned char> {
		int rowBytes = w * 3;
		std::vector<unsigned char> flipped(w * h * 3);
		for (int r = 0; r < h; ++r) {
			memcpy(&flipped[r * rowBytes],
				&src[(h - 1 - r) * rowBytes],
				rowBytes);
		}
		return flipped;
		};

	// Flip horizontally (mirror each row left-to-right)
	auto flipHorizontal = [](const unsigned char* src, int w, int h) -> std::vector<unsigned char> {
		int rowBytes = w * 3;
		std::vector<unsigned char> flipped(w * h * 3);
		for (int r = 0; r < h; ++r) {
			for (int c = 0; c < w; ++c) {
				int srcIdx = r * rowBytes + c * 3;
				int dstIdx = r * rowBytes + (w - 1 - c) * 3;
				flipped[dstIdx + 0] = src[srcIdx + 0];
				flipped[dstIdx + 1] = src[srcIdx + 1];
				flipped[dstIdx + 2] = src[srcIdx + 2];
			}
		}
		return flipped;
		};

	// Load all 3 images
	int wallW, wallH, topW, topH, floorW, floorH;
	unsigned char* wallImg = loadImage(wallPath, wallW, wallH);
	unsigned char* topImg = loadImage(topPath, topW, topH);
	unsigned char* floorImg = loadImage(floorPath, floorW, floorH);

	// Create flipped copies
	std::vector<unsigned char> wallFlipped = flipVertical(wallImg, wallW, wallH);
	std::vector<unsigned char> wallHFlipped = flipHorizontal(wallFlipped.data(), wallW, wallH);

	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	// +X  right  (wall — normal)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, wallW, wallH, 0,
		GL_RGB, GL_UNSIGNED_BYTE, wallFlipped.data());
	// -X  left   (wall — horizontally flipped)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, wallW, wallH, 0,
		GL_RGB, GL_UNSIGNED_BYTE, wallHFlipped.data());
	// +Y  top    (sky)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, topW, topH, 0,
		GL_RGB, GL_UNSIGNED_BYTE, topImg);
	// -Y  bottom (floor)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, floorW, floorH, 0,
		GL_RGB, GL_UNSIGNED_BYTE, floorImg);
	// +Z  front  (wall — normal)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, wallW, wallH, 0,
		GL_RGB, GL_UNSIGNED_BYTE, wallFlipped.data());
	// -Z  back   (wall — horizontally flipped)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, wallW, wallH, 0,
		GL_RGB, GL_UNSIGNED_BYTE, wallHFlipped.data());

	SOIL_free_image_data(wallImg);
	SOIL_free_image_data(topImg);
	SOIL_free_image_data(floorImg);

	// Cubemap texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texID;
}

//-----------------------------------------------------------
// Load a 2D grass texture SOIL
//-----------------------------------------------------------
GLuint LoadTexture(const std::string& filepath) {
	GLuint texID = SOIL_load_OGL_texture(
		filepath.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
	);

	if (texID == 0) {
		throw std::runtime_error(
			std::string("Failed to load texture: ") + filepath +
			"\nSOIL error: " + SOIL_last_result());
	}

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}


//-----------------------------------------------------------
// Create a UV sphere VAO for the sun visual
// Generates a sphere of given radius with lat/lon subdivisions
//-----------------------------------------------------------
GLuint CreateSphereVAO(float radius, int stacks, int slices, GLuint& outIndexCount) {
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	// Generate vertices
	for (int i = 0; i <= stacks; ++i) {
		float phi = glm::pi<float>() * (float)i / (float)stacks;  // 0 to PI
		for (int j = 0; j <= slices; ++j) {
			float theta = 2.0f * glm::pi<float>() * (float)j / (float)slices;  // 0 to 2PI

			float x = radius * std::sin(phi) * std::cos(theta);
			float y = radius * std::sin(phi) * std::sin(theta);
			float z = radius * std::cos(phi);

			// position
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	// Generate indices
	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			GLuint first = i * (slices + 1) + j;
			GLuint second = first + slices + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	outIndexCount = (GLuint)indices.size();

	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(GLfloat),
		vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(GLuint),
		indices.data(), GL_STATIC_DRAW);

	// position only (location 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glBindVertexArray(0);
	return vao;
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
	const float maxPeak = 14.0f;
	const float falloff = 2.0f;
	const float ridgeAmp = 7.0f;
	const float freqX = 5.0f;
	const float freqY = 11.0f;

	// Radial inversion setup
	const float maxRadius = std::sqrt(2.0f);

	for (int r = 0; r < rows; ++r) {
		float v = (rows == 1) ? 0.5f : (float)r / (float)(rows - 1);
		float yNorm = v * 2.0f - 1.0f;

		for (int c = 0; c < cols; ++c) {
			float u = (cols == 1) ? 0.5f : (float)c / (float)(cols - 1);
			float xNorm = u * 2.0f - 1.0f;

			float radial = std::exp(-((xNorm * xNorm + yNorm * yNorm) * falloff));
			float base = maxPeak * radial;
			float ridges = ridgeAmp * std::sin(xNorm * freqX) * std::cos(yNorm * freqY) * (0.4f + 0.6f * radial);
			float waves = 1.5f * (std::sin(xNorm * 3.0f) + std::cos(yNorm * 2.5f)) * (1.0f - 0.6f * radial);

			float bumps1 = 1.6f * std::sin(xNorm * 15.0f + yNorm * 7.0f) * (0.3f + 0.7f * radial);

			float h_orig = base + ridges + waves + bumps1;

			float rDist = std::sqrt(xNorm * xNorm + yNorm * yNorm);
			float rNorm = rDist / maxRadius;
			if (rNorm < 0.0f) rNorm = 0.0f;
			if (rNorm > 1.0f) rNorm = 1.0f;

			float factor = 2.0f * rNorm - 1.0f;
			float h = h_orig * factor;

			heights[r * cols + c] = h;
		}
	}
}


//-----------------------------------------------------------
// Create all scene geometry (terrain + water + skybox)
// Generates the heightmap, builds the terrain mesh,
// creates the water plane, and loads the skybox cubemap
//-----------------------------------------------------------
SceneGeometry CreateSceneGeometry() {
	// Terrain mesh from deterministic heightmap
	const float planeWidth = 50.0f;
	const float planeHeight = 50.0f;
	const int rows = 81;
	const int cols = 81;

	std::vector<float> heights;
	GenerateTerrainHeights(rows, cols, heights);

	SceneGeometry geo;
	geo.terrainVAO = CreateBumpyPlaneVAO(planeWidth, planeHeight, rows, cols, heights, geo.terrainIndexCount);

	// Water plane grid — enough subdivisions for smooth wave animation
	float waterHeight = -2.5f;
	float waterWidth = 30.0f;
	float waterHeightSize = 30.0f;
	int waterRows = 40;
	int waterCols = 40;
	geo.waterVAO = CreateWaterPlaneVAO(waterWidth, waterHeightSize, waterHeight, waterRows, waterCols, geo.waterIndexCount);

	// Skybox
	geo.skyboxVAO = CreateSkyboxVAO();
	std::string assetsDir = std::string(DIRECTORY) + "assets/";
	geo.skyboxTexture = LoadSkyboxTexture(
		assetsDir + "skybox.png",    // 4 walls
		assetsDir + "skyfloor.png",  // top face (was skysky — swapped)
		assetsDir + "skysky.png"     // bottom face (was skyfloor — swapped)
	);

	// Sun sphere
	geo.sunVAO = CreateSphereVAO(5.0f, 20, 20, geo.sunIndexCount);
	// Grass texture for terrain
	geo.grassTexture = LoadTexture(std::string(DIRECTORY) + "assets/grass.jpg");
	return geo;
}


// Window resize callback
void ResizeCallback(GLFWwindow* window, int width, int height) {
	// Prevent division by zero on minimize
	if (width == 0 || height == 0) return;

	glViewport(0, 0, width, height);

	// Update the camera's aspect ratio so the projection matrix
	// stays correct and nothing stretches when resizing/fullscreening
	camera->SetPerspectiveView(camera_fov_g,
		(float)width / (float)height,
		camera_near_clip_distance_g,
		camera_far_clip_distance_g);

	projection_matrix = camera->GetProjectionMatrix(NULL);
}


// Print OpenGL info
void PrintOpenGLInfo() {
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
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
		GLuint myShader = LoadShaders("shaderPhong");
		GLuint waterShader = LoadShaders("waterShader");
		GLuint skyboxShader = LoadShaders("skybox");
		GLuint sunShader = LoadShaders("sun");

		// Create all scene geometry
		SceneGeometry scene = CreateSceneGeometry();

		// Main loop
		float last_time = (float)glfwGetTime();
		while (!glfwWindowShouldClose(window)) {
			float current_time = (float)glfwGetTime();
			float delta = current_time - last_time;
			last_time = current_time;

			ProcessInput(window, camera, delta);

			glClearColor(background.r, background.g, background.b, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			view_matrix = camera->GetViewMatrix(NULL);
			projection_matrix = camera->GetProjectionMatrix(NULL);
			// ---- Draw skybox (drawn first, behind everything) ----
			glDepthFunc(GL_LEQUAL);   // allow fragments at max depth (z = 1.0)
			glDepthMask(GL_FALSE);    // disable depth writing so skybox stays behind
			glUseProgram(skyboxShader);

			// Pass view + projection to skybox shader
			GLint locSkyView = glGetUniformLocation(skyboxShader, "view_mat");
			if (locSkyView != -1) glUniformMatrix4fv(locSkyView, 1, GL_FALSE, glm::value_ptr(view_matrix));
			GLint locSkyProj = glGetUniformLocation(skyboxShader, "projection_mat");
			if (locSkyProj != -1) glUniformMatrix4fv(locSkyProj, 1, GL_FALSE, glm::value_ptr(projection_matrix));

			// Bind cubemap texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, scene.skyboxTexture);
			GLint locSkyTex = glGetUniformLocation(skyboxShader, "skyboxTex");
			if (locSkyTex != -1) glUniform1i(locSkyTex, 0);

			glBindVertexArray(scene.skyboxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			glUseProgram(0);
			glDepthMask(GL_TRUE);     // re-enable depth writing
			glDepthFunc(GL_LESS);     // restore default depth function

			// ---- Draw sun sphere (follows light source position) ----
			glUseProgram(sunShader);

			// Compute sun world position — same formula as SetFrameUniforms
			float sunSpeed = 0.5f;
			float sunAngle = current_time * sunSpeed;
			float sunDistance = 40.0f;  // how far from origin the sun orbits

			glm::vec3 sunPos = sunDistance * glm::normalize(glm::vec3(
				std::cos(sunAngle),
				0.3f,
				std::abs(std::sin(sunAngle))
			));

			// Translate + scale the sphere to the sun's position
			glm::mat4 sunModel = glm::translate(glm::mat4(1.0f), sunPos);

			GLint locSunWorld = glGetUniformLocation(sunShader, "world_mat");
			if (locSunWorld != -1) glUniformMatrix4fv(locSunWorld, 1, GL_FALSE, glm::value_ptr(sunModel));
			GLint locSunView = glGetUniformLocation(sunShader, "view_mat");
			if (locSunView != -1) glUniformMatrix4fv(locSunView, 1, GL_FALSE, glm::value_ptr(view_matrix));
			GLint locSunProj = glGetUniformLocation(sunShader, "projection_mat");
			if (locSunProj != -1) glUniformMatrix4fv(locSunProj, 1, GL_FALSE, glm::value_ptr(projection_matrix));

			glBindVertexArray(scene.sunVAO);
			glDrawElements(GL_TRIANGLES, scene.sunIndexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// ---- Draw terrain (opaque) ----
			glUseProgram(myShader);

			// Lighting toggle implementation for terrain shader
			GLint locA = glGetUniformLocation(myShader, "coefA");
			GLint locD = glGetUniformLocation(myShader, "coefD");
			GLint locS = glGetUniformLocation(myShader, "coefS");

			if (locA != -1) glUniform1f(locA, enableAmbient * 0.15f);
			if (locD != -1) glUniform1f(locD, enableDiffuse * 2.0f);
			if (locS != -1) glUniform1f(locS, enableSpecular * 0.6f);

			glm::mat4 model = glm::mat4(1.0f);
			SetFrameUniforms(myShader, model, current_time);

			// Bind grass texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, scene.grassTexture);
			GLint locGrassTex = glGetUniformLocation(myShader, "grassTex");
			if (locGrassTex != -1) glUniform1i(locGrassTex, 0);
			GLint locUseTex = glGetUniformLocation(myShader, "useTexture");
			if (locUseTex != -1) glUniform1i(locUseTex, 1);  // enable texture

			GLint locShapeColor = glGetUniformLocation(myShader, "shape_color");
			if (locShapeColor != -1) glUniform3f(locShapeColor, 0.5f, 1.0f, 0.2f);

			glBindVertexArray(scene.terrainVAO);
			glDrawElements(GL_TRIANGLES, scene.terrainIndexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);  // unbind texture
			glUseProgram(0);

			// ---- Draw water (transparent, drawn after terrain) ----
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(waterShader);

			// Lighting toggle implementation for water shader
			GLint locWaterA = glGetUniformLocation(waterShader, "coefA");
			GLint locWaterD = glGetUniformLocation(waterShader, "coefD");
			GLint locWaterS = glGetUniformLocation(waterShader, "coefS");

			if (locWaterA != -1) glUniform1f(locWaterA, enableWaterAmbient * 0.15f);
			if (locWaterD != -1) glUniform1f(locWaterD, enableWaterDiffuse * 2.0f);
			if (locWaterS != -1) glUniform1f(locWaterS, enableWaterSpecular * 50.0f);

			glm::mat4 waterModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.02f));
			SetFrameUniforms(waterShader, waterModel, current_time);

			GLint locTime = glGetUniformLocation(waterShader, "time");
			if (locTime != -1) glUniform1f(locTime, current_time);

			GLint locWaterColor = glGetUniformLocation(waterShader, "shape_color");
			if (locWaterColor != -1) glUniform3f(locWaterColor, 0.0f, 0.35f, 0.9f);

			glBindVertexArray(scene.waterVAO);
			glDrawElements(GL_TRIANGLES, scene.waterIndexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glUseProgram(0);

			glDisable(GL_BLEND);

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