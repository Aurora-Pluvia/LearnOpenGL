#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;//纹理坐标
out vec4 FragColor;

struct Material{
    //一处环境光材质颜色向量，因为环境光颜色在几乎所有情况下都等于漫反射颜色
    sampler2D diffuse;//使用采样器采样漫反射贴图替换原先的漫反射颜色向量
    sampler2D specular;//使用采样器采样镜面光贴图替换原先的镜面反射颜色向量
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    //在计算环境光时也应用漫反射贴图颜色
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //应用漫反射贴图采样器获得的颜色信息
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //应用镜面光贴图采样器获得的颜色信息
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}