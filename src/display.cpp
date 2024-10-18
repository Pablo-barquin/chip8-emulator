#include "display.h"

Display::Display()
{
    clear();
}

void Display::clear()
{
    // Limpiamos la pantalla
    for (int i = 0; i < 64 * 32; ++i)
        screen[i] = 0;
}

void Display::drawSprite(int x, int y, unsigned char sprite)
{
}
