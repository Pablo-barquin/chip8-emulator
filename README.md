# Emulador CHIP-8

![CHIP-8 Emulator](https://img.shields.io/badge/C%2B%2B-11-blue.svg) ![SDL2](https://img.shields.io/badge/SDL2-2.0-brightgreen.svg) ![Status](https://img.shields.io/badge/Status-In%20Development-orange.svg)

Este proyecto es un **emulador del sistema CHIP-8** desarrollado en C++ utilizando la biblioteca SDL2 en sistema operativo Windows. El CHIP-8 es una máquina virtual simple que se utilizaba para videojuegos en los años 70.

La idea de este repositorio es aprender sobre la creación de emuladores básicos y explorar conceptos fundamentales como el uso de operaciones con bits, incluidos desplazamientos, manipulación de registros o optimización del uso de la memoria.

## Características 🌟

- **Compatibilidad completa** con las instrucciones del CHIP-8.
- Renderizado escalado con SDL2.
- Soporte para entrada del teclado mapeado a las teclas originales del CHIP-8.
- Implementación de temporizadores de sonido y retraso.
- Ejecución de ROMs de juegos clásicos de CHIP-8.

## Requisitos 🛠️

Antes de comenzar, asegúrate de tener lo siguiente instalado:

- **C++11** (o superior).
- **CMake 3.10+**
- **SDL2** (debe estar configurado en `C:/SDL2` o ajustar el archivo `CMakeLists.txt` si está en otra ubicación).
- **MinGW64** (para compilación en Windows).

## Instalación 📥

1. Clona este repositorio:

   ```bash
   git clone https://github.com/tuusuario/emulador-chip8.git
   cd emulador-chip8
   ```

2. Configura el entorno de compilación:

   ```bash
   mkdir build && cd build
   cmake ..
   ```

3. Compila el emulador:

   ```bash
   cmake --build .
   ```

## Uso 🎮

Ejecuta el emulador proporcionando una ROM como argumento:

```bash
CHIP8_Emulator.exe <ruta_a_la_rom>
```

Las teclas del teclado se mapean a las teclas originales del CHIP-8 de la siguiente manera:

| Tecla CHIP-8 | Tecla Física |
|--------------|---------------|
| 1 | 1 |
| 2 | 2 |
| 3 | 3 |
| C | 4 |
| 4 | Q |
| 5 | W |
| 6 | E |
| D | R |
| 7 | A |
| 8 | S |
| 9 | D |
| E | F |
| A | Z |
| 0 | X |
| B | C |
| F | V |

## Estructura del Proyecto 🗂️

```
/
|-- src/               # Archivos fuente principales
|-- include/           # Archivos de cabecera
|-- build/             # Directorio de construcción (ignorado por git)
|-- CMakeLists.txt     # Archivo de configuración para CMake
|-- .gitignore         # Evita la subida de ficheros innecesarios al repositorio
|-- README.md          # Descripción del proyecto
```

## Contribución 🤝

1. Haz un fork del repositorio.
2. Crea una nueva rama:

   ```bash
   git checkout -b feature/nueva-funcionalidad
   ```

3. Realiza tus cambios y haz commit:

   ```bash
   git commit -m "Agrega nueva funcionalidad"
   ```

4. Envía un pull request.

## Licencia ⚖️

Este proyecto está bajo la licencia MIT. Consulta el archivo [LICENSE](LICENSE) para más información. 📝

## Recursos Adicionales 🌐

- [Documentación de CHIP-8](https://en.wikipedia.org/wiki/CHIP-8)
- [SDL2](https://www.libsdl.org/)

