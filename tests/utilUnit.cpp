//
// Created by remix on 23-5-29.
//
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "serializer/util.h"

TEST_CASE("tuple", "[getTupleElementCount]"){
    std::tuple<std::string,int,float> tplOne { "remix", 25, 173.5};
    std::tuple<std::string,double> tplTwo { "mix",  199.5};

    auto tplOneSize = muse::serializer::getTupleElementCount(tplOne);
    auto tplTwoSize = muse::serializer::getTupleElementCount<std::string, double>(tplTwo);

    REQUIRE(tplOneSize == 3);
    REQUIRE(tplTwoSize == 2);
}