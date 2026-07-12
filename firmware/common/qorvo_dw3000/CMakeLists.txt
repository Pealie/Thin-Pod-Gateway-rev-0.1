target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/thinpod_dw3000_platform.c
    ${CMAKE_CURRENT_LIST_DIR}/src/thinpod_dw3000_identity.c
)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

include(${CMAKE_CURRENT_LIST_DIR}/qorvo_dw3000_external.cmake)
