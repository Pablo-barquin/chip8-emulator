cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(chip8-emulator)

# Establecer el estándar de C++ (C++11, C++14, C++17, etc.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Agregar el directorio de encabezados (include/) al path de búsqueda
include_directories(${CMAKE_SOURCE_DIR}/include)

# Recoge automáticamente todos los archivos fuente de src/
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Añade el archivo main.cpp como fuente
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/main.cpp")

# Crear el ejecutable a partir de los archivos fuente
add_executable(chip8 ${SOURCES})
