#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>

// Estructura para almacenar la información de una burbuja
struct Bubble {
    int x, y;
    int radius;
    int velX, velY;
    SDL_Color color;
};

// Prototipo de la función DrawCircle
void DrawCircle(SDL_Renderer* renderer, int x, int y, int radius);

void DrawBubble(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color baseColor) {
    for (int r = radius; r > 0; --r) {
        SDL_SetRenderDrawColor(renderer, baseColor.r, baseColor.g, baseColor.b, baseColor.a - (radius - r) * 2);
        DrawCircle(renderer, x, y, r);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
    DrawCircle(renderer, x - radius / 3, y - radius / 3, radius / 4);
}

void DrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    int offsetX = 0;
    int offsetY = radius;
    int d = radius - 1;

    while (offsetY >= offsetX) {
        SDL_RenderDrawPoint(renderer, x + offsetX, y + offsetY);
        SDL_RenderDrawPoint(renderer, x + offsetY, y + offsetX);
        SDL_RenderDrawPoint(renderer, x - offsetX, y + offsetY);
        SDL_RenderDrawPoint(renderer, x - offsetY, y + offsetX);
        SDL_RenderDrawPoint(renderer, x + offsetX, y - offsetY);
        SDL_RenderDrawPoint(renderer, x + offsetY, y - offsetX);
        SDL_RenderDrawPoint(renderer, x - offsetX, y - offsetY);
        SDL_RenderDrawPoint(renderer, x - offsetY, y - offsetX);

        if (d >= 2 * offsetX) {
            d -= 2 * offsetX + 1;
            offsetX++;
        } else if (d < 2 * (radius - offsetY)) {
            d += 2 * offsetY - 1;
            offsetY--;
        } else {
            d += 2 * (offsetY - offsetX - 1);
            offsetY--;
            offsetX++;
        }
    }
}

float distance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

void GenerateBubbles(std::vector<Bubble>& bubbles, int numBubbles, int windowWidth, int windowHeight) {
    for (int i = 0; i < numBubbles; ++i) {
        int radius = 20 + std::rand() % 30;

        int x = radius + std::rand() % (windowWidth - 2 * radius);
        int y = radius + std::rand() % (windowHeight - 2 * radius);

        int velX = (std::rand() % 5) + 1;
        int velY = (std::rand() % 5) + 1;

        Uint8 red = static_cast<Uint8>(100 + std::rand() % 156);
        Uint8 green = static_cast<Uint8>(100 + std::rand() % 156);
        Uint8 blue = static_cast<Uint8>(100 + std::rand() % 156);
        SDL_Color bubbleColor = {red, green, blue, 128};

        bubbles.push_back({x, y, radius, velX, velY, bubbleColor});
    }
}

// Función para mostrar el texto de FPS en la pantalla
void RenderFPS(SDL_Renderer* renderer, TTF_Font* font, int fps) {
    SDL_Color white = {255, 255, 255, 255};
    std::string fpsText = "FPS: " + std::to_string(fps);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, fpsText.c_str(), white);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = 10; // Posición X
    messageRect.y = 10; // Posición Y
    messageRect.w = 100; // Ancho del texto
    messageRect.h = 50;  // Alto del texto

    SDL_RenderCopy(renderer, message, nullptr, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <cantidad de burbujas>" << std::endl;
        return -1;
    }

    int numBubbles = std::atoi(argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Error al inicializar SDL: %s", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        SDL_Log("Error al inicializar SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    const int windowWidth = 640;
    const int windowHeight = 480;

    SDL_Window* window = SDL_CreateWindow("Burbujas",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          windowWidth, windowHeight,
                                          SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        SDL_Log("Error al crear la ventana: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        SDL_Log("Error al crear el renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

//    TTF_Font* font = TTF_OpenFont("font.otf", 24);
    TTF_Font* font = TTF_OpenFont("/mnt/c/Users/Jennifer/OneDrive/Escritorio/Proyecto1-Paralela/secuencial/font.otf", 24);

    if (!font) {
        SDL_Log("Error al cargar la fuente: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    std::srand(std::time(nullptr));

    std::vector<Bubble> bubbles;
    GenerateBubbles(bubbles, numBubbles, windowWidth, windowHeight);

    bool running = true;
    SDL_Event event;

    // Medición del tiempo de ejecución total
    auto startExecutionTime = std::chrono::high_resolution_clock::now();
    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    int fps = 0;
    int totalTime = 0; 

    while (running) {
        auto frameStart = std::chrono::high_resolution_clock::now(); // Inicio del frame actual
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Paralelización con OpenMP en el ciclo de burbujas
        #pragma omp parallel for
        for (int i = 0; i < bubbles.size(); ++i) {
            if (distance(mouseX, mouseY, bubbles[i].x, bubbles[i].y) <= bubbles[i].radius) {
                #pragma omp critical
                {
                    bubbles.erase(bubbles.begin() + i);
                }
                continue;
            }

            bubbles[i].x += bubbles[i].velX;
            bubbles[i].y += bubbles[i].velY;

            if (bubbles[i].x - bubbles[i].radius < 0 || bubbles[i].x + bubbles[i].radius > windowWidth) {
                bubbles[i].velX = -bubbles[i].velX;
            }
            if (bubbles[i].y - bubbles[i].radius < 0 || bubbles[i].y + bubbles[i].radius > windowHeight) {
                bubbles[i].velY = -bubbles[i].velY;
            }

            #pragma omp critical
            {
                DrawBubble(renderer, bubbles[i].x, bubbles[i].y, bubbles[i].radius, bubbles[i].color);
            }
        }

        if (bubbles.empty()) {
            GenerateBubbles(bubbles, numBubbles, windowWidth, windowHeight);
        }

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        if (elapsedTime >= 1000) {
            fps = frameCount / (elapsedTime / 1000.0f);
            frameCount = 0;
            startTime = currentTime;
        }

        RenderFPS(renderer, font, fps);

        SDL_RenderPresent(renderer);

        auto frameEnd = std::chrono::high_resolution_clock::now(); // Fin del frame actual
        int frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        totalTime += frameDuration; // Acumular el tiempo total de renderizado


        SDL_Delay(16);
    }

    auto endExecutionTime = std::chrono::high_resolution_clock::now();
    auto totalExecutionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endExecutionTime - startExecutionTime).count();

    std::cout << "Tiempo total de ejecución: " << totalExecutionTime << " ms\n";
    std::cout << "Tiempo promedio por cuadro: " << (totalTime / frameCount) << " ms\n";

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}