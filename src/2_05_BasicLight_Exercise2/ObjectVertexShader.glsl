#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;//把光源位置定义在顶点着色器中

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;//输出计算后的观察空间中的光源位置

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //在观察空间中计算光照
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    LightPos = vec3(view * vec4(lightPos, 1.0));
}