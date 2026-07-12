cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(thinpod_gateway_uwb_initiator_stage2_rf)

target_sources(app PRIVATE
    src/main.c
    ../../common/thinpod_protocol/thinpod_protocol.c
)

target_include_directories(app PRIVATE
    ../../common/thinpod_protocol
)

add_subdirectory(
    ../../common/qorvo_dw3000
    ${CMAKE_CURRENT_BINARY_DIR}/qorvo_dw3000
)
