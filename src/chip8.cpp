#include "chip8.h"

Chip8::Chip8()
{
    initialize_CPU();     // Inicializamos CPU
    initialize_Display(); // Inicializamos Display
    initialize_Input();   // Inicializamos Input
    initialize_Memory();  // Inicializamos Memoria
}

void Chip8::initialize_CPU()
{
    // Inicializamos los registros y el contador de programa
    program_counter = 0x200; // Programas en CHIP-8 comienzan en esta posición de memoria
    opcode = 0;
    I = 0;
    stack_ptr = 0;

    // Inicializamos registros V y temporizadores
    for (int i = 0; i < 16; ++i)
        V[i] = 0;

    delay_timer = 0;
    sound_timer = 0;
}

void Chip8::initialize_Memory()
{
    // Limpiamos la memoria
    for (int i = 0; i < 4096; i++)
        memory[i] = 0;

    loadFontset(); // Cargar las fuentes de caracteres
}

void Chip8::initialize_Display()
{
    // Limpiamos la pantalla
    for (int i = 0; i < 64 * 32; ++i)
        screen[i] = 0;
}

void Chip8::initialize_Input()
{
    // Inicializamos estado de las teclas
    for (int i = 0; i < 16; ++i)
        keys[i] = 0;
}

void Chip8::run()
{
    while (true)
    {
        executeOpcode(); // Ejecutar un ciclo de la CPU
        updateTimers();  // Actualizar temporizadores
    }
}

void Chip8::loadFontset()
{
    // Cargamos las fuentes en la memoria
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i]; // Acceder directamente al array
}

void Chip8::executeOpcode()
{
    // 1. Fetch: Leer el opcode de la memoria desde la dirección pc
    opcode = memory[program_counter] << 8 | memory[program_counter + 1];

    unsigned short vx;
    unsigned short vy;
    unsigned char random_number;

    // Decode opcode
    switch (opcode & 0xF000)
    {
    // Salta a la instrucción NNN
    case 0x1000:
        program_counter = opcode & 0x0FFF;
        break;

    // Llama a la subrutina NNN
    case 0x2000:
        stack[stack_ptr] = program_counter;
        ++stack_ptr;
        program_counter = opcode & 0x0FFF;
        break;

    // Salta la siguiente instrucción si VX == NN
    case 0x3000:
        vx = (opcode & 0x0F00) >> 8;
        if (V[vx] == (opcode & 0x00FF))
            program_counter += 2;
        break;

    // Salta la siguiente instrucción si VX != NN
    case 0x4000:
        vx = (opcode & 0x0F00) >> 8;
        if (V[vx] != (opcode & 0x00FF))
            program_counter += 2;
        break;

    // Salta la siguiente instrucción si VX == VY
    case 0x5000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;
        if (V[vx] == V[vy])
            program_counter += 2;
        break;

    // Asignar valor NN a VX
    case 0x6000:
        vx = (opcode & 0x0F00) >> 8;
        V[vx] = (opcode & 0x00FF);
        break;

    // Incrementar valor NN en VX
    case 0x7000:
        vx = (opcode & 0x0F00) >> 8;
        V[vx] += (opcode & 0x00FF);
        break;

    case 0x8000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;
        switch (opcode & 0x000F)
        {
        // Asignar valor de VY a VX
        case 0x0000:
            V[vx] = V[vy];
            break;

        // Asignar valor de (VX or VY) a VX
        case 0x0001:
            V[vx] |= V[vy];
            break;

        // Asignar valor de (VX and VY) a VX
        case 0x0002:
            V[vx] &= V[vy];
            break;

        // Asignar valor de (VX xor VY) a VX
        case 0x0003:
            V[vx] ^= V[vy];
            break;

        // Suma VY a VX, con acarreo
        case 0x0004:
            if (V[vx] + V[vy] > 0xFF)
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[vx] = (V[vx] + V[vy]) & 0xFF;
            break;

        // Resta VY a VX, con acarreo
        case 0x0005:
            if (V[vx] >= V[vy])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[vx] -= V[vy];
            break;

        // Desplazamiento a derecha de 1 a VX
        case 0x0006:
            V[0xF] = V[vx] & 0x1;
            V[vx] >>= 1;
            break;

        // Resta VX a VY, con acarreo
        case 0x0007:
            if (V[vy] >= V[vx])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[vx] = V[vy] - V[vx];
            break;

        // Desplazamiento a izquierda de 1 a VX
        case 0x000E:
            V[0xF] = (V[vx] & 0x80) >> 7;
            V[vx] <<= 1;
            break;

        default:
            std::cout << "Unknown opcode: " << opcode << std::endl;
        }

    // Salta a la siguiente instrucción si VX != VY
    case 0x9000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;
        if (V[vx] != V[vy])
            program_counter += 2;
        break;

    // Apunta I a la instrucción NNN
    case 0xA000:
        I = opcode & 0x0FFF;
        break;

    // Salta a la instrucción NNN + V0
    case 0xB000:
        program_counter = V[0] + (opcode & 0x0FFF);
        break;

    // Generar un número aleatorio entre 0 y 255, AND con NN, y almacenar en VX
    case 0xC000:
        vx = (opcode & 0x0F00) >> 8;
        random_number = rand() % 256;
        V[vx] = random_number & (opcode & 0x00FF);
        break;

    // Dibuja un sprite en la coordenada (VX, VY) de 8 pixeles de ancho y N pixeles de alto
    case 0xD000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;

        drawSprite(V[vx], V[vy], (opcode & 0x000F));
        break;

    // Salta a la siguiente instrucción si la tecla en VX es presionada
    case 0xE000:

        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;

        drawSprite(V[vx], V[vy], (opcode & 0x000F));
        break;

    default:
        std::cout << "Unknown opcode: " << opcode << std::endl;
    }

    // Cada instrucción son 2 bytes, por lo que incrementamos el contador en 2
    program_counter += 2;
}

void Chip8::updateTimers()
{
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
            std::cout << "BEEP!" << std::endl;
        --sound_timer;
    }
}

void Chip8::drawSprite(unsigned short x, unsigned short y, unsigned short N)
{
    for (size_t yline = 0; yline < (opcode & 0x000F); yline++)
    {
        unsigned char data = memory[I + yline]; // this retreives the byte for a give line of pixels
        for (size_t xpix = 0; xpix < 8; xpix++)
        {
            if ((data & (0x80 >> xpix)) != 0)
            {
                if (screen[V[x] + (V[y] * 64)] == 1)
                    V[0xF] = 1;                  // there has been a collision
                screen[V[x] + (V[y] * 64)] ^= 1; // note: coordinate registers from opcode
            }
        }
    }
}
