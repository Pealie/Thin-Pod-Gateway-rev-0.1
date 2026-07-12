# Thin-Pod external Qorvo DW3000/QM33 driver integration.
#
# Qorvo source remains outside this repository.
# Set QORVO_DW3_SDK_ROOT to the installed DW3_QM33 SDK directory.

if(NOT DEFINED ENV{QORVO_DW3_SDK_ROOT})
    message(FATAL_ERROR
        "QORVO_DW3_SDK_ROOT is not set. "
        "Set it to the installed Qorvo DW3_QM33 SDK root.")
endif()

file(TO_CMAKE_PATH "$ENV{QORVO_DW3_SDK_ROOT}" QORVO_DW3_SDK_ROOT)

set(QORVO_DW3000_DRIVER_ROOT
    "${QORVO_DW3_SDK_ROOT}/Drivers/API/Shared/dwt_uwb_driver")

set(QORVO_DW3000_DEVICE_ROOT
    "${QORVO_DW3000_DRIVER_ROOT}/dw3000")

set(QORVO_QMATH_ROOT
    "${QORVO_DW3000_DRIVER_ROOT}/lib/qmath")

set(QORVO_REQUIRED_FILES
    "${QORVO_DW3000_DRIVER_ROOT}/LICENSES/LicenseRef-QORVO-2.txt"
    "${QORVO_DW3000_DRIVER_ROOT}/deca_compat.c"
    "${QORVO_DW3000_DRIVER_ROOT}/deca_interface.c"
    "${QORVO_DW3000_DRIVER_ROOT}/deca_rsl.c"
    "${QORVO_DW3000_DEVICE_ROOT}/dw3000_device.c"
    "${QORVO_QMATH_ROOT}/src/qmath.c"
)

foreach(required_file IN LISTS QORVO_REQUIRED_FILES)
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR
            "Required external Qorvo file is missing: ${required_file}")
    endif()
endforeach()

target_sources(app PRIVATE
    "${QORVO_DW3000_DRIVER_ROOT}/deca_compat.c"
    "${QORVO_DW3000_DRIVER_ROOT}/deca_interface.c"
    "${QORVO_DW3000_DRIVER_ROOT}/deca_rsl.c"
    "${QORVO_DW3000_DEVICE_ROOT}/dw3000_device.c"
    "${QORVO_QMATH_ROOT}/src/qmath.c"
)

target_include_directories(app PRIVATE
    "${QORVO_DW3000_DRIVER_ROOT}"
    "${QORVO_DW3000_DEVICE_ROOT}"
    "${QORVO_QMATH_ROOT}/include"
)

target_compile_definitions(app PRIVATE
    USE_DRV_DW3000
)

message(STATUS
    "Thin-Pod external Qorvo DW3000 driver: ${QORVO_DW3000_DRIVER_ROOT}")
