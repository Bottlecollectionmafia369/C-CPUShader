//Vector math library (WIP)
//define VMATH_IMPL before including this header in *one* C file to create the implementation.

#include <math.h>

#define PI 3.1415926f

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

#define vec2_o(a) ((vec2_t){a, a})
#define vec2(a, b) ((vec2_t){a, b})

#define vec3_o(a) ((vec3_t){a, a, a})
#define vec3(a, b, c) ((vec3_t){a, b, c})

#define vec4_o(a) ((vec4_t){a, a, a, a})
#define vec4(a, b, c, d) ((vec4_t){a, b, c, d})

float saturate(float x);

float v2_dot(vec2_t a, vec2_t b);
float v2_length(vec2_t v);
vec2_t v2_add(vec2_t a, vec2_t b);
vec2_t v2_sub(vec2_t a, vec2_t b);
vec2_t v2_mul1(vec2_t a, float b);
vec2_t v2_normalize(vec2_t v);
vec2_t v2_reflect(vec2_t v, vec2_t n);
vec2_t v2_refract(vec2_t v, vec2_t n, float eta);

vec3_t v3_add(vec3_t a, vec3_t b);
vec3_t v3_sub(vec3_t a, vec3_t b);
vec3_t v3_mul(vec3_t a, vec3_t b);
vec3_t v3_mul1(vec3_t a, float b);
vec3_t v3_lerp(vec3_t a, vec3_t b, float t);
vec3_t v3_fract(vec3_t a);
float v3_dot(vec3_t a, vec3_t b);
vec3_t v3_normalize(vec3_t v);
vec3_t v3_reflect(vec3_t v, vec3_t n);

vec4_t v4_add(vec4_t a, vec4_t b);
vec4_t v4_mul(vec4_t a, vec4_t b);
vec4_t v4_mul1(vec4_t a, float b);
float v4_dot(vec4_t a, vec4_t b);
vec4_t v4_fract(vec4_t a);
vec4_t v4_sqrt(vec4_t v);

#ifdef VMATH_IMPL

float saturate(float x)
{
    return fmaxf(0.0f, fminf(1.0f, x));
}

float v2_dot(vec2_t a, vec2_t b)
{
    return a.x * b.x + a.y * b.y;
}

float v2_length(vec2_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

vec2_t v2_add(vec2_t a, vec2_t b)
{
    return vec2(a.x + b.x, a.y + b.y);
}
vec2_t v2_sub(vec2_t a, vec2_t b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

vec2_t v2_mul1(vec2_t a, float b)
{
    return vec2(a.x * b, a.y * b);
}

vec2_t v2_normalize(vec2_t v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y);
    return vec2(v.x / len, v.y / len);
}

vec2_t v2_reflect(vec2_t v, vec2_t n)
{
    float dot = v2_dot(v, n);
    return v2_sub(v, v2_mul1(n, 2.0f * dot));
}

vec2_t v2_refract(vec2_t v, vec2_t n, float eta)
{
    float dot = v2_dot(v, n);
    float k = 1.0f - eta * eta * (1.0f - dot * dot);
    float s = eta * dot + sqrtf(k);
    return vec2(eta * v.x - s * n.x, eta * v.y - s * n.y);
}

vec3_t v3_add(vec3_t a, vec3_t b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3_t v3_sub(vec3_t a, vec3_t b)
{
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3_t v3_mul(vec3_t a, vec3_t b)
{
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

vec3_t v3_mul1(vec3_t a, float b)
{
    return vec3(a.x * b, a.y * b, a.z * b);
}

vec3_t v3_lerp(vec3_t a, vec3_t b, float t)
{
    return vec3(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);
}

vec3_t v3_fract(vec3_t a)
{
    return vec3(a.x - floorf(a.x), a.y - floorf(a.y), a.z - floorf(a.z));
}

float v3_dot(vec3_t a, vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t v3_normalize(vec3_t v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return vec3(v.x / len, v.y / len, v.z / len);
}

vec3_t v3_reflect(vec3_t v, vec3_t n)
{
    float dot = v3_dot(v, n);
    return v3_sub(v, v3_mul1(n, 2.0f * dot));
}

vec4_t v4_add(vec4_t a, vec4_t b)
{
    return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

vec4_t v4_mul(vec4_t a, vec4_t b)
{
    return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

vec4_t v4_mul1(vec4_t a, float b)
{
    return vec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

float v4_dot(vec4_t a, vec4_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

vec4_t v4_fract(vec4_t a)
{
    return vec4(a.x - floorf(a.x), a.y - floorf(a.y), a.z - floorf(a.z), a.w - floorf(a.w));
}

vec4_t v4_sqrt(vec4_t v)
{
    return vec4(sqrtf(v.x), sqrtf(v.y), sqrtf(v.z), sqrtf(v.w));
}

#endif