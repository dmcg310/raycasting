const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;
const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;
const FOV_ANGLE = 60 * (Math.PI / 180);
const WALL_STRIP_WIDTH = 1;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;

class Map {
  constructor() {
    this.grid = [
      [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
      [1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
      [1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
      [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
      [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    ];
  }

  hasWallAt(x, y) {
    if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) return true;

    let mapGridIndexX = Math.floor(x / TILE_SIZE);
    let mapGridIndexY = Math.floor(y / TILE_SIZE);

    return this.grid[mapGridIndexY][mapGridIndexX] != 0;
  }
  render() {
    for (let i = 0; i < MAP_NUM_ROWS; i++) {
      for (let j = 0; j < MAP_NUM_COLS; j++) {
        let tileX = j * TILE_SIZE;
        let tileY = i * TILE_SIZE;
        let tileColor = this.grid[i][j] == 1 ? "#222" : "#fff";

        stroke("#222");
        fill(tileColor);
        rect(tileX, tileY, TILE_SIZE, TILE_SIZE);
      }
    }
  }
}

class Player {
  constructor() {
    this.x = WINDOW_WIDTH / 2;
    this.y = WINDOW_HEIGHT / 2;
    this.radius = 3;
    this.turnDirection = 0; // -1 if left, +1 if right
    this.walkDirection = 0; // -1 if back, +1 if front
    this.rotationAngle = Math.PI / 2;
    this.moveSpeed = 2.0;
    this.rotationSpeed = 2 * (Math.PI / 180);
  }

  update() {
    this.rotationAngle += this.turnDirection * this.rotationSpeed;

    let moveStep = this.walkDirection * this.moveSpeed;
    let newPlayerX = this.x + Math.cos(this.rotationAngle) * moveStep;
    let newPlayerY = this.y + Math.sin(this.rotationAngle) * moveStep;

    if (!grid.hasWallAt(newPlayerX, newPlayerY)) {
      this.x = newPlayerX;
      this.y = newPlayerY;
    }
  }

  render() {
    noStroke();
    fill("red");
    circle(this.x, this.y, this.radius);
    /*
        stroke("red");
        line(
            this.x,
            this.y,
            this.x + Math.cos(this.rotationAngle) * 30,
            this.y + Math.sin(this.rotationAngle) * 30
    );
    */
  }
}

class Ray {
  constructor(rayAngle) {
    this.rayAngle = normaliseAngle(rayAngle);
    this.wallHitX = 0;
    this.wallHitY = 0;
    this.distance = 0;
    this.wasHitVertical = false;

    this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
    this.isRayFacingUp = !this.isRayFacingDown;

    this.isRayFacingRight =
      this.rayAngle < 0.5 * Math.PI || this.rayAngle > 1.5 * Math.PI;
    this.isRayFacingLeft = !this.isRayFacingRight;
  }

  cast(columnId) {
    let xintercept, yintercept;
    let xstep, ystep;

    /* HORIZONTAL RAY-GRID INTERSECTION CODE */

    let foundHorzWallHit = false;
    let horzWallHitX = 0;
    let horzWallHitY = 0;

    // find the y-coordinate of the closest horizontal grid intersenction
    yintercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += this.isRayFacingDown ? TILE_SIZE : 0;

    // find the x-coordinate of the closest horizontal grid intersection
    xintercept = player.x + (yintercept - player.y) / Math.tan(this.rayAngle);

    // calculate the increment xstep and ystep
    ystep = TILE_SIZE;
    ystep *= this.isRayFacingUp ? -1 : 1;

    xstep = TILE_SIZE / Math.tan(this.rayAngle);
    xstep *= this.isRayFacingLeft && xstep > 0 ? -1 : 1;
    xstep *= this.isRayFacingRight && xstep < 0 ? -1 : 1;

    let nextHorzTouchX = xintercept;
    let nextHorzTouchY = yintercept;

    // increment xstep and ystep until we find a wall
    while (
      nextHorzTouchX >= 0 &&
      nextHorzTouchX <= WINDOW_WIDTH &&
      nextHorzTouchY >= 0 &&
      nextHorzTouchY <= WINDOW_HEIGHT
    ) {
      if (
        grid.hasWallAt(
          nextHorzTouchX,
          nextHorzTouchY - (this.isRayFacingUp ? 1 : 0),
        )
      ) {
        foundHorzWallHit = true;
        horzWallHitX = nextHorzTouchX;
        horzWallHitY = nextHorzTouchY;
        break;
      } else {
        nextHorzTouchX += xstep;
        nextHorzTouchY += ystep;
      }
    }

    /* VERTICAL RAY-GRID INTERSECTION CODE */

    let foundVertWallHit = false;
    let vertWallHitX = 0;
    let vertWallHitY = 0;

    // find the x-coordinate of the closest vertical grid intersenction
    xintercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += this.isRayFacingRight ? TILE_SIZE : 0;

    // find the y-coordinate of the closest vertical grid intersection
    yintercept = player.y + (xintercept - player.x) * Math.tan(this.rayAngle);

    // calculate the increment xstep and ystep
    xstep = TILE_SIZE;
    xstep *= this.isRayFacingLeft ? -1 : 1;

    ystep = TILE_SIZE * Math.tan(this.rayAngle);
    ystep *= this.isRayFacingUp && ystep > 0 ? -1 : 1;
    ystep *= this.isRayFacingDown && ystep < 0 ? -1 : 1;

    let nextVertTouchX = xintercept;
    let nextVertTouchY = yintercept;

    // increment xstep and ystep until we find a wall
    while (
      nextVertTouchX >= 0 &&
      nextVertTouchX <= WINDOW_WIDTH &&
      nextVertTouchY >= 0 &&
      nextVertTouchY <= WINDOW_HEIGHT
    ) {
      if (
        grid.hasWallAt(
          nextVertTouchX - (this.isRayFacingLeft ? 1 : 0),
          nextVertTouchY,
        )
      ) {
        foundVertWallHit = true;
        vertWallHitX = nextVertTouchX;
        vertWallHitY = nextVertTouchY;
        break;
      } else {
        nextVertTouchX += xstep;
        nextVertTouchY += ystep;
      }
    }

    // calculate both horizontal and vertical distances and choose the smallest value
    let horzHitDistance = foundHorzWallHit
      ? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
      : Number.MAX_VALUE;
    let vertHitDistance = foundVertWallHit
      ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
      : Number.MAX_VALUE;

    // only store the smallest of the distances
    this.wallHitX =
      horzHitDistance < vertHitDistance ? horzWallHitX : vertWallHitX;
    this.wallHitY =
      horzHitDistance < vertHitDistance ? horzWallHitY : vertWallHitY;
    this.distance =
      horzHitDistance < vertHitDistance ? horzHitDistance : vertHitDistance;
    this.wasHitVertical = vertHitDistance < horzHitDistance;
  }

  render() {
    stroke("rgba(255, 0, 0, 0.3)");
    line(player.x, player.y, this.wallHitX, this.wallHitY);
  }
}

let grid = new Map();
let player = new Player();
let rays = [];

function keyPressed() {
  if (keyCode == UP_ARROW) player.walkDirection = +1;
  else if (keyCode == DOWN_ARROW) player.walkDirection = -1;
  else if (keyCode == RIGHT_ARROW) player.turnDirection = +1;
  else if (keyCode == LEFT_ARROW) player.turnDirection = -1;
}

function keyReleased() {
  if (keyCode == UP_ARROW) player.walkDirection = 0;
  else if (keyCode == DOWN_ARROW) player.walkDirection = 0;
  else if (keyCode == RIGHT_ARROW) player.turnDirection = 0;
  else if (keyCode == LEFT_ARROW) player.turnDirection = 0;
}

function castAllRays() {
  let columnId = 0;
  let rayAngle = player.rotationAngle - FOV_ANGLE / 2;

  rays = [];
  for (let i = 0; i < NUM_RAYS; i++) {
    let ray = new Ray(rayAngle);
    ray.cast(columnId);
    rays.push(ray);

    rayAngle += FOV_ANGLE / NUM_RAYS;
    columnId++;
  }
}

function normaliseAngle(angle) {
  angle = angle % (2 * Math.PI);
  if (angle < 0) angle = 2 * Math.PI + angle;

  return angle;
}

function distanceBetweenPoints(x1, y1, x2, y2) {
  return Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

function setup() {
  createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);
}

function update() {
  player.update();
  castAllRays();
}

function draw() {
  update();

  grid.render();
  for (ray of rays) {
    ray.render();
  }

  player.render();
}
