cmake_minimum_required(VERSION 3.25)
project(diploma)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
        src/encoding/utils/bit_input_stream.cpp
        src/encoding/utils/bit_input_stream.hpp
        src/encoding/utils/bit_output_stream.hpp src/encoding/utils/bit_output_stream.cpp src/encoding/arithmetic/base_coder.cpp src/encoding/arithmetic/base_coder.hpp src/encoding/arithmetic/arithmetic_encoder.hpp src/encoding/arithmetic/arithmetic_encoder.cpp src/encoding/arithmetic/arithmetic_decoder.cpp src/encoding/arithmetic/arithmetic_decoder.hpp src/modelling/base_model.hpp src/modelling/distribution.hpp src/defs.hpp src/modelling/ppm_model.cpp src/modelling/ppm_model.hpp src/modelling/naive_model.cpp src/modelling/naive_model.hpp src/encoding/log/primary_log_encoder.hpp src/encoding/log/link/log_link_decoder.hpp src/encoding/log/link/storage/log_storage.hpp src/encoding/log/link/storage/greedy_log_storage.cpp src/encoding/log/link/storage/greedy_log_storage.hpp src/encoding/log/link/residue_link_decoder.cpp src/encoding/log/link/residue_link_decoder.hpp src/encoding/generic/generic_encoder.hpp src/encoding/log/secondary/secondary_log_coder.hpp src/encoding/log/secondary/secondary_log_coder.hpp src/encoding/log/link/log_link_encoder.hpp src/encoding/log/link/log_link.hpp src/encoding/log/link/residue_link_encoder.hpp src/encoding/log/link/residue_link_encoder.cpp src/encoding/log/primary_log_encoder.cpp src/encoding/log/primary_log_decoder.cpp src/encoding/log/primary_log_decoder.hpp src/encoding/log/secondary/secondary_log_encoder.hpp src/encoding/log/secondary/secondary_log_decoder.hpp src/encoding/log/secondary/naive_secondary_log_encoder.cpp src/encoding/log/secondary/naive_secondary_log_encoder.hpp src/encoding/log/secondary/naive_secondary_log_decoder.cpp src/encoding/log/secondary/naive_secondary_log_decoder.hpp src/encoding/generic/subprepcs_encoder.cpp src/encoding/generic/subprepcs_encoder.hpp src/encoding/generic/generic_decoder.hpp src/encoding/generic/modelling_encoder.cpp src/encoding/generic/modelling_encoder.hpp src/encoding/generic/subprepcs_decoder.cpp src/encoding/generic/subprepcs_decoder.hpp src/encoding/generic/modelling_decoder.cpp src/encoding/generic/modelling_decoder.hpp src/encoding/residue_coder.hpp src/encoding/log/secondary/residue_secondary_log_encoder.cpp src/encoding/log/secondary/residue_secondary_log_encoder.hpp src/encoding/log/secondary/residue_secondary_log_decoder.cpp src/encoding/log/secondary/residue_secondary_log_decoder.hpp)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        src/
)

find_package(Catch2 REQUIRED)
add_executable(tests tests/encoding/utils/test_bit_input_stream.cpp tests/encoding/utils/test_bit_output_stream.cpp tests/encoding/arithmetic/test_arithmetic_coding.cpp tests/modelling/test_distribution.cpp tests/modelling/test_naive_model.cpp tests/modelling/test_ppm_model.cpp tests/encoding/log/link/storage/test_greedy_log_storage.cpp tests/encoding/log/link/test_residue_link_coder.cpp tests/encoding/log/test_primary_log_coder.cpp tests/encoding/generic/test_subprepcs_coder.cpp)
target_include_directories(tests
        PUBLIC
            tests/
)
target_link_libraries(tests
        Catch2::Catch2WithMain
        ${PROJECT_NAME}
)
