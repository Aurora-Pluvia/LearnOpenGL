#include <glad/glad.h>//glad用于管理opengl的函数指针
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

//窗口的回调函数，在每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    //告诉OpenGL渲染窗口的尺寸大小，这样OpenGL才只能知道怎样根据窗口大小显示数据和坐标
    //前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    glViewport(0, 0, 800, 600);
}

//检测按键是否被按下
void processInput(GLFWwindow* window){
    //按下esc键后关闭窗口
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

int main(){
    //初始化glfw
    glfwInit();
    //设置glfw主要和次要版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //设置为核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建窗口对象
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGl", NULL, NULL);
    if(window == nullptr){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    //通知GLFW将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);

    //给GLAD传入用来加载系统相关的OpenGL函数指针地址的函数
    //GLFW的glfwGetProcAddress根据编译的系统定义了正确的函数
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    //告诉OpenGL渲染窗口的尺寸大小，这样OpenGL才只能知道怎样根据窗口大小显示数据和坐标
    //前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    //glViewport(0, 0, 800, 600);

    //当用户改变窗口的大小的时候，视口也应该被调整，所以需要对窗口注册一个回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    //程序需要在我们主动关闭它之前不断绘制图像并能够接受用户输入
    //渲染循环
    //glfwWindowShouldClose函数在每次循环的开始前检查一次GLFW是否被要求退出
    while (!glfwWindowShouldClose(window)){
        //输入检测
        processInput(window);

        //所有的渲染操作都要放到渲染循环中，让渲染指令在每次渲染循环迭代的时候都能被执行
        //设置清空屏幕所用的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //在每个新的渲染迭代开始的时候我们总是希望清屏，否则我们仍能看见上一次迭代的渲染结果
        glClear(GL_COLOR_BUFFER_BIT);//清空颜色缓冲

        //检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
        glfwSwapBuffers(window);

        //交换颜色缓冲（一个储存着GLFW窗口每一个像素颜色值的大缓冲），用来在每一次迭代中进行绘制，并且输出在屏幕上
        glfwPollEvents();
        /*双缓冲(Double Buffer)
        应用程序使用单缓冲绘图时可能会存在图像闪烁的问题。 这是因为生成的图像不是一下子被绘制出来的，而是按照从左到右，由上而下逐像素地绘制而成的。
        最终图像不是在瞬间显示给用户，而是通过一步一步生成的，这会导致渲染的结果很不真实。为了规避这些问题，我们应用双缓冲渲染窗口应用程序。
        前缓冲保存着最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在后缓冲上绘制。
        当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。*/
    }
    
    //渲染循环结束后需要释放资源
    glfwTerminate();

    return 0;
}