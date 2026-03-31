#version 330

in vec3 fNormal;
in vec3 fPos;
in vec2 fUV;

out vec4 outColor;

// Sun direction: points FROM the sun toward the scene
// Passed from main.cpp; default = upper-right behind camera
uniform vec3 sunDirection = vec3(0.5, 0.3, 1.0);

//the colors that the light shines on for the modal 
uniform vec3 ambient_color =  vec3(1);
uniform vec3 diffuse_color =  vec3(1);
uniform vec3 specular_color = vec3(0.6);

//the default color that is used as the basis.
uniform vec3 shape_color = vec3(0.5,1.0,0.2);

//the coefficiants for the light modals
uniform float coefA = 0.15;   // low ambient = darker shadows
uniform float coefD = 2.0;    // diffuse
uniform float coefS = 0.6;    // specular

//the shine used for the specular lighting.
uniform float shine = 16.0f;

// Shadow exaggeration: higher = sharper/darker shadow falloff
uniform float shadowHardness = 10.0;

// Grass texture
uniform sampler2D grassTex;
uniform bool useTexture = false;

void main(){

	//normalize interpolated normal (required for Phong)
	vec3 N = normalize(fNormal);

	//view vector (camera at origin in view space)
	vec3 V = normalize(-fPos);

	// Directional light: normalize sun direction (points toward the light)
	vec3 L = normalize(sunDirection);

	//ambient
	vec3 lighting = coefA * ambient_color;

	//diffuse----------------------------------------
	float NdotL = max(dot(L, N), 0.0);
	vec3 diffuse = coefD * diffuse_color *
	               pow(NdotL, shadowHardness);

	//specular----------------------------------------
	vec3 R = reflect(-L, N);
	vec3 specular = coefS * specular_color *
	                pow(max(dot(R, V), 0.0), shine);

	lighting += diffuse + specular;

	//soft clamp to avoid full white saturation while preserving contrast
	lighting = lighting / (lighting + vec3(1.0));

	// Use grass texture if enabled, otherwise fall back to shape_color
	vec3 baseColor;
	if (useTexture) {
		baseColor = texture(grassTex, fUV).rgb;
	} else {
		baseColor = shape_color;
	}

	vec3 finalColor = baseColor * lighting;

	outColor = vec4(finalColor, 1.0);
}