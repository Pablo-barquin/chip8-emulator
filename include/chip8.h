#ifndef CHIP8_H
#define CHIP8_H

#include "cpu.h"

class Chip8
{
public:
    Chip8();
    void loadGame(const char *filename);
    void run();

private:
    Cpu m_cpu;         // Manejo de CPU y registros
    void emulateCycle();
};

#endif