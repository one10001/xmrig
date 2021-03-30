if (NOT CMAKE_SYSTEM_PROCESSOR)
    message(WARNING "CMAKE_SYSTEM_PROCESSOR not defined")
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(/DRAPIDJSON_SSE2)
else()
    set(WITH_SSE4_1 OFF)
endif()

if (NOT ARM_TARGET)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|armv8-a)$")
        set(ARM_TARGET 8)
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(armv7|armv7f|armv7s|armv7k|armv7-a|armv7l)$")
        set(ARM_TARGET 7)
    endif()
endif()

if (ARM_TARGET AND ARM_TARGET GREATER 6)
    set(PYTHONXM_ARM     ON)
    add_definitions(/DPYTHONXM_ARM)

    message(STATUS "Use ARM_TARGET=${ARM_TARGET} (${CMAKE_SYSTEM_PROCESSOR})")

    include(CheckCXXCompilerFlag)

    if (ARM_TARGET EQUAL 8)
        set(PYTHONXM_ARMv8 ON)
        add_definitions(/DPYTHONXM_ARMv8)

        CHECK_CXX_COMPILER_FLAG(-march=armv8-a+crypto PYTHONXM_ARM_CRYPTO)

        if (PYTHONXM_ARM_CRYPTO)
            add_definitions(/DPYTHONXM_ARM_CRYPTO)
            set(ARM8_CXX_FLAGS "-march=armv8-a+crypto")
        else()
            set(ARM8_CXX_FLAGS "-march=armv8-a")
        endif()
    elseif (ARM_TARGET EQUAL 7)
        set(PYTHONXM_ARMv7 ON)
        add_definitions(/DPYTHONXM_ARMv7)
    endif()
endif()

if (WITH_SSE4_1)
    add_definitions(/DPYTHONXM_FEATURE_SSE4_1)
endif()
