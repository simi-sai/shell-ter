cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

include_directories(/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Crear el ejecutable de pruebas
add_executable(test_shellter
    ${CMAKE_CURRENT_SOURCE_DIR}/test_shellter.c
    ${CMAKE_SOURCE_DIR}/src/commands.c
    ${CMAKE_SOURCE_DIR}/src/manager.c
    ${CMAKE_SOURCE_DIR}/src/monitor.c
    ${CMAKE_SOURCE_DIR}/src/prompt.c
    ${CMAKE_SOURCE_DIR}/src/signals.c
)

target_link_libraries(test_shellter PRIVATE unity::unity cjson::cjson)

# Definir la macro TESTING solo para las pruebas
target_compile_definitions(test_shellter PRIVATE TESTING) # Agrega esta línea para definir la macro TESTING

# Registrar la prueba con CTest
add_test(NAME test_shellter COMMAND test_shellter)

