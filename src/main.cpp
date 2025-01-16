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
        -1.0f,-1.0f,-1.0f,
       -1.0f,-1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
       -1.0f,-1.0f,-1.0f,
       -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
       -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
       -1.0f,-1.0f,-1.0f,
       -1.0f,-1.0f,-1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
       -1.0f,-1.0f, 1.0f,
       -1.0f,-1.0f,-1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
       -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f,-1.0f,
       -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
      };

    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f,
    };


    VertexBuffer vertexBuffer = VertexBuffer(g_vertex_buffer_data, 3 * 3 * 12 * sizeof(GLfloat));
    vertexBuffer.bind();

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
    glFrontFace(GL_CCW);


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


        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // ----------


        glfwSwapBuffers(engine.getWindow().getGLFWHandle());
    }




    return 0;
}
