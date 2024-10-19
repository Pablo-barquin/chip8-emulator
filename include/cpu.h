#ifndef CPU_H
#define CPU_H

#include <iostream>

#include "display.h"
#include "input.h"
#include "memory.h"

class Cpu
{
public:
    Cpu() {}
    void initialize();
    void executeOpcode();
    void updateTimers();

    // Operaciones Get para obtener datos privados
    unsigned short getPC() { return program_counter; }

private:
    unsigned char V[16];            // Registros V0 a VF
    unsigned short I;               // Registro I
    unsigned short program_counter; // Contador de programa
    unsigned short stack[16];       // Pila
    unsigned short stack_ptr;       // Puntero de pila
    unsigned short opcode;          // Instrucción actual
    unsigned char delay_timer;      // Temporizador de retraso
    unsigned char sound_timer;      // Temporizador de sonido

    Memory m_memory;   // Manejo de la memoria
    Display m_display; // Manejo de la pantalla
    Input m_input;     // Manejo del teclado
};

#endif