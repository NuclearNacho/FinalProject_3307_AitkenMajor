#version 330

in vec3 vertex;

uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 world_mat;

void main(){
	gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
}