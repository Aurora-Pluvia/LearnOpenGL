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
    //聚光灯同时需要光源位置和方向
    vec3 position;
    vec3 direction;
    float cutOff;//还需要一个切光角限定聚光灯的范围
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);

    //计算与聚光灯方向的夹角的余弦值
    float theta = dot(lightDir, normalize(-light.direction));
    //当片元在聚光灯内时才执行原本的光照计算
    if(theta > light.cutOff) {
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }else{//否则，使用环境光，让场景在聚光之外时不至于完全黑暗
        FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);
    }  
    
}