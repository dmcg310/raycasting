#include "constants.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct Player
{
  float x;
  float y;
  float width;
  float height;
  int turnDirection; // -1 left +1 right
  int walkDirection; // -1 back +1 forward
  float rotationAngle;
  float walkSpeed;
  float turnSpeed;
} player;

struct Ray
{
  float rayAngle;
  float wallHitX;
  float wallHitY;
  int wasHitVertical;
  int isRayFacingUp;
  int isRayFacingDown;
  int isRayFacingLeft;
  int isRayFacingRight;
  int wallHitContent;
} rays[NUM_RAYS];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int isGameRunning = FALSE;
int ticksLastFrame;

int initialiseWindow()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    fprintf(stderr, "Error initialising SDL.\n");
    return FALSE;
  }

  window = SDL_CreateWindow("raycasting", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_BORDERLESS);

  if (!window)
  {
    fprintf(stderr, "Error creating SDL window.\n");
    return FALSE;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer)
  {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return FALSE;
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  return TRUE;
}

void destroyWindow()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void setup()
{
  player.x = WINDOW_WIDTH / 2.0f;
  player.y = WINDOW_HEIGHT / 2.0f;
  player.width = 1;
  player.height = 1;
  player.turnDirection = 0;
  player.walkDirection = 0;
  player.rotationAngle = PI / 2;
  player.walkSpeed = 100;
  player.turnSpeed = 45 * (PI / 180);
}

int mapHasWallAt(float x, float y)
{
  if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
    return TRUE;

  int mapGridIndexX = floor(x / TILE_SIZE);
  int mapGridIndeyY = floor(y / TILE_SIZE);

  return map[mapGridIndeyY][mapGridIndexX] != 0;
}

void movePlayer(float deltaTime)
{
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
  float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

  float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
  float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

  // perform wall collision
  if (!mapHasWallAt(newPlayerX, newPlayerY))
  {
    player.x = newPlayerX;
    player.y = newPlayerY;
  }
}

void renderPlayer()
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect playerRect = {
      player.x * MINIMAP_SCALE_FACTOR,
      player.y * MINIMAP_SCALE_FACTOR,
      player.width * MINIMAP_SCALE_FACTOR,
      player.height * MINIMAP_SCALE_FACTOR,
  };

  SDL_RenderFillRect(renderer, &playerRect);

  SDL_RenderDrawLine(
      renderer, MINIMAP_SCALE_FACTOR * player.x,
      MINIMAP_SCALE_FACTOR * player.y,
      MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
      MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40);
}

float normaliseAngle(float angle)
{
  angle = remainder(angle, TWO_PI);
  if (angle < 0)
  {
    angle = TWO_PI + angle;
  }

  return angle;
}

void castRay(float rayAngle, int stripId)
{
  rayAngle = normaliseAngle(rayAngle);

  int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
  int isRayFacingUp = !isRayFacingDown;
  int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
  int isRayFacingLeft = !isRayFacingRight;

  float xintercept, yintercept;
  float xstep, ystep;

  /* HORIZONTAL RAY-GRID INTERSECTION CODE */

  int foundHorzWallHit = FALSE;
  float horzWallHitX = 0;
  float horzWallHitY = 0;
  int horzWallContent = 0;

  // find the y-coordinate of the closest horizontal grid intersection
  yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
  yintercept += isRayFacingDown ? TILE_SIZE : 0;

  // find the x-coordinate of the closest horizontal grid intersection
  xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

  // calculate the increment xstep and ystep
  ystep = TILE_SIZE;
  ystep *= isRayFacingUp ? -1 : 1;

  xstep = TILE_SIZE / tan(rayAngle);
  xstep *= isRayFacingLeft && xstep > 0 ? -1 : 1;
  xstep *= isRayFacingRight && xstep < 0 ? -1 : 1;

  float nextHorzTouchX = xintercept;
  float nextHorzTouchY = yintercept;

  // increment xstep and ystep until we find a wall
  while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH &&
         nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT)
  {
    float xToCheck = nextHorzTouchX;
    float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

    if (mapHasWallAt(xToCheck, yToCheck))
    {
      // wall hit
      horzWallHitX = nextHorzTouchX;
      horzWallHitY = nextHorzTouchY;
      horzWallContent = map[(int)floor(yToCheck / TILE_SIZE)]
                           [(int)floor(xToCheck / TILE_SIZE)];
      foundHorzWallHit = TRUE;
      break;
    }
    else
    {
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
  while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH &&
         nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT)
  {
    if (grid.hasWallAt(nextVertTouchX - (this.isRayFacingLeft ? 1 : 0),
                       nextVertTouchY, ))
    {
      foundVertWallHit = true;
      vertWallHitX = nextVertTouchX;
      vertWallHitY = nextVertTouchY;
      break;
    }
    else
    {
      nextVertTouchX += xstep;
      nextVertTouchY += ystep;
    }
  }

  // calculate both horizontal and vertical distances and choose the smallest
  // value
  let horzHitDistance = foundHorzWallHit
                            ? distanceBetweenPoints(player.x, player.y,
                                                    horzWallHitX, horzWallHitY)
                            : Number.MAX_VALUE;
  let vertHitDistance = foundVertWallHit
                            ? distanceBetweenPoints(player.x, player.y,
                                                    vertWallHitX, vertWallHitY)
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

void castAllRays()
{
  // start first ray subtracting half of FOV
  float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

  for (int stripId = 0; stripId < NUM_RAYS; stripId++)
  {
    castRay(rayAngle, stripId);

    rayAngle += FOV_ANGLE / NUM_RAYS;
  }
}

void renderMap()
{
  for (int i = 0; i < MAP_NUM_ROWS; i++)
  {
    for (int j = 0; j < MAP_NUM_COLS; j++)
    {
      int tileX = j * TILE_SIZE;
      int tileY = i * TILE_SIZE;
      int tileColor = map[i][j] != 0 ? 255 : 0;

      SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
      SDL_Rect mapTileRect = {
          tileX * MINIMAP_SCALE_FACTOR,
          tileY * MINIMAP_SCALE_FACTOR,
          TILE_SIZE * MINIMAP_SCALE_FACTOR,
          TILE_SIZE * MINIMAP_SCALE_FACTOR,
      };
      SDL_RenderFillRect(renderer, &mapTileRect);
    }
  }
}

void processInput()
{
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type)
  {
  case SDL_QUIT:
  {
    isGameRunning = FALSE;
    break;
  }

  case SDL_KEYDOWN:
  {
    if (event.key.keysym.sym == SDLK_ESCAPE)
      isGameRunning = FALSE;
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      player.walkDirection = +1;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      player.walkDirection = -1;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      player.turnDirection = +1;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      player.turnDirection = -1;
    break;
  }

  case SDL_KEYUP:
  {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      player.walkDirection = 0;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      player.walkDirection = 0;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      player.turnDirection = 0;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      player.turnDirection = 0;
    break;
  }
  }
}

void update()
{
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH))
    ;

  float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;
  ticksLastFrame = SDL_GetTicks();

  movePlayer(deltaTime);
  castAllRays();
}

void render()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  renderMap();
  // renderRays();
  renderPlayer();

  SDL_RenderPresent(renderer);
}

int main(void)
{
  isGameRunning = initialiseWindow();

  setup();

  while (isGameRunning)
  {
    processInput();
    update();
    render();
  }

  destroyWindow();

  return 0;
}
