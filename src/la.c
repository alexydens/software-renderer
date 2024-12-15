/* Implements la.h */
#include <la.h>
#include <math.h>
#include <string.h>

/* Dot product of two 2d vectors */
float v2dot(vec2_t a, vec2_t b) {
  return a.x * b.x + a.y * b.y;
}
/* Dot product of two 3d vectors */
float v3dot(vec3_t a, vec3_t b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
/* Dot product of two 4d vectors */
float v4dot(vec4_t a, vec4_t b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
/* Cross product of two 2d vectors */
float v2cross(vec2_t a, vec2_t b) {
  return a.x * b.y - a.y * b.x;
}
/* Cross product of two 3d vectors */
vec3_t v3cross(vec3_t a, vec3_t b) {
  vec3_t c;
  c.x = a.y * b.z - a.z * b.y;
  c.y = a.z * b.x - a.x * b.z;
  c.z = a.x * b.y - a.y * b.x;
  return c;
}
/* Add two 2d vectors */
vec2_t v2add(vec2_t a, vec2_t b) {
  vec2_t c;
  c.x = a.x + b.x;
  c.y = a.y + b.y;
  return c;
}
/* Add two 3d vectors */
vec3_t v3add(vec3_t a, vec3_t b) {
  vec3_t c;
  c.x = a.x + b.x;
  c.y = a.y + b.y;
  c.z = a.z + b.z;
  return c;
}
/* Add two 4d vectors */
vec4_t v4add(vec4_t a, vec4_t b) {
  vec4_t c;
  c.x = a.x + b.x;
  c.y = a.y + b.y;
  c.z = a.z + b.z;
  c.w = a.w + b.w;
  return c;
}
/* Subtract two 2d vectors */
vec2_t v2sub(vec2_t a, vec2_t b) {
  vec2_t c;
  c.x = a.x - b.x;
  c.y = a.y - b.y;
  return c;
}
/* Subtract two 3d vectors */
vec3_t v3sub(vec3_t a, vec3_t b) {
  vec3_t c;
  c.x = a.x - b.x;
  c.y = a.y - b.y;
  c.z = a.z - b.z;
  return c;
}
/* Subtract two 4d vectors */
vec4_t v4sub(vec4_t a, vec4_t b) {
  vec4_t c;
  c.x = a.x - b.x;
  c.y = a.y - b.y;
  c.z = a.z - b.z;
  c.w = a.w - b.w;
  return c;
}
/* Scale a 2d vector */
vec2_t v2scale(vec2_t a, float b) {
  vec2_t c;
  c.x = a.x * b;
  c.y = a.y * b;
  return c;
}
/* Scale a 3d vector */
vec3_t v3scale(vec3_t a, float b) {
  vec3_t c;
  c.x = a.x * b;
  c.y = a.y * b;
  c.z = a.z * b;
  return c;
}
/* Scale a 4d vector */
vec4_t v4scale(vec4_t a, float b) {
  vec4_t c;
  c.x = a.x * b;
  c.y = a.y * b;
  c.z = a.z * b;
  c.w = a.w * b;
  return c;
}

/* 2d vector length */
float v2len(vec2_t a) {
  return sqrtf(v2dot(a, a));
}
/* 3d vector length */
float v3len(vec3_t a) {
  return sqrtf(v3dot(a, a));
}
/* 4d vector length */
float v4len(vec4_t a) {
  return sqrtf(v4dot(a, a));
}
/* Normalize a 2d vector */
vec2_t v2normalize(vec2_t a) {
  float len = v2len(a);
  vec2_t b;
  if (len == 0) return a;
  b.x = a.x / len;
  b.y = a.y / len;
  return b;
}
/* Normalize a 3d vector */
vec3_t v3normalize(vec3_t a) {
  float len = v3len(a);
  vec3_t b;
  if (len == 0) return a;
  b.x = a.x / len;
  b.y = a.y / len;
  b.z = a.z / len;
  return b;
}
/* Normalize a 4d vector */
vec4_t v4normalize(vec4_t a) {
  float len = v4len(a);
  vec4_t b;
  if (len == 0) return a;
  b.x = a.x / len;
  b.y = a.y / len;
  b.z = a.z / len;
  b.w = a.w / len;
  return b;
}

/* Identity 4x4 matrix */
m4x4_t m4x4_identity(void) {
  m4x4_t m;
  memset(&m, 0, sizeof(m));
  m.m[0][0] = 1;
  m.m[1][1] = 1;
  m.m[2][2] = 1;
  m.m[3][3] = 1;
  return m;
}
/* Translation 4x4 matrix */
m4x4_t m4x4_translation(vec3_t v) {
  m4x4_t m = m4x4_identity();
  m.m[0][3] = v.x;
  m.m[1][3] = v.y;
  m.m[2][3] = v.z;
  return m;
}
/* Scale 4x4 matrix */
m4x4_t m4x4_scale(vec3_t v) {
  m4x4_t m = m4x4_identity();
  m.m[0][0] = v.x;
  m.m[1][1] = v.y;
  m.m[2][2] = v.z;
  return m;
}
/* X axis rotation 4x4 matrix */
m4x4_t m4x4_xrot(float angle) {
  m4x4_t m = m4x4_identity();
  float theta = angle * PI / 180;
  m.m[1][1] = cosf(theta);
  m.m[1][2] = sinf(theta);
  m.m[2][1] = -sinf(theta);
  m.m[2][2] = cosf(theta);
  return m;
}
/* Y axis rotation 4x4 matrix */
m4x4_t m4x4_yrot(float angle) {
  m4x4_t m = m4x4_identity();
  float theta = angle * PI / 180;
  m.m[0][0] = cosf(theta);
  m.m[0][2] = sinf(theta);
  m.m[2][0] = -sinf(theta);
  m.m[2][2] = cosf(theta);
  return m;
}
/* Z axis rotation 4x4 matrix */
m4x4_t m4x4_zrot(float angle) {
  m4x4_t m = m4x4_identity();
  float theta = angle * PI / 180;
  m.m[0][0] = cosf(theta);
  m.m[0][1] = sinf(theta);
  m.m[1][0] = -sinf(theta);
  m.m[1][1] = cosf(theta);
  return m;
}
/* Pitch,yaw,roll 4x4 matrix */
m4x4_t m4x4_euler(float pitch, float yaw, float roll) {
  m4x4_t m = m4x4_identity();
  m = m4x4_mul(m4x4_zrot(roll), m);
  m = m4x4_mul(m4x4_yrot(yaw), m);
  m = m4x4_mul(m4x4_xrot(pitch), m);
  return m;
}
/* Perspective projection 4x4 matrix */
m4x4_t m4x4_perspective(
    float fov,
    float aspect,
    float near,
    float far
) {
  m4x4_t m = m4x4_identity();
  float theta = (fov/2) * PI / 180;
  m.m[0][0] = 1 / (aspect * tanf(theta / 2));
  m.m[1][1] = 1 / tanf(theta / 2);
  m.m[2][2] = -(far + near) / (far - near);
  m.m[2][3] = -2 * far * near / (far - near);
  m.m[3][2] = -1;
  return m;
}
/* Same matrix as gluLookAt(), as far as I know */
m4x4_t m4x4_look_at(vec3_t eye, vec3_t center, vec3_t up) {
  m4x4_t m = m4x4_identity();
  vec3_t f = v3normalize(v3sub(center, eye));
  vec3_t s = v3normalize(v3cross(f, up));
  vec3_t u = v3cross(s, f);
  m.m[0][0] = s.x;
  m.m[0][1] = s.y;
  m.m[0][2] = s.z;
  m.m[1][0] = u.x;
  m.m[1][1] = u.y;
  m.m[1][2] = u.z;
  m.m[2][0] = -f.x;
  m.m[2][1] = -f.y;
  m.m[2][2] = -f.z;
  m.m[0][3] = -v3dot(s, eye);
  m.m[1][3] = -v3dot(u, eye);
  m.m[2][3] = v3dot(f, eye);
  return m;
}

/* Multiply two 4x4 matrices */
m4x4_t m4x4_mul(m4x4_t a, m4x4_t b) {
  m4x4_t m;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m.m[i][j] =
        a.m[i][0] * b.m[0][j]
        + a.m[i][1] * b.m[1][j]
        + a.m[i][2] * b.m[2][j]
        + a.m[i][3] * b.m[3][j];
    }
  }
  return m;
}
/* Multiply a 4x4 matrix with a 4d vector */
vec4_t m4x4v4_mul(m4x4_t a, vec4_t b) {
  vec4_t res;
  for (int i = 0; i < 4; i++) {
    res.v[i] =
      a.m[i][0] * b.v[0]
      + a.m[i][1] * b.v[1]
      + a.m[i][2] * b.v[2]
      + a.m[i][3] * b.v[3];
  }
  return res;
}
