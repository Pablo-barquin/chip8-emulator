#include "chip8.h"

Chip8::Chip8() {
    // Inicialización en el constructor
    initialize();
}

void Chip8::initialize() {
    // Inicializamos registros, memoria y pantalla
    m_cpu.initialize();
    m_memory.initialize();
    m_display.clear();
    m_input.reset();
}
