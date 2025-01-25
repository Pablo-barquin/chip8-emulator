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
    running = true;
    while (running)
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
    case 0x0000:
        // Limpia la pantalla
        if ((opcode & 0x00FF) == 0x0E0)
        {
            initialize_Display();
            program_counter += 2;
        }
        // Volver de una subrutina
        else if ((opcode & 0x00FF) == 0x0EE)
        {
            --stack_ptr;
            program_counter = stack[stack_ptr];
            stack[stack_ptr] = 0;
        }
        break;

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
            program_counter += 4;
        break;

    // Salta la siguiente instrucción si VX != NN
    case 0x4000:
        vx = (opcode & 0x0F00) >> 8;
        if (V[vx] != (opcode & 0x00FF))
            program_counter += 4;
        break;

    // Salta la siguiente instrucción si VX == VY
    case 0x5000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;
        if (V[vx] == V[vy])
            program_counter += 4;
        break;

    // Asignar valor NN a VX
    case 0x6000:
        vx = (opcode & 0x0F00) >> 8;
        V[vx] = (opcode & 0x00FF);
        program_counter += 2;
        break;

    // Incrementar valor NN en VX
    case 0x7000:
        vx = (opcode & 0x0F00) >> 8;
        V[vx] += (opcode & 0x00FF);
        program_counter += 2;
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
        {
            unsigned short sum = V[vx] + V[vy];
            V[0xF] = (sum > 0xFF) ? 1 : 0;
            V[vx] = sum & 0xFF;
            break;
        }

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
            running = false;
        }
        program_counter += 2;
        break;

    // Salta a la siguiente instrucción si VX != VY
    case 0x9000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;
        if (V[vx] != V[vy])
            program_counter += 4;
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

    case 0xE000:
        vx = (opcode & 0x0F00) >> 8;
        // Salta a la siguiente instrucción si la tecla en VX es presionada
        if ((opcode & 0x00FF) == 0x009E)
        {
            if (keys[V[vx]] != 0)
                program_counter += 4;
            else
                program_counter += 2;
        }
        // Salta a la siguiente instrucción si la tecla en VX no es presionada
        else if ((opcode & 0x00FF) == 0x00A1)
        {
            if (keys[V[vx]] == 0)
                program_counter += 4;
            else
                program_counter += 2;
        }
        break;

    case 0xF000:
        vx = (opcode & 0x0F00) >> 8;

        switch (opcode & 0x00FF)
        {
        // Asigna a VX el valor del delay_timer
        case 0x0007:
            V[vx] = delay_timer;
            program_counter += 2;
            break;

        // Se espera a que se pulse una tecla y se asigne a VX
        case 0x000A:
            bool key_pressed = false;
            for (size_t i = 0; i < 16; ++i)
            {
                if (keys[i] != 0)
                {
                    V[vx] = i;
                    program_counter += 2;
                    key_pressed = true;
                    break;
                }
            }
            if (!key_pressed)
                return; // No avanzamos el PC ni salimos del ciclo hasta que se presione una tecla
            break;

        // Se asigna a delay_timer el valor de VX
        case 0x0015:
            delay_timer = V[vx];
            program_counter += 2;
            break;

        // Se asigna a sound_timer el valor de VX
        case 0x0018:
            sound_timer = V[vx];
            program_counter += 2;
            break;

        // Se añade el valor de VX a I
        case 0x001E:
            I += V[vx];
            program_counter += 2;
            break;

        // Asigna a I la localización del sprite de VX
        case 0x0029:
            I = V[vx] * 5;
            program_counter += 2;
            break;

        // Almacena la representación BCD de VX en memoria
        case 0x0033:
        {
            unsigned char value = V[vx];

            // Calcular y almacenar los dígitos BCD
            memory[I] = value / 100;           // Centena
            memory[I + 1] = (value / 10) % 10; // Decena
            memory[I + 2] = value % 10;        // Unidad
            program_counter += 2;
            break;
        }

        // Almacena desde V0 a VX en memoria, desde la dirección I. (I no se modifica)
        case 0x0055:
            for (size_t reg = 0; reg <= vx; reg++)
                memory[I + reg] = V[reg];
            program_counter += 2;
            break;

        // Carga desde V0 a VX con valores de la memoria, empezando desde I. (I no se modifica)
        case 0x0065:
            for (size_t reg = 0; reg <= vx; reg++)
                V[reg] = memory[I + reg];
            program_counter += 2;
            break;
        }
        break;

    default:
        std::cout << "Unknown opcode: " << opcode << std::endl;
        running = false;
    }
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
    for (size_t yline = 0; yline < N; yline++)
    {
        unsigned char data = memory[I + yline]; // Acceso a los bytes que se quieren pintar
        for (size_t xpix = 0; xpix < 8; xpix++)
        {
            if ((data & (0x80 >> xpix)) != 0)
            {
                // Calcular la posición en pantalla, con envolvimiento (modular)
                unsigned short posX = (x + xpix) % 64;
                unsigned short posY = (y + yline) % 32;

                if (screen[posX + (posY * 64)] == 1)
                    V[0xF] = 1; // Existe una colision

                screen[posX + (posY * 64)] ^= 1; // Nota: Las coordenadas son las que ofrece el opcode
            }
        }
    }
}
