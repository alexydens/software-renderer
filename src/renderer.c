/* Implements renderer.h */
#include <renderer.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Consts */
#define DIFFUSE 0.3
#define AMBIENT 0.3

/* Get max of three floats */
static float max(float a, float b, float c) {
  return a > b ? (a > c ? a : c) : (b > c ? b : c);
}
/* Get min of three floats */
static float min(float a, float b, float c) {
  return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* Get r from rgb as float 0-1 */
#define r(rgb) ((rgba >> 24) & 0xFF) / 255.0f
/* Get g from rgb as float 0-1 */
#define g(rgb) ((rgba >> 16) & 0xFF) / 255.0f
/* Get b from rgb as float 0-1 */
#define b(rgb) ((rgba >> 8) & 0xFF) / 255.0f

/* Create rgb from floats */
static uint32_t rgb(float r, float g, float b) {
  return (uint32_t)(r * 255) << 24 | (uint32_t)(g * 255) << 16 |
    (uint32_t)(b * 255) << 8;
}

/* Rasterize a triangle */
static void rasterize_triangle(
    renderer_t *renderer,
    tri_t *tri
) {
  /* Get bounds */
  float xmin = min(tri->points[0].x, tri->points[1].x, tri->points[2].x);
  float ymin = min(tri->points[0].y, tri->points[1].y, tri->points[2].y);
  float xmax = max(tri->points[0].x, tri->points[1].x, tri->points[2].x);
  float ymax = max(tri->points[0].y, tri->points[1].y, tri->points[2].y);
  xmin--;
  ymin--;
  xmax++;
  ymax++;
  uint32_t x_min = xmin > 0 ? (uint32_t)xmin : 0;
  uint32_t y_min = ymin > 0 ? (uint32_t)ymin : 0;
  uint32_t x_max = xmax > 0 ? (uint32_t)xmax : 0;
  uint32_t y_max = ymax > 0 ? (uint32_t)ymax : 0;
  x_min = x_min >= renderer->width ? renderer->width - 1 : x_min;
  y_min = y_min >= renderer->height ? renderer->height - 1 : y_min;
  x_max = x_max >= renderer->width ? renderer->width - 1 : x_max;
  y_max = y_max >= renderer->height ? renderer->height - 1 : y_max;

  /* Precompute where possible */
  vec2_t v0 = V2_FROM(
      tri->points[1].x - tri->points[0].x,
      tri->points[1].y - tri->points[0].y
  );
  vec2_t v1 = V2_FROM(
      tri->points[2].x - tri->points[0].x,
      tri->points[2].y - tri->points[0].y
  );
  vec2_t d0 = V2_FROM(v2dot(v0, v0), v2dot(v0, v1));
  vec2_t d1 = V2_FROM(v2dot(v1, v0), v2dot(v1, v1));
  float d = d0.x * d1.y - d0.y * d0.y;
  if (d == 0) return;

  for (uint32_t y = y_min; y < y_max; y++) {
    bool row = false;
    for (uint32_t x = x_min; x < x_max; x++) {
      vec2_t v2 = V2_FROM(x-tri->points[0].x, y-tri->points[0].y);
      vec2_t d2 = V2_FROM(v2dot(v2, v0), v2dot(v2, v1));

      float v = (d1.y * d2.x - d0.y * d2.y) / d;
      float w = (d0.x * d2.y - d0.y * d2.x) / d;
      float u = 1 - v - w;

      if (u >= 0 && v >= 0 && w >= 0) {
        row = true;
        vec3_t col = V3_FROM(
            u*tri->cols[0].x + v*tri->cols[1].x + w*tri->cols[2].x,
            u*tri->cols[0].y + v*tri->cols[1].y + w*tri->cols[2].y,
            u*tri->cols[0].z + v*tri->cols[1].z + w*tri->cols[2].z
        );
        float z = u*tri->cols[0].z
          + v*tri->cols[1].z
          + w*tri->cols[2].z;
        if (z < renderer->depthbuffer[y * renderer->width + x]) {
          renderer->depthbuffer[y * renderer->width + x] = z;
          renderer->framebuffer[y * renderer->width + x] =
            rgb(col.x, col.y, col.z);
        }
      } else {
        if (row) break;
      }
    }
  }
}
static void draw_triangle(
    renderer_t *renderer,
    tri_t *tri
) {
  /* Calculate normal */
  vec3_t a = v3sub(tri->points[2], tri->points[0]);
  vec3_t b = v3sub(tri->points[1], tri->points[0]);
  vec3_t normal = v3normalize(v3cross(a, b));
  /* Cull if possible */
  if (normal.z < 0) return;
  for (uint32_t i = 0; i < 3; i++) {
    if (tri->points[i].z > 0) return;
  }
  /* Project */
  m4x4_t proj = m4x4_perspective(
      renderer->camera.fov,
      (float)(renderer->width)/(float)(renderer->height),
      renderer->camera.near,
      renderer->camera.far
  );
  vec4_t p0 = V4_FROM(tri->points[0].x, tri->points[0].y, tri->points[0].z, 1);
  vec4_t p1 = V4_FROM(tri->points[1].x, tri->points[1].y, tri->points[1].z, 1);
  vec4_t p2 = V4_FROM(tri->points[2].x, tri->points[2].y, tri->points[2].z, 1);
  p0 = m4x4v4_mul(proj, p0);
  p1 = m4x4v4_mul(proj, p1);
  p2 = m4x4v4_mul(proj, p2);
  vec3_t proj_points[3];
  proj_points[0] = V3_FROM(p0.x/p0.w, p0.y/p0.w, p0.z/p0.w);
  proj_points[1] = V3_FROM(p1.x/p1.w, p1.y/p1.w, p1.z/p1.w);
  proj_points[2] = V3_FROM(p2.x/p2.w, p2.y/p2.w, p2.z/p2.w);
  proj_points[0].x = (1+proj_points[0].x) * renderer->width / 2;
  proj_points[0].y = (1+proj_points[0].y) * renderer->height / 2;
  proj_points[1].x = (1+proj_points[1].x) * renderer->width / 2;
  proj_points[1].y = (1+proj_points[1].y) * renderer->height / 2;
  proj_points[2].x = (1+proj_points[2].x) * renderer->width / 2;
  proj_points[2].y = (1+proj_points[2].y) * renderer->height / 2;
  float l = AMBIENT + DIFFUSE*(v3dot(normal, V3_FROM(0, 0, 1)));
  vec3_t cols[3];
  cols[0] = V3_FROM(tri->cols[0].x*l, tri->cols[0].y*l, tri->cols[0].z*l);
  cols[1] = V3_FROM(tri->cols[1].x*l, tri->cols[1].y*l, tri->cols[1].z*l);
  cols[2] = V3_FROM(tri->cols[2].x*l, tri->cols[2].y*l, tri->cols[2].z*l);
  /* Render */
  tri_t t;
  t.points[0] = proj_points[0];
  t.points[1] = proj_points[1];
  t.points[2] = proj_points[2];
  t.cols[0] = cols[0];
  t.cols[1] = cols[1];
  t.cols[2] = cols[2];
  rasterize_triangle(renderer, &t);
}
static void tri_transform(
    tri_t *tri,
    vec3_t translate,
    vec3_t scale,
    vec3_t rotate
) {
  m4x4_t rot = m4x4_euler(rotate.x, rotate.y, rotate.z);
  vec4_t p0 = V4_FROM(tri->points[0].x, tri->points[0].y, tri->points[0].z, 1);
  vec4_t p1 = V4_FROM(tri->points[1].x, tri->points[1].y, tri->points[1].z, 1);
  vec4_t p2 = V4_FROM(tri->points[2].x, tri->points[2].y, tri->points[2].z, 1);
  p0 = m4x4v4_mul(rot, p0);
  p1 = m4x4v4_mul(rot, p1);
  p2 = m4x4v4_mul(rot, p2);
  tri->points[0] = V3_FROM(p0.x, p0.y, p0.z);
  tri->points[1] = V3_FROM(p1.x, p1.y, p1.z);
  tri->points[2] = V3_FROM(p2.x, p2.y, p2.z);
  tri->points[0] = v3scale(tri->points[0], scale.x);
  tri->points[1] = v3scale(tri->points[1], scale.y);
  tri->points[2] = v3scale(tri->points[2], scale.z);
  tri->points[0] = v3add(tri->points[0], translate);
  tri->points[1] = v3add(tri->points[1], translate);
  tri->points[2] = v3add(tri->points[2], translate);
}
/* Apply a matrix */
static void apply_matrix(
  tri_t *tri,
  m4x4_t m
) {
  vec4_t p0 = V4_FROM(tri->points[0].x, tri->points[0].y, tri->points[0].z, 1);
  vec4_t p1 = V4_FROM(tri->points[1].x, tri->points[1].y, tri->points[1].z, 1);
  vec4_t p2 = V4_FROM(tri->points[2].x, tri->points[2].y, tri->points[2].z, 1);
  p0 = m4x4v4_mul(m, p0);
  p1 = m4x4v4_mul(m, p1);
  p2 = m4x4v4_mul(m, p2);
  tri->points[0] = V3_FROM(p0.x, p0.y, p0.z);
  tri->points[1] = V3_FROM(p1.x, p1.y, p1.z);
  tri->points[2] = V3_FROM(p2.x, p2.y, p2.z);
}

/* Create renderer */
void renderer_create(
    renderer_t *renderer,
    uint32_t width,
    uint32_t height
) {
  renderer->width = width;
  renderer->height = height;
  renderer->framebuffer = malloc(width * height * sizeof(uint32_t));
  memset(renderer->framebuffer, 0, width * height * sizeof(uint32_t));
  renderer->depthbuffer = malloc(width * height * sizeof(float));
  for (uint32_t i = 0; i < width * height; i++) {
    renderer->depthbuffer[i] = INF;
  }
  renderer->camera.pos = V3_FROM(0, 0, 0);
  renderer->camera.forward = V3_FROM(0, 0, -1);
  renderer->camera.up = V3_FROM(0, 1, 0);
  renderer->camera.fov = 60;
  renderer->camera.near = 0.1;
  renderer->camera.far = 100;
  renderer->camera.pitch = 0;
  renderer->camera.yaw = -90;
}
/* Destroy renderer */
void renderer_destroy(renderer_t *renderer) {
  free(renderer->framebuffer);
  free(renderer->depthbuffer);
}
/* Resize renderer */
void renderer_resize(
    renderer_t *renderer,
    uint32_t width,
    uint32_t height
) {
  renderer->width = width;
  renderer->height = height;
  renderer->framebuffer =
    realloc(renderer->framebuffer, width * height * sizeof(uint32_t));
  memset(renderer->framebuffer, 0, width * height * sizeof(uint32_t));
  renderer->depthbuffer =
    realloc(renderer->depthbuffer, width * height * sizeof(float));
  memset(renderer->depthbuffer, 0, width * height * sizeof(float));
}
/* Clear frame and depth buffer */
void renderer_clear(renderer_t *renderer) {
  memset(
      renderer->framebuffer,
      0,
      renderer->width * renderer->height * sizeof(uint32_t)
  );
  for (uint32_t i = 0; i < renderer->width * renderer->height; i++) {
    renderer->depthbuffer[i] = INF;
  }
}
/* Render mesh */
void renderer_draw(renderer_t *renderer, mesh_t *mesh) {
  /* Update camera */
  float pitch = renderer->camera.pitch * (PI/180);
  float yaw = renderer->camera.yaw * (PI/180);
  renderer->camera.forward = v3normalize(
      V3_FROM(
          cosf(yaw) * cosf(pitch),
          sinf(pitch),
          sinf(yaw) * cosf(pitch)
      )
  );
  /* Draw */
  for (uint32_t i = 0; i < mesh->num_tris; i++) {
    tri_t tri = mesh->tris[i];
    tri_transform(&tri, mesh->translate, mesh->scale, mesh->rotate);
    apply_matrix(&tri, m4x4_look_at(
      renderer->camera.pos,
      v3add(renderer->camera.pos, renderer->camera.forward),
      renderer->camera.up
    ));
    draw_triangle(renderer, &tri);
  }
}
