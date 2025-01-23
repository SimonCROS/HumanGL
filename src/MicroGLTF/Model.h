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

    enum AnimationChannelTargetPath
    {
        PathNone,
        PathTranslation,
        PathRotation,
        PathScale,
        PathWeights,
    };

    enum AnimationSamplerInterpolation
    {
        Linear,
    };

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

    struct AnimationChannelTarget
    {
        int node{-1};
        AnimationChannelTargetPath path{PathNone};
    };

    struct AnimationChannel
    {
        int sampler{-1};
        AnimationChannelTarget target;
    };

    struct AnimationSampler
    {
        int input{-1};
        AnimationSamplerInterpolation interpolation{Linear};
        int output{-1};
    };

    struct Animation
    {
        std::vector<AnimationChannel> channels;
        std::vector<AnimationSampler> samplers;
        std::string name;
    };

    struct Node
    {
        std::vector<int> children;
        int mesh{-1};
        std::optional<glm::mat4> matrix;
        mutable std::optional<glm::quat> rotation; // Pas top le mutable
        mutable std::optional<glm::vec3> scale; // Pas top le mutable
        mutable std::optional<glm::vec3> translation; // Pas top le mutable
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
        std::vector<Animation> animations;
        std::vector<BufferView> bufferViews;
        std::vector<Buffer> buffers;
        std::vector<Mesh> meshes;
        std::vector<Node> nodes;
        int scene{-1};
        std::vector<Scene> scenes;
    };

    constexpr auto getComponentSizeInBytes(const uint32_t componentType) -> int32_t
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

    constexpr auto getNumComponentsInType(const AccessorType type) -> int32_t
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

    constexpr auto attributeSize(const Accessor& accessor) -> size_t
    {
        return getNumComponentsInType(accessor.type) * getComponentSizeInBytes(accessor.componentType);
    }

    constexpr auto byteStride(const BufferView& bufferView, const Accessor& accessor) -> size_t
    {
        if (bufferView.byteStride > 0)
            return bufferView.byteStride;
        else
            return attributeSize(accessor);
    }

    constexpr auto byteStride(const BufferView& bufferView, const size_t attributeSize) -> size_t
    {
        if (bufferView.byteStride > 0)
            return bufferView.byteStride;
        else
            return attributeSize;
    }
}

#endif //MODEL_H
