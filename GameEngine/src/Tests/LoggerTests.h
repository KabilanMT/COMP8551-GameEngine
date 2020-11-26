#pragma once
#include <gtest/gtest.h>

#include "../Logger.h"

using namespace std;
using namespace entityx;

TEST (OperatorDoubleArrow, String) {
    string test = "test";
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, EmptyString) {
    string test = "";
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, Integer) {
    int test = 3;
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, UnsignedInteger64) {
    uint64_t test = 2;
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, Float) {
    float test = 4;
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, Double) {
    double test = 4;
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, Char) {
    char test = 'c';
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, NullChar) {
    char test = '\0';
    EXPECT_NO_THROW(Logger::getInstance() << test);
}

TEST (OperatorDoubleArrow, Other) {
    wchar_t test = 'c';
    EXPECT_NO_THROW(Logger::getInstance() << test);
}
