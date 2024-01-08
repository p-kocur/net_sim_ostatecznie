//
// Created by pawel on 20.12.2023.
//
#include "gtest/gtest.h"

#include "package.hpp"
#include "types.hpp"

TEST(test1, przydzielanieID) {
    Package x1;
    Package x2;
    Package* x7 = new Package(7);

    EXPECT_EQ(1, x1.get_id());
    EXPECT_EQ(2, x2.get_id());
    EXPECT_EQ(7, x7->get_id());
    delete x7;

    Package x_7;
    EXPECT_EQ(7, x_7.get_id());
}