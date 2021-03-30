if (WIN32)
    set(PYTHONXM_OS_WIN ON)
elseif (APPLE)
    set(PYTHONXM_OS_APPLE ON)

    if (IOS OR CMAKE_SYSTEM_NAME STREQUAL iOS)
        set(PYTHONXM_OS_IOS ON)
    else()
        set(PYTHONXM_OS_MACOS ON)
    endif()
else()
    set(PYTHONXM_OS_UNIX ON)

    if (ANDROID OR CMAKE_SYSTEM_NAME MATCHES "Android")
        set(PYTHONXM_OS_ANDROID ON)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(PYTHONXM_OS_LINUX ON)
    elseif(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
        set(PYTHONXM_OS_FREEBSD ON)
    endif()
endif()


if (PYTHONXM_OS_WIN)
    add_definitions(/DWIN32)
    add_definitions(/DPYTHONXM_OS_WIN)
elseif(PYTHONXM_OS_APPLE)
    add_definitions(/DPYTHONXM_OS_APPLE)

    if (PYTHONXM_OS_IOS)
        add_definitions(/DPYTHONXM_OS_IOS)
    else()
        add_definitions(/DPYTHONXM_OS_MACOS)
    endif()

    if (PYTHONXM_ARM)
        set(WITH_SECURE_JIT ON)
    endif()
elseif(PYTHONXM_OS_UNIX)
    add_definitions(/DPYTHONXM_OS_UNIX)

    if (PYTHONXM_OS_ANDROID)
        add_definitions(/DPYTHONXM_OS_ANDROID)
    elseif (PYTHONXM_OS_LINUX)
        add_definitions(/DPYTHONXM_OS_LINUX)
    elseif (PYTHONXM_OS_FREEBSD)
        add_definitions(/DPYTHONXM_OS_FREEBSD)
    endif()
endif()

if (WITH_SECURE_JIT)
    add_definitions(/DPYTHONXM_SECURE_JIT)
endif()
