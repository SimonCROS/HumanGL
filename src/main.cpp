#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"


int main()
{
    auto e_context = WindowContext::Create(4, 1);
    if (!e_context)
    {
        std::cout << "Error: " << e_context.error() << std::endl;
        return EXIT_FAILURE;
    }

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
    {
        std::cout << "Error: " << e_window.error() << std::endl;
        return EXIT_FAILURE;
    }

    Engine engine = Engine(*std::move(e_window)); // TODO change

    engine.getWindow().setCurrentContext();
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
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

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
      };

    VertexBuffer vertexBuffer = VertexBuffer(g_vertex_buffer_data, 3 * 3 * sizeof(GLfloat));
    vertexBuffer.bind();

    Shader shader = Shader("./res/shaders/current_shader.glsl");
    shader.bind();

    // [4] Setting uniforms in glsl shader
    shader.setUniformMat4f("u_mvp", camera.computeMVP());


    shader.unbind();
    vertexArray.unbind();
    vertexBuffer.unbind();

    // ---------------

    while (!glfwWindowShouldClose(engine.getWindow().getGLFWHandle()))
    {
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

        shader.setUniformMat4f("u_mvp", camera.computeMVP());

        vertexArray.bind();
        vertexBuffer.bind();


        vertexArray.addBuffer(vertexBuffer, 0, 3);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        // ----------


        glfwSwapBuffers(engine.getWindow().getGLFWHandle());
    }




    return 0;
}
