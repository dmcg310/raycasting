# raycasting

A rendering technique used to create a 3D perspective in a 2D map. Most notably [Wolfenstein 3D](https://en.wikipedia.org/wiki/Wolfenstein_3D).

## Concepts

### Rays

- Imaginary lines extending from the player's point of view (POV) into the game world.

### Intersection Calculation

- Determines where rays intersect with walls.

### Vertical Strips

- Display walls based on ray intersections, creating a 3D illusion.

## How to run the C version

1. Clone the repository.
2. Navigate to the 'raycasting-c' directory: `cd raycasting-c/`.
3. Then run the Makefile: `make` this will compile the program and run it.

### References

- [P5.js](https://p5js.org/)
- [SDL](https://www.libsdl.org/)
