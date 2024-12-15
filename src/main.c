/* Includes */
#include <renderer.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* Consts */
#define SENSITIVITY       32
#define MOVEMENT_SPEED    8
#define SCALE_DOWN        8
/*
 * NOTE: This doesn't do a great job of capping the framerate, but it's good
 * enough for my purposes.
 */
#define FRAMERATE_CAP     60
#define FLOOR_TILES       100
#define NOISE_SIZE        FLOOR_TILES/4
#define PERMUTATION_SIZE  256

/* Noise data */
int permutation[PERMUTATION_SIZE];
/* Mesh data */
tri_t mesh_data[FLOOR_TILES*FLOOR_TILES*2];
/*
tri_t mesh_data[] = {
  {
    .points = {
      {.v = { -0.5,  0.0, -0.5 }},
      {.v = {  0.5,  0.0, -0.5 }},
      {.v = {  0.5,  0.0,  0.5 }},
    },
    .cols = {
      {.v = {  1.0,  0.0,  0.0 }},
      {.v = {  0.0,  1.0,  0.0 }},
      {.v = {  0.0,  0.0,  1.0 }},
    }
  }, {
    .points = {
      {.v = {  0.5,  0.0,  0.5 }},
      {.v = { -0.5,  0.0,  0.5 }},
      {.v = { -0.5,  0.0, -0.5 }},
    },
    .cols = {
      {.v = {  0.0,  0.0,  1.0 }},
      {.v = {  1.0,  1.0,  0.0 }},
      {.v = {  1.0,  0.0,  0.0 }},
    }
  }
};
*/

/* Helpers for get noise */
float lerp(float a, float b, float t) { return a + (b - a) * t; }
float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
vec2_t get_vec(int a) {
  int i = a % 4;
  switch (i) {
    case 0: return V2_FROM(0, 1);
    case 1: return V2_FROM(0, -1);
    case 2: return V2_FROM(1, 0);
    case 3: return V2_FROM(-1, 0);
    default: return V2_FROM(0, 0);
  }
}
/*
 * Get noise value at a point:
 * - x from 0 to 1
 * - y from 0 to 1
 * - heavily inspired by https://rtouti.github.io/graphics/perlin-noise-algorithm
 */
float noise(float x, float y) {
  int ix = (int)(x * NOISE_SIZE);
  int iy = (int)(y * NOISE_SIZE);

  float xf = x * NOISE_SIZE - ix;
  float yf = y * NOISE_SIZE - iy;

  vec2_t top_right = V2_FROM(xf-1, yf-1);
  vec2_t top_left = V2_FROM(xf, yf-1);
  vec2_t bottom_right = V2_FROM(xf-1, yf);
  vec2_t bottom_left = V2_FROM(xf, yf);

  int X = ix % PERMUTATION_SIZE;
  int Y = iy % PERMUTATION_SIZE;
  int value_top_right = permutation[permutation[X+1]+Y+1];
  int value_top_left = permutation[permutation[X]+Y+1];
  int value_bottom_right = permutation[permutation[X+1]+Y];
  int value_bottom_left = permutation[permutation[X]+Y];

  float dot_top_right = v2dot(top_right, get_vec(value_top_right));
  float dot_top_left = v2dot(top_left, get_vec(value_top_left));
  float dot_bottom_right = v2dot(bottom_right, get_vec(value_bottom_right));
  float dot_bottom_left = v2dot(bottom_left, get_vec(value_bottom_left));

  float u = fade(xf);
  float v = fade(yf);

  return lerp(
      lerp(dot_top_left, dot_bottom_left, u),
      lerp(dot_top_right, dot_bottom_right, u),
      v
  );
}

/* Entry point */
int main(void) {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *sdl_window = SDL_CreateWindow(
      "Rasterizer",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      800, 600,
      SDL_WINDOW_RESIZABLE
  );
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_Renderer *sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
  SDL_Texture *sdl_texture = SDL_CreateTexture(
      sdl_renderer,
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_STREAMING,
      800/SCALE_DOWN, 600/SCALE_DOWN
  );
  renderer_t renderer;
  mesh_t mesh;
  renderer_create(&renderer, 800/SCALE_DOWN, 600/SCALE_DOWN);
  mesh.tris = mesh_data;
  mesh.num_tris = sizeof(mesh_data) / sizeof(tri_t);
  mesh.translate = V3_FROM(0, 0, 0);
  mesh.scale = V3_FROM(1, 1, 1);
  mesh.rotate = V3_FROM(0, 0, 0);

  /* Generate permutation */
  for (int i = 0; i < PERMUTATION_SIZE; i++) {
    permutation[i] = i;
  }
  for (int i = 0; i < PERMUTATION_SIZE; i++) {
    int r = rand() % PERMUTATION_SIZE;
    int t = permutation[i];
    permutation[i] = permutation[r];
    permutation[r] = t;
  }
  for (int i = 0; i < PERMUTATION_SIZE; i++) {
    permutation[PERMUTATION_SIZE+i] = permutation[i];
  }

  /* Generate floor */
  float offsets[(FLOOR_TILES+1)*(FLOOR_TILES+1)];
  /* Old (basic noise) */
  /*for (int i = 0; i < FLOOR_TILES+1; i++) {
    for (int j = 0; j < FLOOR_TILES+1; j++) {
      offsets[i*(FLOOR_TILES+1)+j] = (float)rand() / (float)RAND_MAX;
    }
  }*/
  /* New (perlin noise) */
  for (int i = 0; i < FLOOR_TILES+1; i++) {
    for (int j = 0; j < FLOOR_TILES+1; j++) {
      float n = noise((float)i/(float)FLOOR_TILES, (float)j/(float)FLOOR_TILES);
      offsets[i*(FLOOR_TILES+1)+j] = n * 2;
    }
  }

  /* Generate actual mesh */
  for (int i = 0; i < FLOOR_TILES; i++) {
    for (int j = 0; j < FLOOR_TILES; j++) {
      /* Append the relevant quad, 1 triangle at a time */
      float x = (float)(-FLOOR_TILES)/2 + (float)i;
      float z = (float)(-FLOOR_TILES)/2 + (float)j;
      float z_offsets[4];
      z_offsets[0] = offsets[(i*(FLOOR_TILES+1)+j)];
      z_offsets[1] = offsets[(i*(FLOOR_TILES+1)+j+1)];
      z_offsets[2] = offsets[((i+1)*(FLOOR_TILES+1)+j+1)];
      z_offsets[3] = offsets[((i+1)*(FLOOR_TILES+1)+j)];
      mesh_data[(i*FLOOR_TILES+j)*2].points[0] = V3_FROM(x, z_offsets[0], z);
      mesh_data[(i*FLOOR_TILES+j)*2].points[1] = V3_FROM(x, z_offsets[1], z+1);
      mesh_data[(i*FLOOR_TILES+j)*2].points[2] = V3_FROM(x+1, z_offsets[2], z+1);
      mesh_data[(i*FLOOR_TILES+j)*2].cols[0] = v3scale(V3_FROM(1, 1, 1), z_offsets[0]);
      mesh_data[(i*FLOOR_TILES+j)*2].cols[1] = v3scale(V3_FROM(1, 1, 1), z_offsets[1]);
      mesh_data[(i*FLOOR_TILES+j)*2].cols[2] = v3scale(V3_FROM(1, 1, 1), z_offsets[2]);
      //mesh_data[(i*FLOOR_TILES+j)*2].cols[0] = V3_FROM(1, 1, 1);
      //mesh_data[(i*FLOOR_TILES+j)*2].cols[1] = V3_FROM(1, 1, 1);
      //mesh_data[(i*FLOOR_TILES+j)*2].cols[2] = V3_FROM(1, 1, 1);
      mesh_data[(i*FLOOR_TILES+j)*2+1].points[0] = V3_FROM(x+1, z_offsets[2], z+1);
      mesh_data[(i*FLOOR_TILES+j)*2+1].points[1] = V3_FROM(x+1, z_offsets[3], z);
      mesh_data[(i*FLOOR_TILES+j)*2+1].points[2] = V3_FROM(x, z_offsets[0], z);
      mesh_data[(i*FLOOR_TILES+j)*2+1].cols[0] = v3scale(V3_FROM(1, 1, 1), z_offsets[2]);
      mesh_data[(i*FLOOR_TILES+j)*2+1].cols[1] = v3scale(V3_FROM(1, 1, 1), z_offsets[3]);
      mesh_data[(i*FLOOR_TILES+j)*2+1].cols[2] = v3scale(V3_FROM(1, 1, 1), z_offsets[0]);
      //mesh_data[(i*FLOOR_TILES+j)*2+1].cols[0] = V3_FROM(1, 1, 1);
      //mesh_data[(i*FLOOR_TILES+j)*2+1].cols[1] = V3_FROM(1, 1, 1);
      //mesh_data[(i*FLOOR_TILES+j)*2+1].cols[2] = V3_FROM(1, 1, 1);
    }
  }

  /* Main loop */
  const uint8_t *keys = SDL_GetKeyboardState(NULL);
  bool running = true;
  uint64_t now = SDL_GetPerformanceCounter();
  uint64_t last = 0;
  float delta_time = 0;
  uint64_t ticks = 0;
  while (running) {
    if (delta_time < 1.0/FRAMERATE_CAP) {
      SDL_Delay((1.0/60.0 - delta_time)*1000.0);
    }
    /* Calculate delta time */
    last = now;
    now = SDL_GetPerformanceCounter();
    delta_time = (float)(now - last) / (float)SDL_GetPerformanceFrequency();
    ticks++;
    if (ticks % 200 == 0) {
      printf("fps: %f\n", 1/delta_time);
    }

    /* Event loop */
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          renderer_resize(
              &renderer,
              event.window.data1/SCALE_DOWN,
              event.window.data2/SCALE_DOWN
          );
          SDL_DestroyTexture(sdl_texture);
          SDL_CreateTexture(
              sdl_renderer,
              SDL_PIXELFORMAT_RGBA8888,
              SDL_TEXTUREACCESS_STREAMING,
              renderer.width, renderer.height
          );
        }
      }
    }
    if (keys[SDL_SCANCODE_W]) {
      renderer.camera.pos = v3add(
          renderer.camera.pos,
          v3scale(renderer.camera.forward, MOVEMENT_SPEED*delta_time)
      );
    }
    if (keys[SDL_SCANCODE_S]) {
      renderer.camera.pos = v3sub(
          renderer.camera.pos,
          v3scale(renderer.camera.forward, MOVEMENT_SPEED*delta_time)
      );
    }
    if (keys[SDL_SCANCODE_A]) {
      renderer.camera.pos = v3sub(
          renderer.camera.pos,
          v3scale(v3normalize(v3cross(
                renderer.camera.forward,
                renderer.camera.up
          )), MOVEMENT_SPEED*delta_time)
      );
    }
    if (keys[SDL_SCANCODE_D]) {
      renderer.camera.pos = v3add(
          renderer.camera.pos,
          v3scale(v3normalize(v3cross(
                renderer.camera.forward, 
                renderer.camera.up
          )), MOVEMENT_SPEED*delta_time)
      );
    }
    if (keys[SDL_SCANCODE_SPACE]) {
      renderer.camera.pos = v3sub(
          renderer.camera.pos,
          v3scale(renderer.camera.up, MOVEMENT_SPEED*delta_time)
      );
    }
    if (keys[SDL_SCANCODE_LSHIFT]) {
      renderer.camera.pos = v3add(
          renderer.camera.pos,
          v3scale(renderer.camera.up, MOVEMENT_SPEED*delta_time)
      );
    }
    int mousex, mousey;
    SDL_GetRelativeMouseState(&mousex, &mousey);
    renderer.camera.yaw += mousex*SENSITIVITY*delta_time;
    renderer.camera.pitch += mousey*SENSITIVITY*delta_time;
    if (renderer.camera.pitch > 89) renderer.camera.pitch = 89;
    if (renderer.camera.pitch < -89) renderer.camera.pitch = -89;
    renderer_clear(&renderer);
    renderer_draw(&renderer, &mesh);
    SDL_UpdateTexture(
        sdl_texture,
        NULL,
        renderer.framebuffer,
        renderer.width * sizeof(uint32_t)
    );
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
  }

  renderer_destroy(&renderer);
  SDL_DestroyTexture(sdl_texture);
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
  return 0;
}
