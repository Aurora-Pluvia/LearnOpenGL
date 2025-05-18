#version 330 core
out vec4 FragColor;

in vec3 LightingColor; 

uniform vec3 objectColor;

//Gouraud着色的所有光照计算放在顶点着色器中
void main()
{
   FragColor = vec4(LightingColor * objectColor, 1.0);
}