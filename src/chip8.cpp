#include "chip8.h"
#include <fstream>

#include <unordered_map>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 255);

Chip8::Chip8()
{
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Crear la ventana
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "Error al crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // Crear el renderizador
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Error al crear el renderizador: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    initialize_CPU();
    initialize_Display();
    initialize_Input();
    initialize_Memory();
}

Chip8::~Chip8()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
    SDL_Event event;

    while (running)
    {
        // Manejar eventos de SDL
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                handleInput(event);
            }
        }

        executeOpcode();
        updateTimers();
        render();

        SDL_Delay(2); // Retardo para limitar la velocidad de ejecución del emulador
    }
}

void Chip8::handleInput(const SDL_Event& event)
{
    // Mapeo del teclado físico a las teclas del CHIP-8
    static const std::unordered_map<SDL_Keycode, int> keyMap = {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
        {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
        {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
        {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF}
    };

    auto it = keyMap.find(event.key.keysym.sym);
    if (it != keyMap.end())
    {
        int chip8Key = it->second;
        keys[chip8Key] = (event.type == SDL_KEYDOWN) ? 1 : 0; // Actualiza el estado de la tecla
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
    std::cout << "Ejecutando opcode: 0x" << std::hex << opcode << std::endl;

    program_counter += 2; // Incrementar el contador de programa antes de ejecutar la instrucción

    uint8_t vx;
    uint8_t vy;

    // Decode opcode
    switch (opcode & 0xF000)
    {
    case 0x0000:
        // Limpia la pantalla
        if ((opcode & 0x00FF) == 0x0E0)
        {
            initialize_Display();
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
            V[0xF] = 0; // VF se pone a 0
            break;

        // Asignar valor de (VX and VY) a VX
        case 0x0002:
            V[vx] &= V[vy];
            V[0xF] = 0; // VF se pone a 0
            break;

        // Asignar valor de (VX xor VY) a VX
        case 0x0003:
            V[vx] ^= V[vy];
            V[0xF] = 0; // VF se pone a 0
            break;

        // Suma VY a VX, con acarreo
        case 0x0004:
        {
            uint16_t sum = V[vx] + V[vy];
            V[vx] = sum & 0xFF;
            V[0xF] = (sum > 0xFF) ? 1 : 0;
            break;
        }

        // Resta VY a VX, con acarreo
        case 0x0005:
        {
            // Determinar el carry flag antes de modificar VX
            uint8_t carry = (V[vx] >= V[vy]) ? 1 : 0;

            // Realizar la resta
            V[vx] -= V[vy];

            // Asignar acarreo a VF
            V[0xF] = carry;
            break;
        }
        
        // Desplazamiento a derecha de 1 a VX (original CHIP-8)
        case 0x0006:
        {
            V[vx] = V[vy];
            uint8_t carry = V[vx] & 0x1; // Guardar el bit menos significativo en VF
            V[vx] >>= 1;
            V[0xF] = carry;
            break;
        }

        // Resta VX a VY, con acarreo
        case 0x0007:
        {
            // Determinar el carry flag antes de modificar VX
            uint8_t carry = (V[vy] >= V[vx]) ? 1 : 0;

            // Realizar la resta inversa
            V[vx] = V[vy] - V[vx];

            // Asignar acarreo a VF
            V[0xF] = carry;
            break;
        }
        
        // Desplazamiento a izquierda de 1 a VX (original CHIP-8)
        case 0x000E:
        {
            V[vx] = V[vy];
            uint8_t carry = (V[vx] & 0x80) >> 7; // Guardar el bit más significativo en VF
            V[vx] <<= 1;
            V[0xF] = carry;
            break;
        }

        default:
            std::cout << "Unknown opcode: " << opcode << std::endl;
            running = false;
        }
        break;

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
    {
        vx = (opcode & 0x0F00) >> 8;
        uint8_t random_number = dis(gen); // Generar número aleatorio
        V[vx] = random_number & (opcode & 0x00FF);
        break;
    }

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
            if (keys[V[vx]])
                program_counter += 2;
        }
        // Salta a la siguiente instrucción si la tecla en VX no es presionada
        else if ((opcode & 0x00FF) == 0x00A1)
        {
            if (!keys[V[vx]])
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
            break;

        // Se espera a que se pulse una tecla y se asigne a VX
        case 0x000A:
        {
            bool key_pressed = false;
            for (size_t i = 0; i < 16; ++i)
            {
                if (keys[i] != 0)
                {
                    V[vx] = i;
                    key_pressed = true;
                    break;
                }
            }
            if (!key_pressed)
                program_counter -= 2; // No avanzamos el PC hasta que se presione una tecla
            break;
        }

        // Se asigna a delay_timer el valor de VX
        case 0x0015:
            delay_timer = V[vx];
            break;

        // Se asigna a sound_timer el valor de VX
        case 0x0018:
            sound_timer = V[vx];
            break;

        // Se añade el valor de VX a I
        case 0x001E:
            I += V[vx];
            break;

        // Asigna a I la localización del sprite de VX
        case 0x0029:
            I = V[vx] * 5; // Cada fuente tiene 5 bytes
            break;

        // Almacena la representación BCD de VX en memoria
        case 0x0033:
        {
            uint8_t value = V[vx];

            // Calcular y almacenar los dígitos BCD
            memory[I] = value / 100;           // Centena
            memory[I + 1] = (value / 10) % 10; // Decena
            memory[I + 2] = value % 10;        // Unidad
            break;
        }

        // Almacena desde V0 a VX en memoria, desde la dirección I. (I se modifica, siguiendo los CHIP8 originales)
        case 0x0055:
            for (size_t reg = 0; reg <= vx; reg++)
                memory[I + reg] = V[reg];
                I += vx + 1;
            break;

        // Carga desde V0 a VX con valores de la memoria, empezando desde I. (I se modifica, siguiendo los CHIP8 originales)
        case 0x0065:
            for (size_t reg = 0; reg <= vx; reg++)
                V[reg] = memory[I + reg];
                I += vx + 1;
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

void Chip8::render()
{
    // Establecer el color de fondo (negro)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Dibujar los píxeles de la pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (screen[x + (y * 64)] == 1)
            {
                SDL_Rect pixel = {x * 10, y * 10, 10, 10}; // Escalar cada píxel a 10x10
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer); // Mostrar el contenido en la ventana
}


void Chip8::drawSprite(uint8_t x, uint8_t y, uint8_t N)
{
    V[0xF] = 0; // Reinicia el flag de colisión

    if (x > 63) x %= 64; // Wrapping 
    if (y > 31) y %= 32; // Wrapping

    for (size_t yline = 0; yline < N; yline++)
    {
        uint8_t posY = y + yline;
        if (posY >= 32) break; // Clipping

        uint8_t data = memory[I + yline]; // Obtener la línea del sprite
        for (size_t xpix = 0; xpix < 8; xpix++)
        {
            uint8_t posX = x + xpix;
            if (posX >= 64) break; // Clipping

            if ((data & (0x80 >> xpix)) != 0) // Si el bit actual es 1
            {
                if (screen[posX + (posY * 64)] == 1)
                    V[0xF] = 1; // Colisión detectada

                screen[posX + (posY * 64)] ^= 1; // Alternar pixel
                std::cout << "Pixel modificado: PosX=" << posX << ", PosY=" << posY << ", Estado=" << (int)screen[posX + (posY * 64)] << std::endl;
            }
        }
    }
}

void Chip8::loadGame(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return;
    }

    std::streampos size = file.tellg();
    std::cout << "Cargando ROM de : " << size << " bytes." << std::endl;

    char* buffer = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    for (long i = 0; i < size; ++i)
    {
        memory[0x200 + i] = buffer[i];
    }

    delete[] buffer;
    std::cout << "ROM cargada correctamente en memoria." << std::endl;
}

