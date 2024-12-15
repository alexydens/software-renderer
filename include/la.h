/* Include guard */
#if !defined(LA_H)
#define LA_H

/* Linear algebra */

/* Includes */
#include <stdint.h>
#include <stdbool.h>

/* Constants */
#define PI  3.1415926535897932384626433832795
#define INF (1.0 / 0.0)

/* 2d vector type */
typedef struct {
  union {
    float v[2];
    struct {
      float x, y;
    };
  };
} vec2_t;
/* 3d vector type */
typedef struct {
  union {
    float v[3];
    struct {
      float x, y, z;
    };
  };
} vec3_t;
/* 4d vector type */
typedef struct {
  union {
    float v[4];
    struct {
      float x, y, z, w;
    };
  };
} vec4_t;
/* 4x4 matrix type */
typedef struct {
  float m[4][4];
} m4x4_t;

/* Create a 2d vector */
#define V2_FROM(x, y) (vec2_t){ .v = {(x), (y)} }
/* Create a 3d vector */
#define V3_FROM(x, y, z) (vec3_t){.v = { (x), (y), (z) } }
/* Create a 4d vector */
#define V4_FROM(x, y, z, w) (vec4_t){.v = { (x), (y), (z), (w) } }
/* Dot product of two 2d vectors */
extern float v2dot(vec2_t a, vec2_t b);
/* Dot product of two 3d vectors */
extern float v3dot(vec3_t a, vec3_t b);
/* Dot product of two 4d vectors */
extern float v4dot(vec4_t a, vec4_t b);
/* Cross product of two 2d vectors */
extern float v2cross(vec2_t a, vec2_t b);
/* Cross product of two 3d vectors */
extern vec3_t v3cross(vec3_t a, vec3_t b);
/* Add two 2d vectors */
extern vec2_t v2add(vec2_t a, vec2_t b);
/* Add two 3d vectors */
extern vec3_t v3add(vec3_t a, vec3_t b);
/* Add two 4d vectors */
extern vec4_t v4add(vec4_t a, vec4_t b);
/* Subtract two 2d vectors */
extern vec2_t v2sub(vec2_t a, vec2_t b);
/* Subtract two 3d vectors */
extern vec3_t v3sub(vec3_t a, vec3_t b);
/* Subtract two 4d vectors */
extern vec4_t v4sub(vec4_t a, vec4_t b);
/* Scale a 2d vector */
extern vec2_t v2scale(vec2_t a, float b);
/* Scale a 3d vector */
extern vec3_t v3scale(vec3_t a, float b);
/* Scale a 4d vector */
extern vec4_t v4scale(vec4_t a, float b);

/* 2d vector length */
extern float v2len(vec2_t a);
/* 3d vector length */
extern float v3len(vec3_t a);
/* 4d vector length */
extern float v4len(vec4_t a);
/* Normalize a 2d vector */
extern vec2_t v2normalize(vec2_t a);
/* Normalize a 3d vector */
extern vec3_t v3normalize(vec3_t a);
/* Normalize a 4d vector */
extern vec4_t v4normalize(vec4_t a);

/* Identity 4x4 matrix */
extern m4x4_t m4x4_identity(void);
/* Translation 4x4 matrix */
extern m4x4_t m4x4_translation(vec3_t v);
/* Scale 4x4 matrix */
extern m4x4_t m4x4_scale(vec3_t v);
/* X axis rotation 4x4 matrix */
extern m4x4_t m4x4_xrot(float angle);
/* Y axis rotation 4x4 matrix */
extern m4x4_t m4x4_yrot(float angle);
/* Z axis rotation 4x4 matrix */
extern m4x4_t m4x4_zrot(float angle);
/* Pitch,yaw,roll 4x4 matrix */
extern m4x4_t m4x4_euler(float pitch, float yaw, float roll);
/* Perspective projection 4x4 matrix */
extern m4x4_t m4x4_perspective(
    float fov,
    float aspect,
    float near,
    float far
);
/* Same matrix as gluLookAt(), as far as I know */
extern m4x4_t m4x4_look_at(vec3_t eye, vec3_t center, vec3_t up);

/* Multiply two 4x4 matrices */
extern m4x4_t m4x4_mul(m4x4_t a, m4x4_t b);
/* Multiply a 4x4 matrix with a 4d vector */
extern vec4_t m4x4v4_mul(m4x4_t a, vec4_t b);
/*
 * I could add inverting and finding the determinant, but I don't understand or
 * need that.
 */

#endif /* LA_H */
