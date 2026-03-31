#version 330

in vec3 vertex;
in vec3 color;
in vec3 normal;

//for phong, we send position and normals to fragment shader
out vec3 fNormal;
out vec3 fPos;

uniform float time;
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 world_mat;

void main(){

	//convert vertex to view space
	vec4 vtxPos = view_mat * world_mat * vec4(vertex, 1);

	//proper normal transform using mat3 (avoids translation & interpolation artifacts)
	mat3 normalMat = transpose(inverse(mat3(view_mat * world_mat)));
	fNormal = normalize(normalMat * normal);

	//send view-space position to fragment shader
	fPos = vec3(vtxPos);

	gl_Position = projection_mat * vtxPos;
}