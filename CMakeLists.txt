# Verify cmake version
cmake_minimum_required(VERSION 3.28)

# Create a project
project(shellter VERSION 1.0.0 DESCRIPTION "a survival shell(ter)" LANGUAGES C)

# Flags for compiling 
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -Wall -Wpedantic -Werror -Wextra -Wconversion -Wunused-parameter -Wmissing-prototypes -Wstrict-prototypes ")

# Includes headers
include_directories(include)

# Add the monitoring executable
# add_subdirectory(monitor_prometheus)  # Agrega esta línea para incluir el directorio de monitoreo

# Add executable
file(GLOB SRC_FILES src/*.c)
add_executable(${PROJECT_NAME} ${SRC_FILES})
