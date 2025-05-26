#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "CustomShader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

//从文件中加载纹理
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false){
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

class Model{
public:
    //即便同样的纹理已经被加载过很多遍了，对每个网格仍会加载并生成一个新的纹理，因此将所有加载过的纹理全局储存
    //每当想加载一个纹理的时候，首先去检查它有没有被加载过。如果有的话，我们会直接使用那个纹理，并跳过整个加载流程
    vector<Texture> textures_loaded;
    bool gammaCorrection;

    Model(char *path, bool gamma = false) : gammaCorrection(gamma){
        loadModel(path);
    }
    //遍历网格并绘制
    void Draw(CustomShader shader){
        for(unsigned int i = 0; i < meshes.size(); i++){
            meshes[i].Draw(shader);
        }
    }

private:
    vector<Mesh> meshes;//网格
    string directory;

    //加载模型
    void loadModel(string path){
        //读取文件
        Assimp::Importer importer;
        //第二个参数是一些后期处理(Post-processing)的选项
        //aiProcess_Triangulate表示如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形
        //aiProcess_FlipUVs将在处理的时候翻转y轴的纹理坐标，因为在OpenGL中大部分的图像的y轴都是反的
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        //检查场景和其根节点不为null，并且检查标记(Flag)来查看返回的数据是不是不完整的
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        //递归处理子节点
        processNode(scene->mRootNode, scene);
    }

    //递归处理子节点
    void processNode(aiNode *node, const aiScene *scene){
        //处理节点所有的网格，每个节点包含了一系列的网格索引，每个索引指向场景对象中的那个特定网格
        for(unsigned int i = 0; i < node->mNumMeshes; i++){
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];//获取网格
            meshes.push_back(processMesh(mesh, scene));//处理网格，存入meshes
        }
        //递归处理子节点
        for(unsigned int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene);
        }
    }

    //处理网格，访问网格的相关属性并将它们储存到我们自己的对象中
    Mesh processMesh(aiMesh *mesh, const aiScene *scene){
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        //处理顶点位置、法线和纹理坐标，使用所有的相关数据填充Mesh中的结构体
        for(unsigned int i = 0; i < mesh->mNumVertices; i++){
            Vertex vertex;
            glm::vec3 vector;
            // 位置
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // 法线
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // 纹理坐标
            // Assimp允许一个模型在一个顶点上有最多8个不同的纹理坐标
            // 不会用到那么多，只关心第一组纹理坐标
            if(mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // 切线
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }else{
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        //处理索引
        //Assimp的接口定义了每个网格都有一个面(Face)数组，每个面代表了一个图元，由于使用了aiProcess_Triangulate选项，它总是三角形
        //一个面包含了多个索引，它们定义了在每个图元中，我们应该绘制哪个顶点，并以什么顺序绘制。
        //所以如果我们遍历了所有的面，并储存了面的索引到indices这个vector中就可以了
        for(unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++){
                indices.push_back(face.mIndices[j]);
            }
        }

        //处理材质
        //一个网格只包含了一个指向材质对象的索引
        //如果想要获取网格真正的材质，需要索引场景的mMaterials数组
        if(mesh->mMaterialIndex >= 0){
            //从场景的mMaterials数组中获取aiMaterial对象
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            //加载网格的漫反射贴图
            //不同的纹理类型都以aiTextureType_为前缀
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            //加载网格的镜面光贴图
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            //加载网格的法线贴图
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            //加载网格的高度贴图
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }
        return Mesh(vertices, indices, textures);
    }

    //从材质中获取纹理
    //一个材质对象的内部对每种纹理类型都存储了一个纹理位置数组
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName){
        vector<Texture> textures;
        //遍历给定纹理类型的所有纹理位置
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
            //获取了纹理的文件位置
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                //如果纹理已经被加载过，就跳过它，直接使用之前加载过的纹理
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }
            //如果纹理还没有被加载过，就加载它
            if(!skip){
                //加载并生成纹理
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // 同时添加到已加载的纹理中
            }
            
        }
        return textures;
    }
};

#endif