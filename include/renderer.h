/* Include guard */
#if !defined(RENDERER_H)
#define RENDERER_H

/* Includes */
#include <stdint.h>
#include <stdbool.h>
#include <la.h>

/* Triangle struct */
typedef struct {
  vec3_t points[3];
  vec3_t cols[3];
} tri_t;
/* Mesh struct */
typedef struct {
  uint32_t num_tris;
  tri_t *tris;
  vec3_t translate;
  vec3_t scale;
  vec3_t rotate;
} mesh_t;
/* Camera struct */
typedef struct {
  vec3_t pos;
  vec3_t forward;
  vec3_t up;
  float fov;
  float pitch, yaw;
  float far, near;
} camera_t;
/* Renderer struct */
typedef struct {
  camera_t camera;
  uint32_t width, height;
  uint32_t *framebuffer;
  float *depthbuffer;
} renderer_t;

/* Create renderer */
extern void renderer_create(
    renderer_t *renderer,
    uint32_t width,
    uint32_t height
);
/* Destroy renderer */
extern void renderer_destroy(renderer_t *renderer);
/* Resize renderer */
extern void renderer_resize(
    renderer_t *renderer,
    uint32_t width,
    uint32_t height
);
/* Clear frame and depth buffer */
extern void renderer_clear(renderer_t *renderer);
/* Render mesh */
extern void renderer_draw(renderer_t *renderer, mesh_t *mesh);

#endif /* RENDERER_H */
