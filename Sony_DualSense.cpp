#include <SDL2/SDL.h>
#include <iostream>
//==readme.md
// get the library
    //> sudo apt-get install libsdl2-dev

//g++ -o controller controller.cpp `sdl2-config --cflags --libs`



int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Wait for a controller
    std::cout << "Waiting for controller ..." << std::endl;
    while (SDL_NumJoysticks() < 1) {
        SDL_Delay(500);  // Wait and try again
    }

    // Open the first available controller
    SDL_GameController *controller = SDL_GameControllerOpen(0);
    if (controller == nullptr) {
        std::cerr << "Couldn't open controller: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Game loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_CONTROLLERBUTTONDOWN:
                    std::cout << "Button " << (int)event.cbutton.button << " pressed" << std::endl;
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    std::cout << "Button " << (int)event.cbutton.button << " released" << std::endl;
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    std::cout << "Axis " << (int)event.caxis.axis << " value: " << event.caxis.value << std::endl;
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
    }

    // Clean up
    SDL_GameControllerClose(controller);
    SDL_Quit();

    return 0;
}
