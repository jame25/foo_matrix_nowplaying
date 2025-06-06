cmake_minimum_required(VERSION 3.20)
project(foo_matrix_nowplaying VERSION 1.0.0 LANGUAGES CXX)

# Force 32-bit build for Foobar2000 compatibility
set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_SIZEOF_VOID_P 4)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain should be set via command line:
# -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
# This enables manifest mode automatically

# No external packages needed - using Windows WinINet and simple JSON handling

# Set Foobar2000 SDK path
set(FOOBAR2000_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/foobar2000_SDK" CACHE PATH "Path to Foobar2000 SDK")

if(NOT EXISTS "${FOOBAR2000_SDK_PATH}/foobar2000/SDK/foobar2000.h")
    message(FATAL_ERROR "Foobar2000 SDK not found at ${FOOBAR2000_SDK_PATH}")
endif()

# Build our component with minimal SDK dependencies
add_library(foo_matrix_nowplaying SHARED
    src/main.cpp
    src/core_api_impl.cpp
    src/simple_matrix_client.cpp
    src/simple_matrix_client.h
    src/play_callback.cpp
    src/play_callback.h
    src/preferences.cpp
    src/preferences.h
    src/announce_hotkey.cpp
    src/announce_hotkey.h
    src/resource.rc
    # Include only essential SDK files
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK/console.cpp
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK/service.cpp
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK/componentversion.cpp
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK/guids.cpp
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK/preferences_page.cpp
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK/mainmenu.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/string_base.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/string_conv.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/string-lite.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/audio_sample.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/other.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/printf.cpp
    ${FOOBAR2000_SDK_PATH}/pfc/guid.cpp
)

# Set output name without 'lib' prefix
set_target_properties(foo_matrix_nowplaying PROPERTIES
    PREFIX ""
    OUTPUT_NAME "foo_matrix_nowplaying"
)

# Include directories
target_include_directories(foo_matrix_nowplaying PRIVATE
    ${FOOBAR2000_SDK_PATH}
    ${FOOBAR2000_SDK_PATH}/foobar2000/SDK
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries (none needed for simple client)

# Windows specific settings
if(WIN32)
    target_compile_definitions(foo_matrix_nowplaying PRIVATE
        _WINDOWS
        _USRDLL
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
    )
    
    # Set subsystem to Windows
    set_target_properties(foo_matrix_nowplaying PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
    
    # Multi-threaded runtime - STATIC linking to avoid runtime dependencies
    set_property(TARGET foo_matrix_nowplaying PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    
    # Link Windows libraries
    target_link_libraries(foo_matrix_nowplaying PRIVATE
        # Windows libraries
        ws2_32
        winmm
        wldap32
        crypt32
        normaliz
        user32
        gdi32
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
        shlwapi
    )
endif()

# Optimization for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(foo_matrix_nowplaying PRIVATE
        /O2     # Optimize for speed
        /GL     # Whole program optimization
    )
    set_target_properties(foo_matrix_nowplaying PROPERTIES
        LINK_FLAGS "/LTCG"  # Link-time code generation
    )
endif()

# Copy DLL to Foobar2000 components folder after build (optional)
set(FOOBAR2000_PATH "C:/Program Files (x86)/foobar2000" CACHE PATH "Path to Foobar2000 installation")
if(EXISTS "${FOOBAR2000_PATH}/components")
    add_custom_command(TARGET foo_matrix_nowplaying POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:foo_matrix_nowplaying>
        "${FOOBAR2000_PATH}/components/"
        COMMENT "Copying component to Foobar2000 components folder"
    )
endif()

# Install target
install(TARGETS foo_matrix_nowplaying
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Package configuration
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "foo_matrix_nowplaying")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Foobar2000 Matrix Now Playing Component")
include(CPack)