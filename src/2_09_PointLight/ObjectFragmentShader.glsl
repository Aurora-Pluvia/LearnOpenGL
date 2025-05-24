#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light{
    //点光源需要光源位置
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //计算点光源衰减时的参数    
    float constant;//常数项
    float linear;//一次项
    float quadratic;//二次项
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    //片元与光源的距离
    float distance = length(light.position - FragPos);
    //计算衰减
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //应用衰减  
    //可以将环境光分量保持不变，让环境光照不会随着距离减少
    //但是如果我们使用多于一个的光源，所有的环境光分量将会开始叠加，所以在这种情况下我们也希望衰减环境光照
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}