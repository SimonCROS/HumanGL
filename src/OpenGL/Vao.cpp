//
// Created by Simon Cros on 2/6/25.
//

#include "Vao.h"

auto Vao::Create(VaoFlags flags) -> Vao
{
    GLuint id;

    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    if (flags & VaoHasPosition)
        glEnableVertexAttribArray(0);
    if (flags & VaoHasNormal)
        glEnableVertexAttribArray(1);
    if (flags & VaoHasColor0)
        glEnableVertexAttribArray(2);
    if (flags & VaoHasTexCoord0)
        glEnableVertexAttribArray(3);

    return {flags, id};
}
