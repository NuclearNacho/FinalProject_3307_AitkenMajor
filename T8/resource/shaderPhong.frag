
in vec3 fNormal;
in vec3 fPos;
in vec2 fUV;
in vec4 fPosLightSpace;

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
uniform float coefA = 0.2;   // low ambient = darker shadows
uniform float coefD = 2.0;    // diffuse
uniform float coefS = 0.6;    // specular

//the shine used for the specular lighting.
uniform float shine = 16.0f;

// Shadow exaggeration: higher = sharper/darker shadow falloff
uniform float shadowHardness = 5.0;

// Grass texture
uniform sampler2D grassTex;
uniform bool useTexture = false;

// Shadow map
uniform sampler2D shadowMap;

//-----------------------------------------------------------
// Compute shadow factor: 0.0 = fully in shadow, 1.0 = fully lit
// Uses PCF (percentage-closer filtering) for soft edges
//-----------------------------------------------------------
float CalcShadow(vec4 posLightSpace, vec3 normal, vec3 lightDir) {
	// Perspective divide (clip space -> NDC)
	vec3 projCoords = posLightSpace.xyz / posLightSpace.w;

	// Transform from [-1,1] to [0,1] for texture sampling
	projCoords = projCoords * 0.5 + 0.5;

	// Fragments outside the shadow map are lit
	if (projCoords.z > 1.0)
		return 1.0;

	// Bias to prevent shadow acne — scales with surface angle to light
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

	// PCF: sample a 3x3 neighborhood for soft shadow edges
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (projCoords.z - bias > closestDepth) ? 0.0 : 1.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

void main(){

	//normalize interpolated normal (required for Phong)
	vec3 N = normalize(fNormal);

	//view vector (camera at origin in view space)
	vec3 V = normalize(-fPos);

	// Directional light: normalize sun direction (points toward the light)
	vec3 L = normalize(sunDirection);

	//ambient
	vec3 lighting = coefA * ambient_color;

	// Shadow factor (1.0 = lit, 0.0 = shadowed)
	float shadow = CalcShadow(fPosLightSpace, N, L);

	//diffuse----------------------------------------
	float NdotL = max(dot(L, N), 0.0);
	vec3 diffuse = coefD * diffuse_color *
	               pow(NdotL, shadowHardness);

	//specular----------------------------------------
	vec3 R = reflect(-L, N);
	vec3 specular = coefS * specular_color *
	                pow(max(dot(R, V), 0.0), shine);

	// Shadow only darkens diffuse + specular (ambient stays)
	lighting += shadow * (diffuse + specular);

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