#ifndef MATH_H_ //  include guard
#define MATH_H_

// libraries
#include <psputils.h>
#include <time.h>
#include <pspkernel.h>


// c++ guard
#ifdef __cplusplus
extern "C" {
#endif

//constants
#define INV_ROOT_2 0.70710678118
#define ROOT_2 1.41421356237

// functions
float absD(float f);
int nextPowTwo(int n);
void initRandom();
u32 randomD(int min, int max);

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif // MATH_H_
