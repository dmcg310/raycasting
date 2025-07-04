# Raycasting

A 3D rendering technique implementation that creates pseudo-3D environments from 2D maps using ray casting algorithms. This project includes C and JavaScript implementations demonstrating the core concepts behind early 3D games like Wolfenstein 3D.

## Overview

Raycasting works by casting rays from the player's viewpoint into the game world to determine wall intersections. The distance to these intersections is used to calculate wall heights, creating the illusion of 3D depth on a 2D screen.

### Key Concepts

- **Ray Casting**: Projects imaginary rays from the player's position to detect wall collisions
- **Grid-based World**: Uses a 2D array to represent walls and open spaces
- **Perspective Projection**: Converts ray distances to wall heights for 3D appearance
- **Field of View**: Simulates human vision with a 60-degree viewing angle

## Implementations

### C Version (SDL2)

Located in `raycasting-c/` directory.

**Prerequisites:**
- GCC compiler
- SDL2 development libraries

**Installation:**

macOS (with Homebrew):
```bash
brew install sdl2
```

Ubuntu/Debian:
```bash
sudo apt-get install libsdl2-dev
```

**Running:**
```bash
cd raycasting-c/
make
```

**Controls:**
- Arrow keys or WASD: Move and turn
- ESC or Q: Quit

### JavaScript Version (P5.js)

Located in `raycasting-js/` directory.

**Running:**
Simply open `index.html` in a web browser.

**Controls:**
- Arrow keys: Move and turn

## Features

- Real-time 3D rendering using 2D raycasting
- Player movement with collision detection
- Minimap showing player position and rays
- Different wall shading for depth perception
- Smooth movement and rotation

## Technical Details

The raycasting algorithm works by:

1. Casting rays at regular intervals across the player's field of view
2. For each ray, finding intersections with both horizontal and vertical grid lines
3. Determining which intersection is closer to calculate the wall distance
4. Converting distances to wall heights using perspective projection
5. Rendering vertical strips to create the 3D effect

## Performance

- C version: Optimized for performance using SDL2's hardware acceleration
- JavaScript version: Runs at 60 FPS in modern browsers using P5.js

## References

- [Wolfenstein 3D](https://en.wikipedia.org/wiki/Wolfenstein_3D)
- [P5.js](https://p5js.org/)
- [SDL2](https://www.libsdl.org/)
