#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "OpenGL/Cuboid.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndicesBuffer.h"


int main() {
    auto e_context = WindowContext::Create(4, 1);
    if (!e_context) {
        std::cout << "Error: " << e_context.error() << std::endl;
        return EXIT_FAILURE;
    }

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window) {
        std::cout << "Error: " << e_window.error() << std::endl;
        return EXIT_FAILURE;
    }

    Engine engine = Engine(*std::move(e_window)); // TODO change

    engine.getWindow().setCurrentContext();
    engine.getWindow().setKeyCallback([](const Window &window, const int key, const int action, int mode) -> void {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    const int version = gladLoadGL(glfwGetProcAddress);
    // std::cout << "HumanGL " << HumanGL_VERSION_MAJOR << "." << HumanGL_VERSION_MINOR << std::endl;
    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;


    // ----------------

    Camera camera = Camera(engine.getWindow().getGLFWHandle());

    auto vertexArray = VertexArray();
    vertexArray.bind();

    // [2] Set Cubes object instance
    Cuboid cube1 = Cuboid();
    Cuboid cube2 = Cuboid();
    cube2.scaleVertexBuffer(glm::vec3(1.0f, 2.5f, 1.8f));
    cube2.translate(glm::vec3(0.0f, -4.0f, 0.0f));

    VertexBuffer cube1VertexBuffer = VertexBuffer(cube1.getVertexBuffer(), 24 * sizeof(GLfloat));
    cube1VertexBuffer.bind();

    std::cout << "Cube 1 -- sizeofVB : " << sizeof(cube1.getVertexBuffer()) << std::endl;

    VertexBuffer cube2VertexBuffer = VertexBuffer(cube2.getVertexBuffer(), 24 * sizeof(GLfloat));
    cube2VertexBuffer.bind();

    // [3] Set Indices buffer (bind to OpenGL in init)
    IndicesBuffer indicesBuffer = IndicesBuffer(Cuboid::s_indices_buffer, sizeof(Cuboid::s_indices_buffer));

    VertexBuffer colorBuffer = VertexBuffer(Cuboid::s_color_buffer, sizeof(Cuboid::s_color_buffer));
    colorBuffer.bind();

    Shader shader = Shader("./res/shaders/current_shader.glsl");
    shader.bind();


    shader.unbind();
    vertexArray.unbind();
    cube1VertexBuffer.unbind();
    cube2VertexBuffer.unbind();
    colorBuffer.unbind();


    glfwSetInputMode(engine.getWindow().getGLFWHandle(), GLFW_STICKY_KEYS, GL_TRUE);
    /* gl display Settings */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // ---------------

    while (!glfwWindowShouldClose(engine.getWindow().getGLFWHandle())) {
        glfwPollEvents();

        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ---------
        shader.bind();

        camera.enableZoom();
        camera.rotateModelFromInputs();
        camera.autoRotate();
        camera.moveModelFromInputs();
        camera.selectRotationSpeedFromInputs();
        camera.switchAutoRotateFromInputs();
        camera.switchWireframeFromInputs();

        vertexArray.addBuffer(colorBuffer, 1, 3);

        vertexArray.addBuffer(cube2VertexBuffer, 0, 3);
        shader.setUniformMat4f("u_mvp", camera.computeMVP() * cube2.getModel());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        vertexArray.addBuffer(cube1VertexBuffer, 0, 3);
        shader.setUniformMat4f("u_mvp", camera.computeMVP() * cube1.getModel());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // ----------


        glfwSwapBuffers(engine.getWindow().getGLFWHandle());
    }

    return 0;
}
