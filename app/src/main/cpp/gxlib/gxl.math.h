#ifndef GXLIB_GXL_MATH_H_
#define GXLIB_GXL_MATH_H_

/*
 * Math constants
 */
const float PI = 3.1415926535897932384626433832795f;
extern float SIN_TABLE_FLT[256];
extern float COS_TABLE_FLT[256];
extern fix32 SIN_TABLE_FIX[256];
extern fix32 COS_TABLE_FIX[256];

#define iter1(N) \
    tval = root + (1 << (N)); \
    if (n >= tval << (N))   \
{   n -= tval << (N);   \
	root |= 2 << (N); \
}

inline uint32 int_sqrt(uint32 n)
{
    uint32 root = 0, tval;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root >> 1;
}
#undef iter1

void InitMath();


#endif //GXLIB_GXL_MATH_H_
