//
// Created by ByteDance on 2022/8/11.
//

//#include "compress_img.h"
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <texture2d.h>

GLFWwindow *window;

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void init_opengl() {

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(960, 640, "Compress Image", NULL, NULL);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}



int main() {
    init_opengl();

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f/255,77.f/255,121.f/255,1.f);

        //
        std::string src_img_file_path("../../data/images/rem.jpg");
        std::string cpt_file_path("../../data/images/rem.cpt");
        Texture2D::CompressImageFile(src_img_file_path, cpt_file_path);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
