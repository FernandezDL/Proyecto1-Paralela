<h1 align="center">
 🫧 Burbujas Protector de Pantalla 🫧 
</h1>



#### Descripción del Proyecto
Este proyecto consiste en un protector de pantalla que dibuja múltiples burbujas en movimiento utilizando la biblioteca SDL para los gráficos y OpenMP para la paralelización. Las burbujas se generan de forma pseudoaleatoria y rebotan contra los bordes de la ventana, creando un efecto visual dinámico.

## Características

- **Paralelización con OpenMP:** Utiliza múltiples hilos para mejorar el rendimiento del dibujo y actualización de las burbujas.

- **Uso de SDL2:** Para renderizado gráfico y manejo de eventos de la ventana.

- **Movimiento y colisión de burbujas:** Las burbujas se mueven libremente por la pantalla y rebotan contra los bordes.

- **Visualización de FPS:** Muestra los cuadros por segundo en la esquina superior izquierda de la ventana para monitorizar el rendimiento.


## 🛠️ Requisitos
Para ejecutar este proyecto, se necesita tener SDL2 y SDL2_ttf instalados en un sistema con sistema operativo Linux. También se debe asegurar de que OpenMP esté disponible para su uso en el compilador.

1. Clonar el repositorio

```bash
git clone https://github.com/FernandezDL/Proyecto1-Paralela
```

2. Navegar al directorio del proyecto y compilar el código:

```bash
g++ -o Burbujas main.cpp -fopenmp `sdl2-config --cflags --libs` -lSDL2_ttf
```

3. Ejecuta el programa

```bash
./Burbujas <cantidad_de_burbujas>
```
Reemplaza <cantidad_de_burbujas> con el número de burbujas que deseas generar.


## Estructura de Directorios

- src/: Contiene los archivos de código fuente.
- include/: Contiene los archivos de cabecera.
- assets/: Contiene recursos gráficos como fuentes.
- doc/: Documentación del proyecto y reportes de rendimiento.


## Contribuciones
Las contribuciones son bienvenidas. Si se desea mejorar el protector de pantalla o añadir nuevas características, por favor haz un fork del repositorio y envía un pull request.
