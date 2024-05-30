//
// Created by zeliboba on 5/23/24.
//

#include <catch2/catch_test_macros.hpp>
#include "modelling/naive_model.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "utils/mtf2_storage.hpp"
#include "utils/mtf2_stable_storage.hpp"

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

TEST_CASE("MTF2 works when stable", "[MTF2StableStorage]") {
    MTF2StableStorage<int> storage(3, 1);
    storage.PushAndOverflow(1);
    storage.PushAndOverflow(2);
    storage.PushAndOverflow(3);

    storage.MoveToFrontByIndex(2);

    REQUIRE(storage.Elements == std::vector{1, 2, 3});

    storage.MoveToFrontByIndex(1);

    REQUIRE(storage.Elements == std::vector{1, 2, 3});

    storage.PushAndOverflow(4);

    REQUIRE(storage.Elements == std::vector{4, 2, 3});

    storage.PushAndOverflow(5);

    REQUIRE(storage.Elements == std::vector{4, 2, 5});

    storage.MoveToFrontByIndex(0);

    REQUIRE(storage.Elements == std::vector{4, 2, 5});

    storage.MoveToFrontByIndex(2);

    REQUIRE(storage.Elements == std::vector{4, 2, 5});

    storage.PushAndOverflow(6);

    REQUIRE(storage.Elements == std::vector{4, 6, 5});
}