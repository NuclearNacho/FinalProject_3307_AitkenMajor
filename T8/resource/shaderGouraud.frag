#version 330

in vec3 fNormal;
in vec3 fPos;

out vec4 outColor;

//Uniform for the light position. By default its set to whatever value below. 
uniform vec3 lightPos = vec3(4.0,0,2);

//the colors that the light shines on for the modal 
uniform vec3 ambient_color =  vec3(1);
uniform vec3 diffuse_color =  vec3(1);
uniform vec3 specular_color = vec3(0.6); // reduced specular base to avoid bright highlights

//the default color that is used as the basis.
uniform vec3 shape_color = vec3(0.5,1.0,0.2);

//the coefficiants for the light modals (i.e. ambience is 10%)
uniform float coefA = 0.06;   // slightly reduced ambient
uniform float coefD = 0.8;    // reduced diffuse
uniform float coefS = 0.4;    // substantially reduced specular

//the shine used for the specular lighting. The larger the value, the 
//brighter it is
uniform float shine = 16.0f;

void main(){

	vec3 lightPosArr [2];
	lightPosArr[0] = vec3(4,0,2);
	lightPosArr[1] = vec3(-2,2,1);

	//normalize interpolated normal (required for Phong)
	vec3 N = normalize(fNormal);

	//view vector (camera at origin in view space)
	vec3 V = normalize(-fPos);

	//ambient added only once
	vec3 lighting = coefA * ambient_color;

	for(int i = 0; i < 2; i++)
	{
		vec3 L = normalize(lightPosArr[i] - fPos);

		//diffuse----------------------------------------
		vec3 diffuse = coefD * diffuse_color *
		               max(dot(L, N), 0.0);

		//specular----------------------------------------
		vec3 R = reflect(-L, N);
		vec3 specular = coefS * specular_color *
		                pow(max(dot(R, V), 0.0), shine);

		lighting += diffuse + specular;
	}

	//soft clamp to avoid full white saturation while preserving contrast
	lighting = lighting / (lighting + vec3(1.0));

	vec3 finalColor = shape_color * lighting;

	outColor = vec4(finalColor, 1.0);
}
