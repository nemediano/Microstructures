#version 150

in vec4 vPosition;
in vec4 vNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform int perspective;

out vec4 vColor;

vec4 ambient_term();
vec4 diffuse_term();
vec4 specular_term();

vec4 Color = vec4(0.8, 0.8, 0.0, 1.0);
vec4 lightDirection = vec4(0.0, 0.0, 1.0, 0.0);

void main() {
    gl_Position = P * V * M * vPosition;
	vColor = ambient_term() + diffuse_term();
}

vec4 ambient_term () {
	vec4 k_a = 0.75 * Color;
	vec4 l_a = vec4(0.45, 0.45, 0.45, 1.0);
	
	return l_a * k_a; 
}

vec4 diffuse_term () {
	vec4 k_d = 0.95 * Color;
	vec4 l_d = vec4(0.85, 0.85, 0.85, 1.0);

	mat4 MVT = transpose(inverse(M * V));

	vec4 n = normalize(MVT * mix(vNormal, -vNormal, perspective));
	//vec4 n = normalize(MVT * vNormal);

	return l_d * k_d * max(0.0, dot(n, normalize(lightDirection)));
}