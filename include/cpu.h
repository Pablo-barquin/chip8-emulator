#ifndef CPU_H
#define CPU_H

class Cpu
{
public:
    Cpu();
    void initialize();
    void executeOpcode(unsigned short opcode);

private:
    unsigned char V[16];       // Registros V0 a VF
    unsigned short I;          // Registro I
    unsigned short pc;         // Contador de programa
    unsigned short stack[16];  // Pila
    unsigned short sp;         // Puntero de pila
    unsigned short opcode;     // Instrucción actual
    unsigned char delay_timer; // Temporizador de retraso
    unsigned char sound_timer; // Temporizador de sonido
};

#endif