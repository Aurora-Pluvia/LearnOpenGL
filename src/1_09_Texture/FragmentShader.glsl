#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    //FragColor = texture(ourTexture, TexCoord);
    //把得到的纹理颜色与顶点颜色混合
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
}