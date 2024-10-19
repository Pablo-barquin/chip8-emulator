#include "cpu.h"

void Cpu::initialize()
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

	// Inicializamos memoria, display e input
	m_memory.initialize();
	m_input.reset();
	m_display.clear();
}

void Cpu::executeOpcode()
{
	// 1. Fetch: Leer el opcode de la memoria desde la dirección pc
	opcode = m_memory.readMemory(program_counter) << 8 | m_memory.readMemory(program_counter + 1);

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
	

	case 0xD000:
		vx = (opcode & 0x0F00) >> 8;
		vy = (opcode & 0x00F0) >> 4;

		m_display.drawSprite(V[vx], V[vy], (opcode & 0x000F));
		break;

	default:
		std::cout << "Unknown opcode: " << opcode << std::endl;
	}

	// Cada instrucción son 2 bytes, por lo que incrementamos el contador en 2
	program_counter += 2;
}

void Cpu::updateTimers()
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