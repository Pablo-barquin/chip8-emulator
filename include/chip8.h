#ifndef CHIP8_H
#define CHIP8_H

#include "cpu.h"
#include "display.h"
#include "input.h"
#include "memory.h"

class Chip8
{
public:
    Chip8();
    void initialize();
    void loadGame(const char *filename);
    void run();

private:
    Cpu m_cpu;         // Manejo de CPU y registros
    Memory m_memory;   // Manejo de la memoria
    Display m_display; // Manejo de la pantalla
    Input m_input;     // Manejo del teclado

    void emulateCycle();
};

#endif