//
// Created by zeliboba on 5/23/24.
//

#include <catch2/catch_test_macros.hpp>
#include "modelling/naive_model.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "utils/mtf2_storage.hpp"

TEST_CASE("MTF2 storage works with simple degree", "[MTF2Storage]") {
    MTF2Storage<int> storage(3, 1);
    storage.PushAndOverflow(1);
    storage.PushAndOverflow(2);
    storage.PushAndOverflow(3);

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{3, 1, 2});

    storage.MoveToFront(1);

    REQUIRE(storage.Elements == std::vector{1, 3, 2});

    storage.MoveToFront(2);

    REQUIRE(storage.Elements == std::vector{2, 1, 3});

    storage.MoveToFront(2);

    REQUIRE(storage.Elements == std::vector{2, 1, 3});

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{3, 2, 1});
}

TEST_CASE("MTF2 storage works with second degree", "[MTF2Storage]") {
    MTF2Storage<int> storage(3, 2);
    storage.PushAndOverflow(1);
    storage.PushAndOverflow(2);
    storage.PushAndOverflow(3);

    storage.MoveToFront(2);
    storage.MoveToFront(1);
    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{3, 1, 2});

    storage.MoveToFront(1);

    REQUIRE(storage.Elements == std::vector{3, 1, 2});

    storage.MoveToFront(1);

    REQUIRE(storage.Elements == std::vector{1, 3, 2});

    storage.MoveToFront(2);

    REQUIRE(storage.Elements == std::vector{1, 3, 2});

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{1, 3, 2});

    storage.MoveToFront(2);

    REQUIRE(storage.Elements == std::vector{2, 1, 3});

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{3, 2, 1});
}

TEST_CASE("MTF2 storage works with dynamic degree", "[MTF2Storage]") {
    MTF2Storage<int> storage(3, -1);
    storage.PushAndOverflow(1);
    storage.PushAndOverflow(2);
    storage.PushAndOverflow(3);

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{1, 3, 2});

    storage.MoveToFront(1);

    REQUIRE(storage.Elements == std::vector{1, 3, 2});

    storage.MoveToFront(2);

    REQUIRE(storage.Elements == std::vector{1, 2, 3});

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{3, 1, 2});

    storage.MoveToFront(3);

    REQUIRE(storage.Elements == std::vector{3, 1, 2});
}