#include "chip8.h"

Chip8::Chip8()
{
    // Inicializamos cpu
    m_cpu.initialize();
}

void Chip8::run()
{
    while (true)
    {
        emulateCycle(); // Ejecutar un ciclo de la CPU
    }
}

void Chip8::emulateCycle()
{
    m_cpu.executeOpcode();

    // Actualizar temporizadores
    m_cpu.updateTimers();
}
