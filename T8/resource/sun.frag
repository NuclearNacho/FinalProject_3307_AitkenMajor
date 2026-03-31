#version 330

out vec4 outColor;

// Sun color — bright yellow, unaffected by lighting
uniform vec3 sunColor = vec3(1.0, 0.9, 0.2);

void main(){
	outColor = vec4(sunColor, 1.0);
}