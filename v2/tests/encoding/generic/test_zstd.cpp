//
// Created by zeliboba on 5/30/24.
//

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include "encoding/generic/zstd/zstdstream.hpp"

void doRoundTrip(const std::string& data) {
    std::stringstream ss;
    zstd::ostream zOutStream(ss);
    zOutStream << data;
    zOutStream.flush();
    zstd::istream zInStream(ss);
    std::string result((std::istreambuf_iterator<char>(zInStream)), std::istreambuf_iterator<char>());
    REQUIRE(result == data);
}

TEST_CASE("ZSTD streams round trip", "[ZstdStream]") {
    doRoundTrip("Hello, Zstd!");
    doRoundTrip(std::string(100000, 'a') + std::string(100000, 'b') + std::string(100000, 'c'));
}