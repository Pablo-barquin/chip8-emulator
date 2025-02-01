#ifndef CHIP8_H
#define CHIP8_H

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include <fontset.h>
#include <iostream>

class Chip8
{
public:
    // Constructor y destructor
    Chip8();
    ~Chip8();

    // Inicialización de componentes
    void initialize_CPU();
    void initialize_Memory();
    void initialize_Display();  // Sirve para reiniciar la pantalla
    void initialize_Input();

    // Emulacion & Ciclos
    void run();
    void executeOpcode();
    void updateTimers();
    void render();
    void handleInput(const SDL_Event &event);

    // Dibujado en pantalla
    void drawSprite(unsigned short x, unsigned short y, unsigned short N);

    // Carga de Juego
    void loadGame(const char *filename);

private:
    // CPU
    unsigned char V[16];            // Registros V0 a VF
    unsigned short I;               // Registro I
    unsigned short program_counter; // Contador de programa
    unsigned short stack[16];       // Pila
    unsigned short stack_ptr;       // Puntero de pila
    unsigned short opcode;          // Instrucción actual
    unsigned char delay_timer;      // Temporizador de retraso
    unsigned char sound_timer;      // Temporizador de sonido

    // Memoria
    void loadFontset();
    unsigned char memory[4096]; // Memoria de 4KB

    // Display
    unsigned char screen[64 * 32]; // Pantalla de 64x32

    // Input
    unsigned char keys[16]; // Teclado hexadecimal

    // Manejo de errores
    bool running;

    // SDL
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

#endif