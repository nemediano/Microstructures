#version 150
in vec2 text_coord;
uniform sampler2D texture_map;

out vec4 fragcolor;
void main(void){
	fragcolor = texture2D(texture_map, text_coord);
}