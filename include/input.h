#ifndef INPUT_H
#define INPUT_H

class Input
{
public:
    Input();
    void reset();
    bool isKeyPressed(unsigned char key);
    void pressKey(unsigned char key);
    void releaseKey(unsigned char key);

private:
    unsigned char keys[16]; // Teclado hexadecimal
};

#endif