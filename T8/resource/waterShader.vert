#version 330

in vec3 vertex;
in vec3 color;
in vec3 normal;

//for phong, we send position and normals to fragment shader
out vec3 fNormal;
out vec3 fPos;
out vec3 fColor;

uniform float time;
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 world_mat;

// ---- Pseudo-random hash (deterministic noise from position) ----
// Returns a value in [-1, 1] that looks random but is repeatable
float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453) * 2.0 - 1.0;
}

// ---- Smooth value noise (interpolated hash) ----
// Gives organic-looking variation without harsh jumps
float noise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);

	// smooth interpolation curve
	vec2 u = f * f * (3.0 - 2.0 * f);

	// four corner hashes
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

void main(){

	// ---- Base sinusoidal waves ----
	float wave1 = 0.20 * sin(vertex.x * 1.5 + time * 2.0);
	float wave2 = 0.12 * cos(vertex.y * 2.0 + time * 1.5);
	float wave3 = 0.08 * sin(vertex.x * 3.0 + vertex.y * 2.5 + time * 3.0);

	// ---- Random impulses from value noise ----
	// Multiple noise layers at different scales + speeds for unpredictability
	float chop1 = 0.06 * noise(vertex.xy * 2.0 + time * 1.3);
	float chop2 = 0.04 * noise(vertex.xy * 5.0 - time * 2.1);
	float chop3 = 0.02 * noise(vertex.xy * 11.0 + time * 3.7);

	float totalDisplacement = wave1 + wave2 + wave3 + chop1 + chop2 + chop3;

	vec3 displaced = vertex;
	displaced.z += totalDisplacement;

	// ---- Analytically compute the normal from wave derivatives ----
	// Smooth wave derivatives (analytical)
	float dzdx = 0.20 * 1.5 * cos(vertex.x * 1.5 + time * 2.0)
	           + 0.08 * 3.0 * cos(vertex.x * 3.0 + vertex.y * 2.5 + time * 3.0);

	float dzdy = -0.12 * 2.0 * sin(vertex.y * 2.0 + time * 1.5)
	           + 0.08 * 2.5 * cos(vertex.x * 3.0 + vertex.y * 2.5 + time * 3.0);

	// Approximate noise derivatives via finite differences
	float eps = 0.05;
	float nX0 = 0.06 * noise((vertex.xy + vec2(eps, 0)) * 2.0 + time * 1.3)
	          + 0.04 * noise((vertex.xy + vec2(eps, 0)) * 5.0 - time * 2.1)
	          + 0.02 * noise((vertex.xy + vec2(eps, 0)) * 11.0 + time * 3.7);
	float nX1 = 0.06 * noise((vertex.xy - vec2(eps, 0)) * 2.0 + time * 1.3)
	          + 0.04 * noise((vertex.xy - vec2(eps, 0)) * 5.0 - time * 2.1)
	          + 0.02 * noise((vertex.xy - vec2(eps, 0)) * 11.0 + time * 3.7);

	float nY0 = 0.06 * noise((vertex.xy + vec2(0, eps)) * 2.0 + time * 1.3)
	          + 0.04 * noise((vertex.xy + vec2(0, eps)) * 5.0 - time * 2.1)
	          + 0.02 * noise((vertex.xy + vec2(0, eps)) * 11.0 + time * 3.7);
	float nY1 = 0.06 * noise((vertex.xy - vec2(0, eps)) * 2.0 + time * 1.3)
	          + 0.04 * noise((vertex.xy - vec2(0, eps)) * 5.0 - time * 2.1)
	          + 0.02 * noise((vertex.xy - vec2(0, eps)) * 11.0 + time * 3.7);

	// Combine analytical wave normals + noise normals
	dzdx += (nX0 - nX1) / (2.0 * eps);
	dzdy += (nY0 - nY1) / (2.0 * eps);

	vec3 waveNormal = normalize(vec3(-dzdx, -dzdy, 1.0));

	//convert vertex to view space
	vec4 vtxPos = view_mat * world_mat * vec4(displaced, 1);

	//proper normal transform using mat3 (avoids translation & interpolation artifacts)
	mat3 normalMat = transpose(inverse(mat3(view_mat * world_mat)));
	fNormal = normalize(normalMat * waveNormal);

	//send view-space position to fragment shader
	fPos = vec3(vtxPos);
	fColor = color;

	gl_Position = projection_mat * vtxPos;
}