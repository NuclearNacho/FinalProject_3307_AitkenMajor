#version 330

in vec3 vertex;

uniform mat4 lightSpaceMatrix;
uniform mat4 world_mat;

void main(){
	gl_Position = lightSpaceMatrix * world_mat * vec4(vertex, 1.0);
}