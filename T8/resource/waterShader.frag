#version 330

in vec3 fNormal;
in vec3 fPos;
in vec3 fColor;

out vec4 outColor;

// Sun direction (set from main.cpp, same as terrain shader)
uniform vec3 sunDirection = vec3(0.5, 0.3, 1.0);

//the colors that the light shines on for the modal 
uniform vec3 ambient_color =  vec3(1);
uniform vec3 diffuse_color =  vec3(1);
uniform vec3 specular_color = vec3(1.0);

//water color
uniform vec3 shape_color = vec3(0.0, 0.35, 0.9);

//the coefficiants for the light modals
uniform float coefA = 0.15;
uniform float coefD = 2.0;	  // same diffuse as terrain for consistency
uniform float coefS = 50.0;    // water is shiny — higher specular

//the shine used for the specular lighting.
uniform float shine = 64.0f;  // tight specular highlight for water

// Water transparency (0 = invisible, 1 = opaque)
uniform float waterAlpha = 0.6;

void main(){

	//normalize interpolated normal
	vec3 N = normalize(fNormal);

	//view vector (camera at origin in view space)
	vec3 V = normalize(-fPos);

	// Directional light
	vec3 L = normalize(sunDirection);

	//ambient
	vec3 lighting = coefA * ambient_color;

	//diffuse----------------------------------------
	float NdotL = max(dot(L, N), 0.0);
	vec3 diffuse = coefD * diffuse_color * NdotL;

	//specular----------------------------------------
	vec3 R = reflect(-L, N);
	vec3 specular = coefS * specular_color *
	                pow(max(dot(R, V), 0.0), shine);

	lighting += diffuse + specular;

	//soft clamp to avoid full white saturation
	lighting = lighting / (lighting + vec3(1.0));

	vec3 finalColor = shape_color * lighting;

	// Semi-transparent output
	outColor = vec4(finalColor, waterAlpha);
}