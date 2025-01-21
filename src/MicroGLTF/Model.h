//
// Created by Simon Cros on 1/20/25.
//

#ifndef MODEL_H
#define MODEL_H
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "glad/gl.h"
#include "Scripts/Transform.h"

namespace microgltf
{
    enum AccessorType
    {
        Scalar,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
    };

    constexpr int32_t GetComponentSizeInBytes(const uint32_t componentType)
    {
        switch (componentType)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return 1;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return 2;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
            return 4;
        case GL_DOUBLE:
            return 8;
        default:
            return -1;
        }
    }

    static int32_t GetNumComponentsInType(const AccessorType type)
    {
        switch (type)
        {
        case Scalar:
            return 1;
        case Vec2:
            return 2;
        case Vec3:
            return 3;
        case Vec4:
        case Mat2:
            return 4;
        case Mat3:
            return 9;
        case Mat4:
            return 16;
        default:
            return -1;
        }
    }

    struct Buffer
    {
        std::vector<GLubyte> data;
        GLint byteLength;
        std::string uri;
    };

    struct BufferView
    {
        size_t buffer;
        GLsizeiptr byteLength;
        size_t byteOffset;
        size_t byteStride;
        GLenum target;
        std::string name;
    };

    struct Accessor
    {
        size_t bufferView;
        size_t byteOffset;
        size_t componentType;
        size_t count;
        AccessorType type;
    };

    struct Primitive
    {
        std::map<std::string, size_t> attributes;
        int indices{-1};
        GLenum mode{GL_TRIANGLES};
    };

    struct Mesh
    {
        std::vector<Primitive> primitives;
        std::string name;
    };

    struct Node
    {
        std::vector<int> children;
        int mesh{-1};
        std::optional<glm::mat4> matrix;
        std::optional<glm::quat> rotation;
        std::optional<glm::vec3> scale;
        std::optional<glm::vec3> translation;
        std::string name;
    };

    struct Scene
    {
        std::vector<int> nodes;
        std::string name;
    };

    struct Model
    {
        std::vector<Accessor> accessors;
        std::vector<BufferView> bufferViews;
        std::vector<Buffer> buffers;
        std::vector<Mesh> meshes;
        std::vector<Node> nodes;
        int scene{-1};
        std::vector<Scene> scenes;
    };
}

#endif //MODEL_H
