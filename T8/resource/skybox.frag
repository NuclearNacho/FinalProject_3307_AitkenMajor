#version 330

in vec3 fTexCoord;

out vec4 outColor;

uniform samplerCube skyboxTex;

void main(){
	outColor = texture(skyboxTex, fTexCoord);
}