#include "cpu.h"

Cpu::Cpu()
{
    initialize();
}

void Cpu::initialize()
{
    // Inicializamos los registros y el contador de programa
    pc = 0x200; // Programas en CHIP-8 comienzan en esta posición de memoria
    opcode = 0;
    I = 0;
    sp = 0;

    // Inicializamos registros V y temporizadores
    for (int i = 0; i < 16; ++i)
        V[i] = 0;

    delay_timer = 0;
    sound_timer = 0;
}
