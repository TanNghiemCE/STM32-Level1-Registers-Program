cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME GPIO_Bare_Metal)

# Include toolchain file
include("gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# --- 1. THÊM CỜ KIẾN TRÚC ARM CORTEX-M3 ---
set(CPU_PARAMETERS -mcpu=cortex-m3 -mthumb)
add_compile_options(${CPU_PARAMETERS} -Wall -fdata-sections -ffunction-sections)
add_link_options(${CPU_PARAMETERS} -Wl,--gc-sections,--print-memory-usage)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})




# BỔ SUNG CỜ USE_HAL_DRIVER VÀO ĐÂY
set(BAREMETAL_Defines_Syms 
    STM32F103xB
    #USE_HAL_DRIVER
    # USE_FULL_ASSERT
    HSI_VALUE=8000000
    $<$<CONFIG:Debug>:DEBUG>
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${BAREMETAL_Defines_Syms}
)

file(GLOB_RECURSE PROJECT_SOURCES 
    "Core/*.c"
    "Devices/*.c"
    "Core/*.s"
    "Core/*.S"
    "*.s"
)
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*_template\\.c$|.*_example\\.c$")

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
    Devices/Inc
    CMSIS/Include
)


# Thêm toàn bộ file tìm được vào target (Đã đồng nhất dùng biến CMAKE_PROJECT_NAME)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

# KHAI BÁO THƯ MỤC CHỨA FILE HEADER (.h)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
    Core/Include
)

# Post-build commands to generate hex and bin files
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Building HEX and BIN files"
)

# Print executable size
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMENT "Program size:"
)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES 
    ADDITIONAL_CLEAN_FILES "${CMAKE_PROJECT_NAME}.map;${CMAKE_PROJECT_NAME}.hex;${CMAKE_PROJECT_NAME}.bin"
)

# Validate that code is compatible with C standard
if((CMAKE_C_STANDARD LESS 11))
    message(FATAL_ERROR "Generated code requires C11 or higher")
endif()