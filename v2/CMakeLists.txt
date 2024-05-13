cmake_minimum_required(VERSION 3.25)
project(diploma)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CONFIG_PROTO_FILE ${CMAKE_SOURCE_DIR}/src/config.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CONFIG_PROTO_FILE})

add_library(${PROJECT_NAME}
        src/encoding/utils/bit_input_stream.cpp
        src/encoding/utils/bit_output_stream.cpp
        src/encoding/arithmetic/base_coder.cpp
        src/encoding/arithmetic/arithmetic_encoder.cpp
        src/encoding/arithmetic/arithmetic_decoder.cpp
        src/modelling/ppm_model.cpp
        src/modelling/naive_model.cpp
        src/encoding/log/link/storage/greedy_log_storage.cpp
        src/encoding/log/link/residue_link_decoder.cpp
        src/encoding/log/link/residue_link_encoder.cpp
        src/encoding/log/primary_log_encoder.cpp
        src/encoding/log/primary_log_decoder.cpp
        src/encoding/log/secondary/naive_secondary_log_encoder.cpp
        src/encoding/log/secondary/naive_secondary_log_decoder.cpp
        src/encoding/generic/subprepcs_encoder.cpp
        src/encoding/generic/modelling_encoder.cpp
        src/encoding/generic/subprepcs_decoder.cpp
        src/encoding/generic/modelling_decoder.cpp
        src/encoding/log/secondary/residue_secondary_log_encoder.cpp
        src/encoding/log/secondary/residue_secondary_log_decoder.cpp
        src/encoding/log/secondary/ppm_secondary_log_encoder.cpp
        src/encoding/log/secondary/ppm_secondary_log_decoder.cpp
        src/encoding/log/link/storage/mtf_log_storage.cpp
        src/encoding/generic/identity_encoder.cpp
        src/encoding/generic/identity_decoder.cpp
        ${PROTO_SRCS}
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(${PROJECT_NAME}
        ${PROTOBUF_LIBRARIES}
        )

add_executable(cli src/cli.cpp)
target_include_directories(cli
        PRIVATE
        src/
        ${CMAKE_CURRENT_BINARY_DIR}
        )
target_link_libraries(cli ${PROJECT_NAME} ${PROTOBUF_LIBRARIES})

find_package(Catch2 REQUIRED)
add_executable(tests
        tests/encoding/utils/test_bit_input_stream.cpp
        tests/encoding/utils/test_bit_output_stream.cpp
        tests/encoding/arithmetic/test_arithmetic_coding.cpp
        tests/modelling/test_distribution.cpp
        tests/modelling/test_naive_model.cpp
        tests/modelling/test_ppm_model.cpp
        tests/encoding/log/link/storage/test_greedy_log_storage.cpp
        tests/encoding/log/link/test_residue_link_coder.cpp
        tests/encoding/log/test_primary_log_coder.cpp
        tests/encoding/generic/test_subprepcs_coder.cpp)

target_include_directories(tests
        PUBLIC
        tests/
        )

target_link_libraries(tests
        Catch2::Catch2WithMain
        ${PROJECT_NAME}
        )