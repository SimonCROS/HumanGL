//
// Created by loumarti on 1/28/25.
//

#include "ft_glm_tests.h"

auto launchTests() -> void {
    std::vector<UnitTest> unitTests = {
        {test_ft_glm_01, "vec2 basic maths"},
        {test_ft_glm_02, "vec3 basic maths"},

      };


    for (unsigned int i = 0; i < unitTests.size(); i++) {
        auto result = unitTests[i].testFunc();
        std::cout << "Test " << std::setfill('0') << std::setw(2) << (i + 1) << " " << unitTests[i].name;
        if (!result) {
            std::cout << " \033[1;31mKO !\033[0m" << std::endl;
            exit(-1);
        } else {
            std::cout << " \033[1;32mOK !\033[0m" << std::endl;
        }
    }

}


// Vec2 test
auto test_ft_glm_01() -> bool {

    glm::vec2 v1(3.0f, 4.0f);
    glm::vec2 v2(1.0f, 2.0f);
    ft_glm::vec2 ft_v1(3.0f, 4.0f);
    ft_glm::vec2 ft_v2(1.0f, 2.0f);

    // Test Sum (+)
    glm::vec2 sum = v1 + v2;
    ft_glm::vec2 ft_sum = ft_v1 + ft_v2;
    assert(std::abs(sum.x - ft_sum.x) < TOL);
    assert(std::abs(sum.y - ft_sum.y) < TOL);

    // Test Subtraction (-)
    glm::vec2 diff = v1 - v2;
    ft_glm::vec2 ft_diff = ft_v1 - ft_v2;
    assert(std::abs(diff.x - ft_diff.x) < TOL);
    assert(std::abs(diff.y - ft_diff.y) < TOL);

    // Test Multiplication (*)
    glm::vec2 prod = v1 * v2;
    ft_glm::vec2 ft_prod = ft_v1 * ft_v2;
    assert(std::abs(prod.x - ft_prod.x) < TOL);
    assert(std::abs(prod.y - ft_prod.y) < TOL);

    // Test Division (/)
    glm::vec2 div = v1 / v2;
    ft_glm::vec2 ft_div = ft_v1 / ft_v2;
    assert(std::abs(div.x - ft_div.x) < TOL);
    assert(std::abs(div.y - ft_div.y) < TOL);

    return true;
}

// Vec3 test
auto test_ft_glm_02() -> bool {
    glm::vec3 v1(3.0f, 4.0f, 5.0f);
    glm::vec3 v2(1.0f, 2.0f, 3.0f);
    ft_glm::vec3 ft_v1(3.0f, 4.0f, 5.0f);
    ft_glm::vec3 ft_v2(1.0f, 2.0f, 3.0f);

    // Test Addition
    glm::vec3 sum = v1 + v2;
    ft_glm::vec3 ft_sum = ft_v1 + ft_v2;
    assert(std::abs(sum.x - ft_sum.x) < TOL);
    assert(std::abs(sum.y - ft_sum.y) < TOL);
    assert(std::abs(sum.z - ft_sum.z) < TOL);

    // Test Subtraction
    glm::vec3 diff = v1 - v2;
    ft_glm::vec3 ft_diff = ft_v1 - ft_v2;
    assert(std::abs(diff.x - ft_diff.x) < TOL);
    assert(std::abs(diff.y - ft_diff.y) < TOL);
    assert(std::abs(diff.z - ft_diff.z) < TOL);

    // Test Multiplication
    glm::vec3 prod = v1 * v2;
    ft_glm::vec3 ft_prod = ft_v1 * ft_v2;
    assert(std::abs(prod.x - ft_prod.x) < TOL);
    assert(std::abs(prod.y - ft_prod.y) < TOL);
    assert(std::abs(prod.z - ft_prod.z) < TOL);

    // Test Division
    glm::vec3 div = v1 / v2;
    ft_glm::vec3 ft_div = ft_v1 / ft_v2;
    assert(std::abs(div.x - ft_div.x) < TOL);
    assert(std::abs(div.y - ft_div.y) < TOL);
    assert(std::abs(div.z - ft_div.z) < TOL);

    return true;
}