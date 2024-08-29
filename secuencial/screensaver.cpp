#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <cstdlib>  // Para usar atoi
#include <string>   // Para manejar cadenas
#include <sstream>  // Para convertir números a cadenas

// Dimensiones de la ventana
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Pattern {
    float x, y;
    float vx, vy;
    float size;
    float angle;
};

// Función para dibujar un patrón en una sección de la pantalla
void drawPattern(SDL_Renderer* renderer, const Pattern& p) {
    // Colores aleatorios para el patrón
    SDL_SetRenderDrawColor(renderer, rand() % 256, rand() % 256, rand() % 256, 255);

    // Coordenadas de los vértices del triángulo
    int x1 = p.x + p.size * cos(p.angle);
    int y1 = p.y + p.size * sin(p.angle);
    int x2 = p.x + p.size * cos(p.angle + 2 * M_PI / 3);
    int y2 = p.y + p.size * sin(p.angle + 2 * M_PI / 3);
    int x3 = p.x + p.size * cos(p.angle + 4 * M_PI / 3);
    int y3 = p.y + p.size * sin(p.angle + 4 * M_PI / 3);

    // Dibujar el triángulo
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

// Función para actualizar la posición y manejar colisiones
void updatePattern(Pattern& p) {
    // Actualizar la posición
    p.x += p.vx;
    p.y += p.vy;

    // Rebote en los bordes
    if (p.x < 0 || p.x > SCREEN_WIDTH) {
        p.vx = -p.vx;
    }
    if (p.y < 0 || p.y > SCREEN_HEIGHT) {
        p.vy = -p.vy;
    }
}

// Función para manejar colisiones entre patrones
void handleCollisions(Pattern& p1, Pattern& p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Verificar colisión
    if (distance < (p1.size + p2.size) / 2) {
        // Intercambiar velocidades para simular colisión elástica
        std::swap(p1.vx, p2.vx);
        std::swap(p1.vy, p2.vy);
    }
}

// Función para renderizar texto en la pantalla (FPS)
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};  // Blanco
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 100, 50, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 255)); // Fondo negro

    SDL_Rect rect = {10, 10, surface->w, surface->h};
    SDL_BlitSurface(surface, NULL, surface, &rect);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {x, y, 100, 50};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

int main(int argc, char* args[]) {
    // Verificar que se haya pasado el parámetro N
    if (argc < 2) {
        std::cerr << "Uso: " << args[0] << " N (cantidad de elementos a renderizar)" << std::endl;
        return -1;
    }

    // Convertir el argumento a entero
    int N = atoi(args[1]);
    if (N <= 0) {
        std::cerr << "N debe ser un número positivo." << std::endl;
        return -1;
    }

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Crear la ventana
    SDL_Window* window = SDL_CreateWindow("Kaleidoscope Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Error al crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Crear el renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Error al crear el renderizador: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Crear los patrones con posiciones y velocidades aleatorias
    Pattern patterns[N];
    for (int i = 0; i < N; ++i) {
        patterns[i].x = rand() % SCREEN_WIDTH;
        patterns[i].y = rand() % SCREEN_HEIGHT;
        patterns[i].vx = (rand() % 100 / 50.0f) - 1.0f; // Velocidades entre -1.0 y 1.0
        patterns[i].vy = (rand() % 100 / 50.0f) - 1.0f;
        patterns[i].size = 20.0f + rand() % 30; // Tamaño entre 20 y 50
        patterns[i].angle = rand() % 360 * M_PI / 180.0f;
    }

    // Variables para el cálculo de FPS
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    // Bucle principal
    bool running = true;
    SDL_Event event;
    while (running) {
        // Manejo de eventos
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Limpiar la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Actualizar posiciones y manejar colisiones
        for (int i = 0; i < N; ++i) {
            updatePattern(patterns[i]);

            // Dibujar cada patrón
            drawPattern(renderer, patterns[i]);

            // Verificar colisiones con otros patrones
            for (int j = i + 1; j < N; ++j) {
                handleCollisions(patterns[i], patterns[j]);
            }
        }

        // Incrementar el contador de frames
        frameCount++;

        // Calcular FPS cada segundo
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime >= 1000) {
            float fps = frameCount / ((currentTime - startTime) / 1000.0f);
            std::stringstream ss;
            ss << "FPS: " << fps;
            std::string fpsText = ss.str();

            // Reiniciar contador de tiempo y frame
            startTime = currentTime;
            frameCount = 0;

            // Renderizar texto con FPS
            renderText(renderer, fpsText, 10, 10);
        }

        // Actualizar la pantalla
        SDL_RenderPresent(renderer);

        // Esperar un pequeño tiempo para ver el cambio
        SDL_Delay(16); // Aproximadamente 60 FPS
    }

    // Limpiar recursos y salir
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
