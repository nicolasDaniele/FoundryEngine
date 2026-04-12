#pragma once

#include <cmath>
#include <cfloat>

#define RAD2DEG(x) ((x) * 57.295754f)
#define DEG2RAD(x) ((x) * 0.0174533f)

#define COS(x) cosf(x)
#define SIN(x) sinf(x)
#define TAN(x) tanf(x)
#define ACOS(x) acos(x)

#define POW(x, y) powf(x, y)
#define SQRT(x) sqrtf(x)

#define CMP(x, y) (fabsf((x)-(y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))