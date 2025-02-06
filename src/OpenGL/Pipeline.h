//
// Created by Simon Cros on 2/6/25.
//

#ifndef PIPELINE_H
#define PIPELINE_H
#include "ShaderProgram.h"

class Pipeline
{
public:
    [[nodiscard]] static auto Create(const ShaderProgramInstance& vertProgram, const ShaderProgramInstance& fragProgram)
        -> Pipeline
    {
        GLuint id;
        glGenProgramPipelines(1, &id);

        if (std::addressof(vertProgram) == std::addressof(fragProgram))
        {
            glUseProgramStages(id, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, vertProgram.id());
        }
        else
        {
            glUseProgramStages(id, GL_VERTEX_SHADER_BIT, vertProgram.id());
            glUseProgramStages(id, GL_FRAGMENT_SHADER_BIT, fragProgram.id());
        }


    }
};

#endif //PIPELINE_H
