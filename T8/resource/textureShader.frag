#version 330 

in vec4 f_color;
in vec2 f_uv;
in float f_time;

out vec4 fragColor;

uniform sampler2D texture_map;
uniform sampler2D texture_map2;
void main(void)
{

	// Retrieve texture value
	vec2 new_uv = f_uv;
	new_uv.x +=f_time*0.1;
	new_uv.y = new_uv.y;
    vec4 pixel1 = texture(texture_map, new_uv);
    vec4 pixel2 = texture(texture_map2, new_uv);

	vec4 pixel = 0.5*(pixel1+pixel2);
    // Use texture in determining fragment colour
    gl_FragColor = pixel;
	//Save the color

	//IF ON WINDOWS USE gl_FragColor
	gl_FragColor = pixel;

	//IF ON MAC USE fragColor
	//fragColor = pixel;
}
