#include "Game.h"
#include <cstdlib> // Dodane do obsługi funkcji rand()
#include <ctime>   // Dodane do inicjalizacji generatora losowego
#include <sstream>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), floorTexture(nullptr), playerTexture(nullptr), originalPlayerTexture(nullptr), invincibleTexture(nullptr), obstacleTexture(nullptr), coinTexture(nullptr), playerVelocity(0), isJumping(false), invincible(false), score(0), coins(0), lives(3), highScore(0) {
    playerRect = { 320, 448 - 32, 32, 32 }; // Startowa pozycja gracza
    startTime = SDL_GetTicks(); // Initialize start time
    coinRect = { 0, 0, 32, 32 }; // zaladuj pozycja i rozmiar pieniazka
    srand(static_cast<unsigned int>(time(nullptr))); //wygenruj losowa pozucja dla pienazka
}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    screenWidth = width;
    screenHeight = height;

    int flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() == -1) {
        SDL_Log("TTF could not initialize: %s", TTF_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (!window) {
        SDL_Log("Window could not be created: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Renderer could not be created: %s", SDL_GetError());
        return false;
    }

    // Zakaduj SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("SDL_image could not initialize: %s", IMG_GetError());
        return false;
    }

    //Zakaduj wyglad dolnej platformy
    floorTexture = loadTexture("../assets/floor.png");
    if (!floorTexture) {
        SDL_Log("Failed to load floor texture: %s", IMG_GetError());
        return false;
    }

    // Zakaduj wyglad gornej platformy
    roofTexture = loadTexture("../assets/roof.png");
    if (!roofTexture) {
        SDL_Log("Failed to load roof texture: %s", IMG_GetError());
        return false;
    }

    // Zakaduj wyglad gracza
    playerTexture = loadTexture("../assets/player.png");
    if (!playerTexture) {
        SDL_Log("Failed to load player texture: %s", IMG_GetError());
        return false;
    }
    originalPlayerTexture = playerTexture; // Zapisz oryginalną teksturę gracza

    // Zakaduj wyglad ochronnego gracza
    invincibleTexture = loadTexture("../assets/enemy.png");
    if (!invincibleTexture) {
        SDL_Log("Failed to load invincible texture: %s", IMG_GetError());
        return false;
    }

    // Zakaduj wyglad przeszkod
    obstacleTexture = loadTexture("../assets/brick.png");
    if (!obstacleTexture) {
        SDL_Log("Failed to load obstacle texture: %s", IMG_GetError());
        return false;
    }

    // Zakaduj wyglad pieniazka
    coinTexture = loadTexture("../assets/coin.png");
    if (!coinTexture) {
        SDL_Log("Failed to load coin texture: %s", IMG_GetError());
        return false;
    }

    // Rozpocznij generowanie przeszkod
    for (int i = 0; i < 5; ++i) {
        int obstacleHeight = minObstacleHeight + rand() % (maxObstacleHeight - minObstacleHeight);
        bool onTop = rand() % 2;
        SDL_Rect obstacle;
        if (onTop) {
            obstacle = { 640 + i * 300, 64, 32, obstacleHeight };
        } else {
            obstacle = { 640 + i * 300, 448 - obstacleHeight, 32, obstacleHeight };
        }
        obstacles.push_back(obstacle);
    }

    // Initialize coin position
    coinRect.x = 640 + rand() % (screenWidth - 32);
    coinRect.y = minCoinHeight + rand() % (maxCoinHeight - minCoinHeight);

    isRunning = true;
    return true;
}

SDL_Texture* Game::loadTexture(const std::string& path) {
    SDL_Texture* texture = nullptr;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        SDL_Log("Unable to load image %s! SDL_image Error: %s", path.c_str(), IMG_GetError());
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    return texture;
}

SDL_Texture* Game::renderText(const std::string& message, const std::string& fontFile, SDL_Color color, int fontSize) {
    TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        SDL_Log("Failed to create text surface: %s", TTF_GetError());
        TTF_CloseFont(font);
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(10);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                isJumping = true;
            }
        } else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                isJumping = false;
            }
        }
    }
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    if (isJumping) {
        playerVelocity = jumpStrength; // Rusz sie do gory kiedy spacja jest klikneta
    } else {
        playerVelocity += gravity; // Dodaj grawitacja jak spacja nie jest klinieta
    }

    playerRect.y += static_cast<int>(playerVelocity);

    // Kolizja z gora i dolna patforma
    if (playerRect.y <= 64) {
        playerRect.y = 64;
        playerVelocity = bounceStrength / 2; // Odbij się od wysokości 64 czyli gornej pozycji
    }
    if (playerRect.y >= 448 - playerRect.h) {
        playerRect.y = 448 - playerRect.h;
        playerVelocity = -bounceStrength; // Odbij się od dolnej podłogi
    }

    // Poruszanie sie przeszkod i sprawdzenie kolizji
    for (auto& obstacle : obstacles) {
        obstacle.x -= obstacleSpeed;
        if (obstacle.x + obstacle.w < 0) {
            obstacle.x = screenWidth;
            obstacle.h = minObstacleHeight + rand() % (maxObstacleHeight - minObstacleHeight);
            bool onTop = rand() % 2;
            if (onTop) {
                obstacle.y = 64;
            } else {
                obstacle.y = 448 - obstacle.h;
            }
        }
        if (!invincible && SDL_HasIntersection(&playerRect, &obstacle)) {
            lives--;
            if (lives <= 0) {
                // Akualizacja najwyzszego wyniku
                if (score > highScore) {
                    highScore = score;
                }
                // Resetowanie gry po utracie 3 zycia
                lives = 3;
                score = 0;
                coins = 0;
                startTime = SDL_GetTicks();
                playerRect = { 320, 448 - 32, 32, 32 }; // Reset player position
                // Reinitialize obstacles and coin
                for (auto& obstacle : obstacles) {
                    obstacle.x = screenWidth + (rand() % screenWidth);
                    obstacle.h = minObstacleHeight + rand() % (maxObstacleHeight - minObstacleHeight);
                    bool onTop = rand() % 2;
                    if (onTop) {
                        obstacle.y = 64;
                    } else {
                        obstacle.y = 448 - obstacle.h;
                    }
                }
                coinRect.x = screenWidth + rand() % (screenWidth - 32);
                coinRect.y = minCoinHeight + rand() % (maxCoinHeight - minCoinHeight);
            } else {
                invincible = true;
                invincibleStartTime = currentTime;
                playerTexture = invincibleTexture; // Zmień teksturę gracza na czas ochronny
                playerRect = { 320, 448 - 32, 32, 32 }; // Resetowanie pozycji gracza
            }
        }
    }

    // Move coin
    coinRect.x -= obstacleSpeed;
    if (coinRect.x + coinRect.w < 0) {
        coinRect.x = screenWidth;
        coinRect.y = minCoinHeight + rand() % (maxCoinHeight - minCoinHeight);
    }

    // Check for collision with coin
    if (!invincible && SDL_HasIntersection(&playerRect, &coinRect)) {
        coins++;
        coinRect.x = screenWidth;
        coinRect.y = minCoinHeight + rand() % (maxCoinHeight - minCoinHeight);
    }

    // Check if invincibility period is over
    if (invincible && currentTime - invincibleStartTime >= 3000) {
        invincible = false;
        playerTexture = originalPlayerTexture; // Przywróć oryginalną teksturę gracza
    }

    // Update score
    score = (currentTime - startTime) / 1000 + coins * 10;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Czarny kolor tła
    SDL_RenderClear(renderer);

    // Renderowanie podłogi
    int brickWidth = 32;
    int brickHeight = 32;
    for (int x = 0; x <= screenWidth; x += brickWidth) {
        SDL_Rect dstRect = { x, 448, brickWidth, brickHeight }; // Pozycja y = 448, ponieważ okno ma wysokość 480 pikseli
        SDL_RenderCopy(renderer, floorTexture, nullptr, &dstRect);
    }
    for (int x = 0; x <= screenWidth; x += brickWidth) {
        SDL_Rect dstRect = { x, 32, brickWidth, brickHeight };
        SDL_RenderCopy(renderer, roofTexture, nullptr, &dstRect);
    }

    // Renderowanie przeszkód
    for (const auto& obstacle : obstacles) {
        SDL_RenderCopy(renderer, obstacleTexture, nullptr, &obstacle);
    }

    // Renderowanie monety
    SDL_RenderCopy(renderer, coinTexture, nullptr, &coinRect);

    // Renderowanie gracza
    SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);

    // Renderowanie informacji
    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
    std::stringstream ss;
    ss << "Time: " << elapsedTime << "  Coins: " << coins << "  Lives: " << lives << "  Score: " << score << "  Highscore: " << highScore;
    SDL_Color textColor = { 255, 255, 255, 255 }; // Biały
    SDL_Texture* textTexture = renderText(ss.str(), "/System/Library/Fonts/Supplemental/Arial.ttf", textColor, 24);
    if (textTexture) {
        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
        SDL_Rect textRect = { (screenWidth - textWidth) / 2, 0, textWidth, textHeight };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyTexture(floorTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(obstacleTexture);
    SDL_DestroyTexture(coinTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}