#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <vector>

// Estructura para almacenar la información de una burbuja
struct Bubble {
    int x, y;
    int radius;
    int velX, velY; // Velocidades en X y Y
    SDL_Color color;
};

// Prototipo de la función DrawCircle
void DrawCircle(SDL_Renderer* renderer, int x, int y, int radius);

// Función para dibujar una burbuja con gradiente
void DrawBubble(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color baseColor) {
    // Dibujar burbuja con gradiente
    for (int r = radius; r > 0; --r) {
        SDL_SetRenderDrawColor(renderer, baseColor.r, baseColor.g, baseColor.b, baseColor.a - (radius - r) * 2);
        DrawCircle(renderer, x, y, r);
    }

    // Añadir reflejo en la parte superior izquierda
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150); // Blanco semitransparente
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

// Función para calcular la distancia entre dos puntos
float distance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

// Función para generar nuevas burbujas
void GenerateBubbles(std::vector<Bubble>& bubbles, int numBubbles, int windowWidth, int windowHeight) {
    for (int i = 0; i < numBubbles; ++i) {
        int radius = 20 + std::rand() % 30;

        int x = radius + std::rand() % (windowWidth - 2 * radius);
        int y = radius + std::rand() % (windowHeight - 2 * radius);

        // Generar velocidades aleatorias para la burbuja
        int velX = (std::rand() % 5) + 1;
        int velY = (std::rand() % 5) + 1;

        // Generar un color aleatorio para la burbuja
        Uint8 red = static_cast<Uint8>(100 + std::rand() % 156);  // Rango entre 100 y 255
        Uint8 green = static_cast<Uint8>(100 + std::rand() % 156);
        Uint8 blue = static_cast<Uint8>(100 + std::rand() % 156);
        SDL_Color bubbleColor = {red, green, blue, 128};

        bubbles.push_back({x, y, radius, velX, velY, bubbleColor});
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <cantidad de burbujas>" << std::endl;
        return -1;
    }

    int numBubbles = std::atoi(argv[1]);

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Error al inicializar SDL: %s", SDL_GetError());
        return -1;
    }

    const int windowWidth = 640;
    const int windowHeight = 480;

    // Crear la ventana
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

    // Crear el renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        SDL_Log("Error al crear el renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Semilla para generar números aleatorios
    std::srand(std::time(nullptr));

    // Crear un vector de burbujas
    std::vector<Bubble> bubbles;
    GenerateBubbles(bubbles, numBubbles, windowWidth, windowHeight);

    bool running = true;
    SDL_Event event;

    // Bucle principal
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Obtener la posición del mouse
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Limpiar pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Actualizar la posición de las burbujas y dibujarlas
        for (auto it = bubbles.begin(); it != bubbles.end(); ) {
            // Comprobar si el mouse está dentro de la burbuja
            if (distance(mouseX, mouseY, it->x, it->y) <= it->radius) {
                // Eliminar la burbuja (estalla)
                it = bubbles.erase(it);
                continue;
            }

            // Actualizar la posición
            it->x += it->velX;
            it->y += it->velY;

            // Rebotar en los bordes de la pantalla
            if (it->x - it->radius < 0 || it->x + it->radius > windowWidth) {
                it->velX = -it->velX;
            }
            if (it->y - it->radius < 0 || it->y + it->radius > windowHeight) {
                it->velY = -it->velY;
            }

            // Dibujar la burbuja
            DrawBubble(renderer, it->x, it->y, it->radius, it->color);

            ++it;
        }

        // Verificar si todas las burbujas estallaron
        if (bubbles.empty()) {
            GenerateBubbles(bubbles, numBubbles, windowWidth, windowHeight);
        }

        // Presentar lo que se ha dibujado
        SDL_RenderPresent(renderer);

        // Pequeño retraso para no sobrecargar la CPU
        SDL_Delay(16); // Aproximadamente 60 FPS
    }

    // Limpiar recursos
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
