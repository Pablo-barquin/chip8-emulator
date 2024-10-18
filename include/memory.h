#ifndef MEMORY_H
#define MEMORY_H

#include "fontset.h"

class Memory {
public:
    Memory();
    void initialize();
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);

private:
    void loadFontset();
    unsigned char memory[4096]; // Memoria de 4KB
};

#endif
