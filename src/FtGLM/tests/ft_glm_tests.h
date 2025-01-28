//
// Created by loumarti on 1/28/25.
//

#ifndef FT_GLM_TESTS_H
#define FT_GLM_TESTS_H



#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

// only in this Test file to compare with ft_glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FtGLM/ft_glm.h"

#define TOL 0.0001f

struct UnitTest {
    bool (*testFunc)();
    std::string name;
};

auto launchTests() -> void;
auto test_ft_glm_01() -> bool;
auto test_ft_glm_02() -> bool;



#endif //FT_GLM_TESTS_H
