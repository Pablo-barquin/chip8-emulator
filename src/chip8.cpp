#include "chip8.h"

Chip8::Chip8()
{
    // Inicialización en el constructor
    initialize();
}

void Chip8::initialize()
{
    // Inicializamos registros, memoria y pantalla
    m_cpu.initialize();
    m_memory.initialize();
    m_display.clear();
    m_input.reset();
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
    // 1. Fetch: Leer el opcode de la memoria desde la dirección pc
    unsigned short opcode = m_memory.readMemory(m_cpu.getPC()) << 8 | m_memory.readMemory(m_cpu.getPC() + 1);

    // 2. Decode & Execute el Opcode
    m_cpu.executeOpcode(opcode);

    // Actualizar temporizadores
    m_cpu.updateTimers();
}
