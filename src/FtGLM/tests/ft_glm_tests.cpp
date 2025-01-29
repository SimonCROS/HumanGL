//
// Created by loumarti on 1/28/25.
//

#include "ft_glm_tests.h"

#include <glm/detail/type_quat.hpp>
#include <glm/gtx/string_cast.hpp>

auto launchTests() -> void {
    std::vector<UnitTest> unitTests = {
        {test_ft_glm_01, "vec2 basic maths"},
        {test_ft_glm_02, "vec3 basic maths"},
        {test_ft_glm_03, "vec4 basic maths"},
        {test_ft_glm_04, "mat4 basic maths"},
        {test_ft_glm_05, "mat4 subscripts"},
        {test_ft_glm_06, "mat4 3d funcs"},
        {test_ft_glm_07, "quat basic maths"},
        {test_ft_glm_08, "quat + slerp interpolation"},
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

auto compare_mat4(const glm::mat4& m, const ft_glm::mat4& ft_m) -> bool {
    for (int i = 0; i < 4; ++i) {
        if (!compare_vec4(m[i],ft_m.columns[i])) {
            return false;
        }
    }
    return true;
}

auto compare_vec4(const glm::vec4& v, const ft_glm::vec4& ft_v) -> bool {
    for (int i = 0; i < 4; ++i) {
        if (v[i] != ft_v[i]) {
            return false;
        }
    }
    return true;
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

// Vec4 tests
auto test_ft_glm_03() -> bool {
    ft_glm::vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    ft_glm::vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    ft_glm::vec4 ft_v1(1.0f, 2.0f, 3.0f, 4.0f);
    ft_glm::vec4 ft_v2(5.0f, 6.0f, 7.0f, 8.0f);

    // Test Multiplication (*)
    ft_glm::vec4 prod = v1 * v2;
    ft_glm::vec4 ft_prod = ft_v1 * ft_v2;
    assert(std::abs(prod.x - ft_prod.x) < TOL);
    assert(std::abs(prod.y - ft_prod.y) < TOL);
    assert(std::abs(prod.z - ft_prod.z) < TOL);
    assert(std::abs(prod.w - ft_prod.w) < TOL);

    // Test equality (==)
    assert(v1 == ft_v1);
    assert(v2 == ft_v2);

    // Test assignation (=)
    ft_glm::vec4 v3 = ft_v1;
    assert(v3 == ft_v1);

    return true;
}

// mat4 basic tests
auto test_ft_glm_04() -> bool {
    ft_glm::mat4 ft_mat1(
        ft_glm::vec4(1.0f, 2.0f, 3.0f, 4.0f),
        ft_glm::vec4(5.0f, 6.0f, 7.0f, 8.0f),
        ft_glm::vec4(9.0f, 10.0f, 11.0f, 12.0f),
        ft_glm::vec4(13.0f, 14.0f, 15.0f, 16.0f)
    );

    glm::mat4 mat1(
        glm::vec4(1.0f, 2.0f, 3.0f, 4.0f),
        glm::vec4(5.0f, 6.0f, 7.0f, 8.0f),
        glm::vec4(9.0f, 10.0f, 11.0f, 12.0f),
        glm::vec4(13.0f, 14.0f, 15.0f, 16.0f)
    );

    auto test = mat1[0][1];


    ft_glm::mat4 ft_mat2(
        ft_glm::vec4(16.0f, 15.0f, 14.0f, 13.0f),
        ft_glm::vec4(12.0f, 11.0f, 10.0f, 9.0f),
        ft_glm::vec4(8.0f, 7.0f, 6.0f, 5.0f),
        ft_glm::vec4(4.0f, 3.0f, 2.0f, 1.0f)
    );

    glm::mat4 mat2(
        glm::vec4(16.0f, 15.0f, 14.0f, 13.0f),
        glm::vec4(12.0f, 11.0f, 10.0f, 9.0f),
        glm::vec4(8.0f, 7.0f, 6.0f, 5.0f),
        glm::vec4(4.0f, 3.0f, 2.0f, 1.0f)
    );

    auto ft_identity = ft_glm::mat4(1.0f);
    auto identity = glm::mat4(1.0f);

    assert((mat1 * identity) == mat1);
    assert((mat2 * identity) == mat2);
    assert(mat1 != mat2);
    assert((ft_mat1 * ft_identity) == ft_mat1);
    assert((ft_mat2 * ft_identity) == ft_mat2);
    assert(ft_mat1 != ft_mat2);

    assert(compare_mat4(mat1 * identity, ft_mat1 * ft_identity));
    assert(compare_mat4(mat2 * identity, ft_mat2 * ft_identity));
    assert(compare_mat4(mat1 * mat1, ft_mat1 * ft_mat1));
    assert(compare_mat4(mat2 * mat2, ft_mat2 * ft_mat2));
    assert(compare_mat4(mat1 * mat2, ft_mat1 * ft_mat2));
    assert(compare_mat4(mat2 * mat1, ft_mat2 * ft_mat1));

    return true;
}

// mat4 subscripts
auto test_ft_glm_05() -> bool
{
    ft_glm::mat4 ft_mat(
        ft_glm::vec4(1.0f, 2.0f, 3.0f, 4.0f),
        ft_glm::vec4(5.0f, 6.0f, 7.0f, 8.0f),
        ft_glm::vec4(9.0f, 10.0f, 11.0f, 12.0f),
        ft_glm::vec4(13.0f, 14.0f, 15.0f, 16.0f)
    );

    glm::mat4 glm_mat(
        glm::vec4(1.0f, 2.0f, 3.0f, 4.0f),
        glm::vec4(5.0f, 6.0f, 7.0f, 8.0f),
        glm::vec4(9.0f, 10.0f, 11.0f, 12.0f),
        glm::vec4(13.0f, 14.0f, 15.0f, 16.0f)
    );

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (ft_mat[row][col] != glm_mat[row][col]) {
                std::cerr << "Mismatch at (" << row << ", " << col << "): "
                          << "ft_mat = " << ft_mat[row][col] << ", "
                          << "glm_mat = " << glm_mat[row][col] << std::endl;
                return false;
            }
        }
    }

    ft_mat[0][0] = 42.0f;
    glm_mat[0][0] = 42.0f;

    ft_mat[3][2] = -7.5f;
    glm_mat[3][2] = -7.5f;

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (ft_mat[row][col] != glm_mat[row][col]) {
                std::cerr << "Mismatch after modification at (" << row << ", " << col << "): "
                          << "ft_mat = " << ft_mat[row][col] << ", "
                          << "glm_mat = " << glm_mat[row][col] << std::endl;
                return false;
            }
        }
    }

    return true;
}

// translate, perspective, lookAt
auto test_ft_glm_06() -> bool
{
    ft_glm::vec3 ft_center = ft_glm::vec3(3.0f, 5.5f, 0.7f);
    glm::vec3 center = glm::vec3(3.0f, 5.5f, 0.7f);

    ft_glm::mat4 ft_translateTestMatrix = ft_glm::translate(ft_glm::mat4(1.0f), ft_glm::vec3(-ft_center.x, -ft_center.y, -ft_center.z));
    glm::mat4 translateTestMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-center.x, -center.y, -center.z));

    assert(compare_mat4(translateTestMatrix, ft_translateTestMatrix));

    float fovDegrees = 45.0f;
    float ratio = 4.0f / 3.0f;
    float near = 0.1f;
    float far = 100.0f;

    float ft_fovRadians = ft_glm::radians(fovDegrees);
    float fovRadians = glm::radians(fovDegrees);

    if (ft_fovRadians != fovRadians) {
        std::cout << "ft_fovRadians = " << ft_fovRadians << std::endl;
        std::cout << "fovRadians = " << fovRadians << std::endl;
        return false;
    }

    glm::mat4 projMatrix = glm::perspective(fovRadians, ratio, near, far);
    ft_glm::mat4 ft_projMatrix = ft_glm::perspective(ft_fovRadians, ratio, near, far);


    assert(compare_mat4(projMatrix, ft_projMatrix));

    ft_glm::mat4 ft_identityMatrix = ft_glm::mat4(1.0f);;
    glm::mat4 identityMatrix = glm::mat4(1.0f);;

    assert(compare_mat4(identityMatrix, ft_identityMatrix));


    ft_glm::mat4 ft_emptyMatrix = ft_glm::mat4(0.0f);
    glm::mat4 emptyMatrix = glm::mat4(0.0f);

    assert(compare_mat4(emptyMatrix, ft_emptyMatrix));

    ft_glm::vec3 ft_position(0.0f, 0.0f, 0.0f);
    ft_glm::vec3 ft_target(0.0f, 0.0f, -1.0f);
    ft_glm::vec3 ft_up(0.0f, 1.0f, 0.0f);
    ft_glm::mat4 ft_resultLookAt = ft_glm::lookAt(ft_position, ft_target, ft_up);

    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::vec3 target(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 resultLookAt = glm::lookAt(position, target, up);

    assert(compare_mat4(resultLookAt, ft_resultLookAt));

    return true;
}

auto test_ft_glm_07() -> bool
{
    ft_glm::quat ft_q1(1.0f, 0.0f, 0.0f, 0.0f);
    ft_glm::quat ft_q2(0.0f, 1.0f, 0.0f, 0.0f);
    glm::quat glm_q1(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat glm_q2(0.0f, 1.0f, 0.0f, 0.0f);

    // Compare attributes
    {
        assert(glm_q1.x == ft_q1.x);
        assert(glm_q1.y == ft_q1.y);
        assert(glm_q1.z == ft_q1.z);
        assert(glm_q1.w == ft_q1.w);
    }

    // Addition
    {
        ft_glm::quat ft_add_result = ft_q1 + ft_q2;
        glm::quat glm_add_result = glm_q1 + glm_q2;
        assert(std::abs(ft_add_result.w - glm_add_result.w) < TOL);
        assert(std::abs(ft_add_result.x - glm_add_result.x) < TOL);
        assert(std::abs(ft_add_result.y - glm_add_result.y) < TOL);
        assert(std::abs(ft_add_result.z - glm_add_result.z) < TOL);
    }

    // Multiplication (q1 * scalar)
    float scalar = 2.0f;
    {
        ft_glm::quat ft_scalar_result = ft_q1 * scalar;
        glm::quat glm_scalar_result = glm_q1 * scalar;

        assert(std::abs(ft_scalar_result.w - glm_scalar_result.w) < TOL);
        assert(std::abs(ft_scalar_result.x - glm_scalar_result.x) < TOL);
        assert(std::abs(ft_scalar_result.y - glm_scalar_result.y) < TOL);
        assert(std::abs(ft_scalar_result.z - glm_scalar_result.z) < TOL);
    }

    // Multiplication (q1 * q2)
    {
        ft_glm::quat ft_mul_result = ft_q1 * ft_q2;
        glm::quat glm_mul_result = glm_q1 * glm_q2;

        assert(std::abs(ft_mul_result.w - glm_mul_result.w) < TOL);
        assert(std::abs(ft_mul_result.x - glm_mul_result.x) < TOL);
        assert(std::abs(ft_mul_result.y - glm_mul_result.y) < TOL);
        assert(std::abs(ft_mul_result.z - glm_mul_result.z) < TOL);
    }

    // Division (q1 / scalar)
    {
        ft_glm::quat ft_div_result = ft_q1 / scalar;
        glm::quat glm_div_result = glm_q1 / scalar;

        assert(std::abs(ft_div_result.w - glm_div_result.w) < TOL);
        assert(std::abs(ft_div_result.x - glm_div_result.x) < TOL);
        assert(std::abs(ft_div_result.y - glm_div_result.y) < TOL);
        assert(std::abs(ft_div_result.z - glm_div_result.z) < TOL);
    }

    // Normalisation
    {
        ft_glm::quat ft_normalized = normalize(ft_q1);
        glm::quat glm_normalized = glm::normalize(glm_q1);

        assert(std::abs(ft_normalized.w - glm_normalized.w) < TOL);
        assert(std::abs(ft_normalized.x - glm_normalized.x) < TOL);
        assert(std::abs(ft_normalized.y - glm_normalized.y) < TOL);
        assert(std::abs(ft_normalized.z - glm_normalized.z) < TOL);
    }

    return true;
}

// quaternion + slerp
auto test_ft_glm_08() -> bool
{
    ft_glm::quat ft_q1(1.0f, 0.0f, 0.0f, 0.0f);
    ft_glm::quat ft_q2(0.0f, 1.0f, 0.0f, 0.0f);

    glm::quat glm_q1(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat glm_q2(0.0f, 1.0f, 0.0f, 0.0f);

    for (float t = 0.0f; t <= 1.0f; t += 0.1f) {
        ft_glm::quat ft_result = ft_glm::slerp(ft_q1, ft_q2, t);
        glm::quat glm_result = glm::slerp(glm_q1, glm_q2, t);

        assert(std::abs(ft_result.w - glm_result.w) < TOL);
        assert(std::abs(ft_result.x - glm_result.x) < TOL);
        assert(std::abs(ft_result.y - glm_result.y) < TOL);
        assert(std::abs(ft_result.z - glm_result.z) < TOL);
    }

    return true;
}
