#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

//顶点着色器的源代码，需要在运行时动态编译
//版本声明，每个着色器都起始于一个版本声明
//使用in关键字，在顶点着色器中声明所有的输入顶点属性
//现在我们只关心位置数据，所以只需要一个顶点属性
//通过layout (location = 0)设定输入变量的位置值
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
//片元着色器的源代码
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, 800, 600);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGl", NULL, NULL);
    if(window == nullptr){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //动态编译shader源码
    //着色器ID
    unsigned int vertexShader, fragmentShader;
    //创建着色器，参数为需要创建的着色器类型
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //把着色器源码附加到着色器对象上
    //要编译的着色器对象为第一个参数，第二参数指定了传递的源码字符串数量，第三个参数是顶点着色器真正的源码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    //编译
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    //检测编译是否成功并返回错误信息
    int successVertex, successFragment;
    char infoLogVertex[512], infoLogFragment[512];
    //检查是否编译成功
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successVertex);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successFragment);
    if (!successVertex) {
        //获取错误信息
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLogVertex);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLogVertex << endl;
    }
    if (!successFragment) {
        //获取错误信息
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogFragment);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLogFragment << endl;
    }

    //创建着色器程序对象，着色器程序对象是多个着色器合并之后并最终链接完成的版本
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    //把之前编译的着色器附加到程序对象上，然后用glLinkProgram链接它们
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //检测是否成功并返回错误信息
    int successProgram;
    char infoLogProgram[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successProgram);
    if(!successProgram) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLogProgram);
        cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLogProgram << endl;
    }

    //链接完的着色器对象可以删除了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //激活程序对象
    //glUseProgram(shaderProgram);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    //所有OpenGL对象都有一个独一无二的ID，使用glGenBuffers函数生成一个带有缓冲ID的VBO对象
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    //顶点数组对象(Vertex Array Object, VAO)可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中
    //这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了
    //这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了，刚刚设置的所有状态都将存储在VAO中
    glGenVertexArrays(1, &VAO);//生成VAO对象

    //OpenGL允许同时绑定多个不同缓冲类型的缓冲
    //顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER，使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //绑定VAO
    glBindVertexArray(VAO);

    //调用glBufferData函数把之前定义的顶点数据复制到缓冲的内存中
    //第一个参数是目标缓冲的类型，第二个参数指定传输数据的大小(以字节为单位)，第三个参数是发送的实际数据
    /*第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
        GL_STATIC_DRAW ：数据不会或几乎不会改变。
        GL_DYNAMIC_DRAW：数据会被改变很多。
        GL_STREAM_DRAW ：数据每次绘制时都会改变。*/
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //告诉OPENGL该如何解析顶点数据
    //第一个参数为位置值，即着色器源码中的Location
    //第二个参数指定顶点属性的大小，这里是vec3，有三个值，所以是3
    //第三个参数指定数据的类型float
    //第四个参数参数定义是否希望数据被标准化，如果设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间
    //第五个参数是步长表示在连续的顶点属性组之间的间隔，可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）
    //第六个参数表示位置数据在缓冲中起始位置的偏移量
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //每个顶点属性从一个VBO管理的内存中获得它的数据，而具体是从哪个VBO获取则是通过在调用glVertexAttribPointer时绑定到GL_ARRAY_BUFFER的VBO决定的
    //由于在调用glVertexAttribPointer之前绑定的是先前定义的VBO对象，所以顶点属性0现在会链接到它的顶点数据。
    //启用位置值为0的顶点属性
    glEnableVertexAttribArray(0);

    //??????
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    while (!glfwWindowShouldClose(window)){
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //激活程序对象
        glUseProgram(shaderProgram);
        //绑定VAO
        glBindVertexArray(VAO);
        //画图
        //第一个参数是打算绘制的OpenGL图元的类型
        //第二个参数指定了顶点数组的起始索引
        //最后一个参数指定我们打算绘制多少个顶点
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //删除分配的内容
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();

    return 0;
}