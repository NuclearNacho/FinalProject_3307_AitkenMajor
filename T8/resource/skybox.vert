#version 330

in vec3 vertex;

out vec3 fTexCoord;

uniform mat4 view_mat;
uniform mat4 projection_mat;

void main(){
	// Remap axes: scene uses Z-up, but cubemaps use Y-up
	// Swizzle so that scene Z -> cubemap Y, scene Y -> cubemap -Z
	fTexCoord = vec3(vertex.x, -vertex.z, -vertex.y);

	// Remove translation from view matrix so skybox stays centered on camera
	mat4 rotView = mat4(mat3(view_mat));

	vec4 pos = projection_mat * rotView * vec4(vertex, 1.0);

	// Set z = w so the skybox is always at maximum depth (behind everything)
	gl_Position = pos.xyww;
}