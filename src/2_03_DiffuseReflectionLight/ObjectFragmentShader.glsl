#version 330 core
in vec3 Normal;//法线
in vec3 FragPos;//片元位置
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  // 光源位置

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //法线归一化
    vec3 norm = normalize(Normal);
    //计算归一化光源方向
    vec3 lightDir = normalize(lightPos - FragPos);
    //计算光源对当前片段实际的漫反射影响，入射方向和法线方向之间的角度越小，入射角越小，漫反射分量越大
    //如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数，使用max函数保证漫反射分量不会变成负数
    float diff = max(dot(norm, lightDir), 0.0);
    //计算漫反射分量
    vec3 diffuse = diff * lightColor;
    //叠加环境光分量、漫反射分量与物体颜色
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}