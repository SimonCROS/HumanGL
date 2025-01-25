#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "UserInterface.h"
#include "Window.h"
#include "WindowContext.h"
#include "Engine/Animation.h"
#include "glm/gtx/string_cast.hpp"
#include "Engine/AnimationSampler.h"
#include "MicroGLTF/Struct.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "OpenGL/VertexArray.h"
#include "Scripts/CameraController.h"
#include "Golem.microgltf.h"


GLuint whiteTexture = 0;

void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

void setDoubleSided(bool value)
{
    // if (state.doubleSided != value)
    // {
    //     state.doubleSided = value;
        if (value)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }
    // }
}

auto bindTexture(const std::unordered_map<int, GLuint>& textures, const int textureIndex,
                 ShaderProgram& program, const std::string_view& bindingKey, const GLint bindingValue) -> void
{
    // assert(bindingValue < CUSTOM_MAX_BINDED_TEXTURES);

    const auto it = textures.find(textureIndex);
    if (it != textures.end())
    {
        GLuint glTexture = it->second;

        // SetInt on program before, if the shader has changed
        program.setInt(bindingKey.data(), bindingValue);

        // if (state.bindedTextures[bindingValue] == glTexture)
        // return;

        glActiveTexture(GL_TEXTURE0 + bindingValue);
        glBindTexture(GL_TEXTURE_2D, glTexture > 0 ? glTexture : whiteTexture);
        // state.bindedTextures[bindingValue] = glTexture;
    }
}

auto renderMesh(const microgltf::Model& model, const int meshIndex, VertexArray& vao, ShaderProgram& program,
                const std::unordered_map<int, GLuint>& buffers, const std::unordered_map<int, GLuint>& textures, const glm::mat4& transform) -> void
{
    const microgltf::Mesh& mesh = model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives)
    {
        for (const auto& [attribute, accessorIndex] : primitive.attributes)
        {
            const microgltf::Accessor& accessor = model.accessors[accessorIndex];

            const int attributeLocation = program.getAttributeLocation(attribute);
            if (attributeLocation != -1)
            {
                vao.bindArrayBuffer(buffers.at(accessor.bufferView));

                const microgltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const int componentSize = microgltf::getComponentSizeInBytes(accessor.componentType);
                const int componentCount = microgltf::getNumComponentsInType(accessor.type);

                GLsizei byteStride;
                if (bufferView.byteStride > 0)
                    byteStride = static_cast<GLsizei>(bufferView.byteStride);
                else
                    byteStride = componentSize * componentCount;

                glVertexAttribPointer(attributeLocation, componentCount, accessor.componentType, GL_FALSE, byteStride,
                                      bufferOffset(accessor.byteOffset));
                // CheckErrors("vertex attrib pointer");

                program.enableAttribute(attributeLocation);
                // CheckErrors("enable vertex attrib array");
            }
        }

        program.applyAttributeChanges();
        program.setMat4("u_transform", transform);

        if (primitive.material >= 0)
        {
            const auto &material = model.materials[primitive.material];

            setDoubleSided(material.doubleSided);

            bindTexture(textures, material.pbrMetallicRoughness.baseColorTexture.index, program, "u_baseColorTexture", 0);
            program.setVec4("u_baseColorFactor", material.pbrMetallicRoughness.baseColorFactor);

            if (material.normalTexture.index >= 0)
            {
                bindTexture(textures, material.normalTexture.index, program, "u_normalMap", 2);
                program.setFloat("u_normalScale", static_cast<float>(material.normalTexture.scale));
            }
        }
        else
        {
            setDoubleSided(false);
        }

        const microgltf::Accessor& indexAccessor = model.accessors[primitive.indices];

        vao.bindElementArrayBuffer(buffers.at(indexAccessor.bufferView));

        glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count), indexAccessor.componentType,
                       bufferOffset(indexAccessor.byteOffset));
        // CheckErrors("draw elements");
    }
}

auto renderNode(const microgltf::Model& model, const int nodeIndex, VertexArray& vao, ShaderProgram& program,
                const std::unordered_map<int, GLuint>& buffers, const std::unordered_map<int, GLuint>& textures, const Animation& animation, glm::mat4 transform) -> void
{
    const microgltf::Node& node = model.nodes[nodeIndex];

    if (node.matrix.has_value())
    {
        transform *= *node.matrix;
    }
    else
    {
        const Animation::AnimatedNode an = animation.animatedNode(nodeIndex);

        if (an.translationSampler >= 0)
            transform = glm::translate(transform, animation.sampler(an.translationSampler).vec3());
        else if (node.translation.has_value())
            transform = glm::translate(transform, *node.translation);

        if (an.rotationSampler >= 0)
            transform *= glm::mat4_cast(animation.sampler(an.rotationSampler).quat());
        else if (node.rotation.has_value())
            transform *= glm::mat4_cast(*node.rotation);

        if (an.scaleSampler >= 0)
            transform = glm::scale(transform, animation.sampler(an.scaleSampler).vec3());
        else if (node.scale.has_value())
            transform = glm::scale(transform, *node.scale);
    }

    if (node.mesh > -1)
        renderMesh(model, node.mesh, vao, program, buffers, textures, transform);
    for (const auto childIndex : node.children)
        renderNode(model, childIndex, vao, program, buffers, textures, animation, transform);
}

#include <fstream>

auto readFileToVector(const std::string& filename, const std::streamsize fileSize) -> std::vector<uint8_t>
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    if (!file)
    {
        throw std::runtime_error("Failed to read file");
    }

    return buffer;
}

auto createWhiteTexture() -> GLuint
{
    constexpr int data[4] = {~0, ~0, ~0, ~0};
    GLuint whiteTextureId;
    glGenTextures(1, &whiteTextureId);
    glBindTexture(GL_TEXTURE_2D, whiteTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return whiteTextureId;
}

auto loadTexture(const microgltf::Model& model, const int& textureId, std::unordered_map<int, GLuint>& textures,
                        const GLint internalformat) -> bool
{
    if (textureId < 0)
        return false;

    if (textures.contains(textureId))
        return true;

    const auto &texture = model.textures[textureId];
    assert(texture.source >= 0);

    GLuint glTexture = 0;
    const auto &image = model.images[texture.source];

    if (!image.uri.empty())
    {
        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_2D, glTexture);

        if (texture.sampler >= 0)
        {
            const auto &sampler = model.samplers[texture.sampler];

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
            if (sampler.minFilter >= 0)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
            if (sampler.magFilter >= 0)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
        }

        int width, height, component;
        stbi_uc *data = stbi_load((RESOURCE_PATH"models/iron_golem/" + image.uri).c_str(), &width, &height, &component, 0);
        if (data != nullptr)
        {
            GLenum format = GL_RGBA;
            if (component == 1)
                format = GL_RED;
            else if (component == 2)
                format = GL_RG;
            else if (component == 3)
                format = GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "ERROR::TEXTURE::LOADING_FAILED\n" << stbi_failure_reason() << std::endl;
        }

        stbi_image_free(data);
    }

    textures[textureId] = glTexture;
    return true;
}

auto prepare(const microgltf::Model& model, VertexArray& vao, std::unordered_map<int, GLuint>& buffers,
    std::unordered_map<int, GLuint>& textures, std::vector<Animation>& animations) -> void
{
    vao.bind();

    textures.reserve(model.textures.size());
    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            if (primitive.indices < 0)
                continue;

            for (const auto& [attributeName, accessorId] : primitive.attributes)
            {
                const auto& accessor = model.accessors[accessorId];
                const auto& bufferView = model.bufferViews[accessor.bufferView];

                if (!buffers.contains(accessor.bufferView))
                {
                    const auto& buffer = model.buffers[bufferView.buffer];
                    GLuint glBuffer = 0;

                    glGenBuffers(1, &glBuffer);
                    glBindBuffer(bufferView.target, glBuffer);
                    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset,
                                 GL_STATIC_DRAW);

                    buffers[accessor.bufferView] = glBuffer;
                }
            }

            {
                const auto& accessor = model.accessors[primitive.indices];
                if (!buffers.contains(accessor.bufferView))
                {
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[bufferView.buffer];
                    GLuint glBuffer = 0;

                    glGenBuffers(1, &glBuffer);
                    glBindBuffer(bufferView.target, glBuffer);
                    glBufferData(bufferView.target, static_cast<GLsizeiptr>(bufferView.byteLength),
                                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

                    buffers[accessor.bufferView] = glBuffer;
                }
            }

            if (primitive.material >= 0)
            {
                const auto &material = model.materials[primitive.material];
                loadTexture(model, material.pbrMetallicRoughness.baseColorTexture.index, textures, GL_SRGB_ALPHA);
                loadTexture(model, material.normalTexture.index, textures, GL_RGB);
            }
        }
    }

    animations.reserve(model.animations.size());
    for (const auto& animation : model.animations)
    {
        animations.emplace_back(Animation::create(model, animation));
    }

    vao.unbind();
}

auto start() -> Expected<void, std::string>
{
    std::cout << "HumanGL " << HumanGL_VERSION_MAJOR << "." << HumanGL_VERSION_MINOR << std::endl;

    auto e_window_context = WindowContext::Create(4, 1);
    if (!e_window_context)
        return Unexpected("Failed to create window context: " + std::move(e_window_context).error());

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
        return Unexpected("Failed to create window: " + std::move(e_window).error());

    auto engine = Engine(*std::move(e_window));
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    auto camera = Camera(engine.getWindow().width(), engine.getWindow().height(), 60.0f);

    microgltf::Model model = golemMicrogltf;
    auto ui = UserInterface(engine.getWindow().getGLFWHandle());







    for (auto& buffer : model.buffers)
    {
        if (!buffer.data.empty())
            continue;

        buffer.data = readFileToVector(RESOURCE_PATH"models/iron_golem/" + buffer.uri, buffer.byteLength);
    }

    ShaderProgramVariants programVariants(RESOURCE_PATH "shaders/default.vert", RESOURCE_PATH "shaders/default.frag");
    programVariants.enableVariant(ShaderHasNone);
    ShaderProgram program = programVariants.getProgram(ShaderHasNone); // ONLY USE ONE SHADER

    auto vao = VertexArray();
    std::unordered_map<int, GLuint> buffers;
    std::unordered_map<int, GLuint> textures;
    std::vector<Animation> animations;
    prepare(model, vao, buffers, textures, animations);

    whiteTexture = createWhiteTexture();

    CameraController c({0, 3.5, 0}, 50);

    vao.bind();
    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    engine.run([&](Engine& engine)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ui.set();


        // Will be automatized
        c.update(engine.getWindow().getCurrentControls(), camera, engine.frameInfo().deltaTime.count());

        const auto pvMat = camera.projectionMatrix() * camera.computeViewMatrix();
        program.use();
        program.setMat4("u_projectionView", pvMat);

        animations[ui.selected_animation()].update(engine.frameInfo());
        for (const auto nodeIndex : model.scenes[model.scene].nodes)
            renderNode(model, nodeIndex, vao, program, buffers, textures, animations[ui.selected_animation()], glm::scale(glm::identity<glm::mat4>(), glm::vec3(10)));


        ui.render();
    });


    return {};
}

auto main() -> int
{
    auto e_result = start();
    if (!e_result)
    {
        std::cerr << "Error: " << e_result.error() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
