#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class Game {
public:
    Game();
    ~Game();
    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void run();
    void handleEvents();
    void update();
    void render();
    void clean();
    SDL_Texture* loadTexture(const std::string& path);
    SDL_Texture* renderText(const std::string& message, const std::string& fontFile, SDL_Color color, int fontSize);

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* floorTexture;
    SDL_Texture* roofTexture;
    SDL_Texture* playerTexture;
    SDL_Texture* originalPlayerTexture;
    SDL_Texture* invincibleTexture;
    SDL_Texture* obstacleTexture;
    SDL_Texture* coinTexture;
    int screenWidth{};
    int screenHeight{};
    SDL_Rect playerRect;
    SDL_Rect coinRect; // Pozycja i rozmiar monety
    float playerVelocity;
    const float gravity = 0.5f; // Zmniejszona siła przyciągania
    const int jumpStrength = -7;
    const int bounceStrength = 10; // Dodana siła odbicia
    bool isJumping;
    bool invincible; // Zmienna oznaczająca stan niezniszczalności
    Uint32 invincibleStartTime; // Czas rozpoczęcia stanu niezniszczalności

    std::vector<SDL_Rect> obstacles;
    const int obstacleSpeed = 4;
    const int minObstacleHeight = 64;
    const int maxObstacleHeight = 160;

    Uint32 startTime;
    int score;
    int coins;
    int lives;
    int highScore;

    int minCoinHeight = 64;
    int maxCoinHeight = 416;
    int coinDirection;
};

#endif // GAME_H
