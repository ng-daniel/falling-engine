#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main() {
  // no lighting for now
  FragColor = vec4(texture(texture_diffuse1, TexCoords).xyz, 1.0);
}