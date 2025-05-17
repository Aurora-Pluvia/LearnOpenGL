#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

//创建uniform变量模型变换矩阵、观察变换矩阵、投影变换矩阵
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //应用变换矩阵
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}