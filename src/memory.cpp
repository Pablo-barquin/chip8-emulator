#include "memory.h"

Memory::Memory()
{
    initialize();
}

void Memory::initialize()
{
    // Limpiamos la memoria
    for (int i = 0; i < 4096; i++)
        memory[i] = 0;

    loadFontset(); // Cargar las fuentes de caracteres
}

unsigned char Memory::readMemory(unsigned short address)
{
    return memory[address];
}

void Memory::writeMemory(unsigned short address, unsigned char value)
{
    memory[address] = value;
}

void Memory::loadFontset()
{
    // Cargamos las fuentes en la memoria
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i]; // Acceder directamente al array
}