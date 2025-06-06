#version 330 core
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

//材质结构体
struct Material{
    vec3 ambient;//定义了在环境光照下这个表面反射的是什么颜色，通常与表面的颜色相同
    vec3 diffuse;//定义了在漫反射光照下表面的颜色，也被设置为期望的物体颜色
    vec3 specular;//表面上镜面高光的颜色
    float shininess;//影响镜面高光的散射/半径
};

//对不同的反射类型应用不同的光照强度
struct Light{
    vec3 position;//光源位置
    vec3 ambient;//环境光照
    vec3 diffuse;//漫反射
    vec3 specular;//镜面光照
};

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;//材质
uniform Light light;//光照强度与位置

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}