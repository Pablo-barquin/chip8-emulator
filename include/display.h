#ifndef DISPLAY_H
#define DISPLAY_H

class Display
{
public:
    Display() {}
    void clear();
    void drawSprite(int x, int y, unsigned char sprite);

private:
    unsigned char screen[64 * 32]; // Pantalla de 64x32
};

#endif