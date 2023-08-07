#version 330 core
uniform vec3 objectColor;
in vec3 vertexColor; 
out vec4 FragColor; 
void main() 
{ 
	FragColor = vec4(objectColor.r, objectColor.g, objectColor.b, 1.0f);
}