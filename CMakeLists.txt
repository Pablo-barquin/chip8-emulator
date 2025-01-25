cmake_minimum_required(VERSION 3.10)
project(CHIP8_Emulator)

# Configuración de C++ estándar
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluye directorios de encabezados
include_directories(${CMAKE_SOURCE_DIR}/include)

# Archivos fuente
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Especificar la ruta de SDL2
set(SDL2_PATH "C:/SDL2")

# Agregar la carpeta 'include' al compilador
include_directories(${SDL2_PATH}/include/SDL2)
link_directories(${SDL2_PATH}/lib)

# Crear el ejecutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Vincular bibliotecas de SDL2
target_link_libraries(${PROJECT_NAME} SDL2 SDL2main)
