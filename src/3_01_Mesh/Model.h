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

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model{
public:
    Model(char *path){
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
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
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

        //处理材质
        if(mesh->mMaterialIndex >= 0){

        }
        return Mesh(vertices, indices, textures);
    }
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif