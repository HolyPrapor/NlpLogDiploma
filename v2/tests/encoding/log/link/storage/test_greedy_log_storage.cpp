//
// Created by zeliboba on 2/25/24.
//

#include <catch2/catch_test_macros.hpp>
#include "modelling/naive_model.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"

TEST_CASE("Greedy log storage respects max size", "[GreedyLogStorage]") {
    GreedyLogStorage storage(3);
    storage.Store({1});
    storage.Store({2});
    storage.Store({3});

    SECTION("Has all logs when size is less than max") {
        REQUIRE(storage.TryLink({1}, 0).has_value());
        REQUIRE(storage.TryLink({2}, 0).has_value());
        REQUIRE(storage.TryLink({3}, 0).has_value());
    }

    SECTION("Rotates logs when size is equal to max") {
        storage.Store({4});
        REQUIRE_FALSE(storage.TryLink({1}, 0).has_value());
        REQUIRE(storage.TryLink({2}, 0).has_value());
        REQUIRE(storage.TryLink({3}, 0).has_value());
        REQUIRE(storage.TryLink({4}, 0).has_value());
    }
}

TEST_CASE("Greedy log storage correctly finds logs", "[GreedyLogStorage]") {
    GreedyLogStorage storage(3);
    storage.Store({1, 2, 3, 4});
    storage.Store({1, 2, 3, 4, 5});
    storage.Store({1, 2, 3});

    REQUIRE(storage.GetSize() == 3);

    SECTION("Finds the longest matching substring across all stored logs") {
        auto link = storage.TryLink({1, 2, 3, 4, 5, 6, 7, 8}, 0);
        REQUIRE(link.has_value());
        REQUIRE(link->RecordIndex == 1);
        REQUIRE(link->StartIndex == 0);
        REQUIRE(link->Length == 5);
    }

    SECTION("Respects the start index") {
        auto link = storage.TryLink({1, 2, 3, 4, 5, 6, 7, 8}, 1);
        REQUIRE(link.has_value());
        REQUIRE(link->RecordIndex == 1);
        REQUIRE(link->StartIndex == 1);
        REQUIRE(link->Length == 4);
    }

    SECTION("Matches from start index") {
        storage.Store({2, 3, 4, 5, 6, 7, 8});
        auto link = storage.TryLink({1, 2, 3, 4, 5, 6, 7, 8}, 0);
        REQUIRE(link.has_value());
        REQUIRE(link->RecordIndex == 0);
        REQUIRE(link->StartIndex == 0);
        REQUIRE(link->Length == 5);
    }
}