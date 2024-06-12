#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init("SDL Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, false)) {
        SDL_Log("Failed to initialize game");
        return -1;
    }

    game.run();
    game.clean();

    return 0;
}
