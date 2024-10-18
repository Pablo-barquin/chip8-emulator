#include "input.h"

Input::Input()
{
    reset();
}

void Input::reset()
{
    // Inicializamos estado de las teclas
    for (int i = 0; i < 16; ++i)
        keys[i] = 0;
}

bool Input::isKeyPressed(unsigned char key)
{
    return keys[key] != 0;
}

void Input::pressKey(unsigned char key)
{
    keys[key] = 1;
}

void Input::releaseKey(unsigned char key)
{
    keys[key] = 0;
}
