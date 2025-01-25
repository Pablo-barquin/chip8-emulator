#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Crear una ventana
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", 
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                          640, 320, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Error al crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Crear un renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Error al crear el renderizador: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Dibujar algo en la pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
    SDL_RenderClear(renderer); // Limpiar pantalla
    SDL_RenderPresent(renderer); // Mostrar en pantalla

    // Esperar 3 segundos antes de cerrar
    SDL_Delay(3000);

    // Limpiar recursos y salir
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}