#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;//法线

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;//片元位置

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //把法线转换为世界坐标，使用法线矩阵与法线向量相乘，得到世界坐标下的法线向量
    //法线矩阵是模型矩阵左上角3x3部分的逆矩阵的转置矩阵
    //因为模型矩阵的逆矩阵是模型矩阵的正交矩阵，正交矩阵的转置矩阵等于其逆矩阵
    //所以法线矩阵是模型矩阵的正交矩阵的转置矩阵的逆矩阵的转置矩阵，即模型矩阵的逆矩阵的转置矩阵
    //矩阵求逆是一项对于着色器开销很大的运算，因为它必须在场景中的每一个顶点上进行，所以应该尽可能地避免在着色器中进行求逆运算
    //最好先在CPU上计算出法线矩阵，再通过uniform把它传递给着色器（就像模型矩阵一样）
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));//把顶点坐标转换为世界坐标
}