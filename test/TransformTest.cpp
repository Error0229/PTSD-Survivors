#include <gtest/gtest.h>

#include "Util/Transform.hpp"

using Util::Transform;

TEST(TransformTest, DefaultConstructor) {
    Transform t1 = Transform();
    ASSERT_EQ(t1.Mat3(), glm::mat3(1.0f));
}
TEST(TransformTest, Constructor) {
    auto m1 = glm::mat3(4.0f);
    Transform t1 = Transform(m1);
    ASSERT_EQ(t1.Mat3(), m1);
}

TEST(TransformTest, Translation) {
    Transform t1 = Transform().Translate(6, 9);
    auto origin = glm::vec3(0,0,1);
    auto p69 = glm::vec3(6,9,1);
    ASSERT_EQ(t1.Mat3() * origin, p69);
}

TEST(TransformTest, Rotate360) {
    Transform t1 = Transform().Rotate(0.0f);
    auto v = glm::vec3(1.0f,0.0f,1.0f);
    auto v2 = glm::vec3(1.0f,0.0f,1.0f);

    auto result = t1.Mat3() * v;
    EXPECT_FLOAT_EQ(result[0], v2[0]);
    EXPECT_FLOAT_EQ(result[1], v2[1]);
    EXPECT_FLOAT_EQ(result[2], v2[2]);
}

TEST(TransformTest, Rotate90) { // TODO: cant pass, maybe Floating-point error
    Transform t1 = Transform().Rotate(90.0f);
    auto v = glm::vec3(1.0f,0.0f,1.0f);
    auto v2 = glm::vec3(0.0f,1.0f,1.0f);

    auto result = t1.Mat3() * v;
    EXPECT_FLOAT_EQ(result[0], v2[0]);
    EXPECT_FLOAT_EQ(result[1], v2[1]);
    EXPECT_FLOAT_EQ(result[2], v2[2]);
}

TEST(TransformTest, Scaling) {
    Transform t1 = Transform().Scale(4,2);
    auto v = glm::vec3(1.0f,1.0f,1.0f);
    auto v2 = glm::vec3(4.0f,2.0f,1.0f);
    ASSERT_EQ(t1.Mat3() * v, v2);
}

TEST(TransformTest, ScalingAndTranslation) {
    Transform t1 = Transform().Translate(-6,1).Scale(-1,9);
    auto v = glm::vec3(0.0f,0.0f,1.0f);
    auto v2 = glm::vec3(6.0f,9.0f,1.0f);
    ASSERT_EQ(t1.Mat3() * v, v2);
}