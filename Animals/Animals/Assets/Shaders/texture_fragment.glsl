#version 330 core

in vec3 vertexColor;
in vec2 vertexUV;
uniform vec3 objectColor;
uniform sampler2D textureSampler;

out vec4 FragColor;
void main()
{
   vec4 textureColor = texture( textureSampler, vertexUV );
   FragColor = textureColor;
};