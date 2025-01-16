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
#include "OpenGL/IndicesBuffer.h"


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

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f,  1.0f,  // 0 : Front-bottom-left
         1.0f, -1.0f,  1.0f,  // 1 : Front-bottom-right
         1.0f,  1.0f,  1.0f,  // 2 : Front-top-right
        -1.0f,  1.0f,  1.0f,  // 3 : Front-top-left
        -1.0f, -1.0f, -1.0f,  // 4 : Back-bottom-left
         1.0f, -1.0f, -1.0f,  // 5 : Back-bottom-right
         1.0f,  1.0f, -1.0f,  // 6 : Back-top-right
        -1.0f,  1.0f, -1.0f,  // 7 : Back-top-left
    };


    static const GLuint g_indices_buffer_data[] = {
        // Triangle1 // Triangle2
        0, 1, 2,    2, 3, 0, // Front
        6, 5, 4,    4, 7, 6, // Back
        4, 0, 3,    3, 7, 4, // Left
        1, 5, 6,    6, 2, 1, // Right
        3, 2, 6,    6, 7, 3, // Up
        4, 5, 1,    1, 0, 4, // Down
    };


    static const GLfloat g_color_buffer_data[] = {
        1.0f, 0.0f, 0.0f,  // Rouge pour sommet 0
        0.0f, 1.0f, 0.0f,  // Vert pour sommet 1
        0.0f, 0.0f, 1.0f,  // Bleu pour sommet 2
        1.0f, 1.0f, 0.0f,  // Jaune pour sommet 3
        1.0f, 0.0f, 1.0f,  // Magenta pour sommet 4
        0.0f, 1.0f, 1.0f,  // Cyan pour sommet 5
        1.0f, 1.0f, 1.0f,  // Blanc pour sommet 6
        0.0f, 0.0f, 0.0f   // Noir pour sommet 7
    };


    VertexBuffer vertexBuffer = VertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    vertexBuffer.bind();

    // [3] Set Indices buffer (bind to OpenGL in init)
    IndicesBuffer indicesBuffer = IndicesBuffer(g_indices_buffer_data, sizeof(g_indices_buffer_data));

    VertexBuffer colorBuffer = VertexBuffer(g_color_buffer_data, sizeof(g_color_buffer_data));
    colorBuffer.bind();

    Shader shader = Shader("./res/shaders/current_shader.glsl");
    shader.bind();

    // [4] Setting uniforms in glsl shader
    shader.setUniformMat4f("u_mvp", camera.computeMVP());


    shader.unbind();
    vertexArray.unbind();
    vertexBuffer.unbind();
    colorBuffer.unbind();


    glfwSetInputMode(engine.getWindow().getGLFWHandle(), GLFW_STICKY_KEYS, GL_TRUE);
    /* gl display Settings */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);



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



        vertexArray.addBuffer(vertexBuffer, 0, 3);
        vertexArray.addBuffer(colorBuffer, 1, 3);



        // glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // ----------


        glfwSwapBuffers(engine.getWindow().getGLFWHandle());
    }




    return 0;
}
