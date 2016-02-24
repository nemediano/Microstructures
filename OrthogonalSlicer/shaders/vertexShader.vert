#version 150
in vec3 position_attribute;
in vec2 texture_coordinates_attribute;
uniform mat4 Proj;
uniform mat4 VM;
uniform vec3 displacement;
out vec2 text_coord;

void main(void){
	vec4 position;
	position = vec4(position_attribute, 1.0) + vec4(displacement, 0.0);
	text_coord = texture_coordinates_attribute;  
	gl_Position = Proj * VM * position;
	//gl_Position = position;
}
