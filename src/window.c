#ifndef WINDOW
#define WINDOW

#include <stdio.h>

#include "include/glad/gl.h"
#include "GLFW/glfw3.h"

#define WINDOW_NAME "Chess"

GLFWwindow* window;

int init(int width, int height) {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

    if (!glfwInit()){
        printf("GLFW didn't initialise.\n");
        return -1;
    }

    // Window Hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, WINDOW_NAME, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Disables v-sync

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialise OpenGL context\n");
        return -1;
    }

    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);

    return 0;
}

int main() {
    if (init(800, 600) < 0) { return 1; }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
}

#endif
