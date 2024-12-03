# Especifica la versión mínima de CMake
cmake_minimum_required(VERSION 3.28.3)

# Nombre del proyecto
project(MetricsProject)

# Establece el estándar de C para tu proyecto
set(CMAKE_C_STANDARD 17)

# Incluye el directorio de encabezados
include_directories(include)

# Añade los archivos fuente al proyecto
set(SOURCES
    src/metrics.c
    src/main.c
    src/expose_metrics.c
    src/cjson_metrics.c
)

# Crea el ejecutable
add_executable(metrics ${SOURCES})

#Especifica las biblioteca cjson
find_package(cJSON REQUIRED)

#Busca las bibliotecas externas
find_library(PROM_LIB prom REQUIRED)
find_library(PROMHTTP_LIB promhttp REQUIRED)

# Link libraries
target_link_libraries(metrics PRIVATE ${PROM_LIB} ${PROMHTTP_LIB} pthread cjson::cjson)

# Establece el directorio de salida para el ejecutable
set_target_properties(metrics PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)  

# Mensaje recordatorio para la biblioteca compartida
message(STATUS "Para ejecutar, asegúrate de tener configurado LD_LIBRARY_PATH:")
message(STATUS "export LD_LIBRARY_PATH=/usr/local/lib:\$LD_LIBRARY_PATH")