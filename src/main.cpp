#include "chip8.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <ruta_al_juego>" << std::endl;
        return 1;
    }

    // Crear instancia del emulador
    Chip8 chip8;

    // Cargar el juego especificado
    const char* romPath = argv[1];
    chip8.loadGame(romPath);

    // Ejecutar el emulador
    chip8.run();

    return 0;
}