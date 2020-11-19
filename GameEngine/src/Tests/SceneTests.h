#include <gtest/gtest.h>
#include "../Scene.h"

// class SceneTests {
//     public:
//         static void runTests();

//     private:
//         TEST (SquareRootTest, ZeroAndNegativeNos) { 
//             ASSERT_EQ (0.0, 0.0);
//         }
// };

TEST (AddAudioSource, ValidArguments) {
    EXPECT_EQ(0, 1);
    EXPECT_EQ(0, 0);
}

TEST (AddAudioSource, InvalidArguments) {
    EXPECT_EQ(0, 0);
}

TEST (AddAudioSource, MissingArguments) {
    EXPECT_EQ(0, 0);
}

TEST (AddBoxCollider, ValidArguments) {
    EXPECT_EQ(0, 0);
}

TEST (AddBoxCollider, InvalidArguments) {
    EXPECT_EQ(0, 0);
}

TEST (AddBoxCollider, MissingArguments) {
    EXPECT_EQ(0, 0);
}
