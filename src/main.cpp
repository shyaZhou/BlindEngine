#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <texture2d.h>
#include <shader_source.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vertex_data.h>

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* window;
Texture2D *texture2d = nullptr;
GLuint vertex_shader, fragment_shader, program;
GLint mvp_location, vpos_location, vcol_location, u_diffuse_texture_location, a_uv_location;

void init_opengl() {
    glfwSetErrorCallback(error_callback);
    /* Init glfw */
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    /* Create Window and OpenGL context */
    window = glfwCreateWindow(960, 640, "Diao Engine", NULL, NULL);
    if (!window) {
        // exit when create failed
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /* Active OpenGL context */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}

void compile_shader() {
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);

    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);

    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
}

void CreateTexture(std::string image_file_path) {
    texture2d = Texture2D::LoadFromFile(image_file_path);
}

template<typename T>
constexpr T t_mul(T t1, T t2) {
    return t1 * t2;
}

int main(void)
{
    init_opengl();

    CreateTexture("../../data/images/rem.jpg");

    compile_shader();

    mvp_location = glGetUniformLocation(program, "u_mvp");
    vpos_location = glGetAttribLocation(program, "a_pos");
    vcol_location = glGetAttribLocation(program, "a_color");
    a_uv_location = glGetAttribLocation(program, "a_uv");
    u_diffuse_texture_location = glGetUniformLocation(program, "u_diffuse_texture");

    /* Engine main loop */
    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;
        glm::mat4 model, view, projection, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f/255,77.f/255,121.f/255,1.f);

        glm::mat4 trans = glm::translate(glm::vec3(0,0,0));

        static float rotate_eulerAngle = 0.f;
        rotate_eulerAngle += 1.f;
        glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle));

        glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
        model = trans*scale*rotation;

        view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0,0,0), glm::vec3(0, 1, 0));

        projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);

        mvp = projection * view * model;

        glUseProgram(program); {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            // Enable vertexShader attribute(a_pos) to connect with vertex coord data
            glEnableVertexAttribArray(vpos_location);
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(glm::vec3), kPositions);

            // Enable vertexShader attribute(a_color) to connect with vertex color data
            glEnableVertexAttribArray(vcol_location);
            glVertexAttribPointer(vcol_location, 3, GL_FLOAT, false, sizeof(glm::vec4), kColors);

            //Enable vertexShader attribute(a_uv) to connect with vertex uv data
            glEnableVertexAttribArray(a_uv_location);
            glVertexAttribPointer(a_uv_location, 2, GL_FLOAT, false, sizeof(glm::vec2), kUvs);

            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

            // active texture unit 0
            glActiveTexture(GL_TEXTURE0);
            // bind loaded texture handle to texture unit 0 which type is Texture2D
            glBindTexture(GL_TEXTURE0, texture2d->_gl_texture_id);
            // set Shader program read tex data from texture unit 0
            glUniform1i(u_diffuse_texture_location, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Mouse and Keyboard Event */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}