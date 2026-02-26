#include "math.h"

// variables
SceKernelUtilsMt19937Context ctx;
u32 seed;

float absD(float f) {
    if(f < 0)
        return -f;
    else
        return f;
}

int nextPowTwo(int n){
    int r = 1;
    while(r < n){
        r <<= 1; // r *= 2;
    }
    return r;
}

void initRandom(){
    // time as a seed for random numbers
    seed = sceKernelLibcTime(NULL);
    sceKernelUtilsMt19937Init(&ctx, seed);
}

u32 randomD(int min, int max){
    u32 r = sceKernelUtilsMt19937UInt(&ctx);
    return (r % (max - min + 1)) + min;
}