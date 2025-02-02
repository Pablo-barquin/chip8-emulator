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
    void drawSprite(uint8_t x, uint8_t y, uint8_t N);

    // Carga de Juego
    void loadGame(const char *filename);

private:
    // CPU
    uint8_t V[16];            // Registros V0 a VF
    uint16_t I;               // Registro I
    uint16_t program_counter; // Contador de programa
    uint16_t stack[16];       // Pila
    uint16_t stack_ptr;       // Puntero de pila
    uint16_t opcode;          // Instrucción actual
    uint8_t delay_timer;      // Temporizador de retraso
    uint8_t sound_timer;      // Temporizador de sonido

    // Memoria
    void loadFontset();
    uint8_t memory[4096]; // Memoria de 4KB

    // Display
    uint8_t screen[64 * 32]; // Pantalla de 64x32

    // Input
    uint8_t keys[16]; // Teclado hexadecimal

    // Manejo de errores
    bool running;

    // SDL
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

#endif