#ifndef FIXEDPOINTMATH_H
#define FIXEDPOINTMATH_H

#define FP	16	// don't touch this, m'kay

#define FP_ONE		(1<<FP)
#define FP_INTMASK	(~((1<<FP)-1))
#define FP_FRACMASK	((1<<FP)-1)
#define FP_MIN_DIVISOR	(1<<6)

typedef int scalar;

const scalar PI = 205887;
const scalar PI_OVER_2 = (PI/2);
const scalar E = 178145;
const scalar HALF = 2<<15;

inline scalar FPInt(int i);
inline scalar FPMul(scalar a, scalar b);
inline scalar FPDiv(scalar a, scalar b);
inline scalar FPSqrt(scalar a);
scalar FPSin(scalar f);
scalar FPCos(scalar f);
scalar FPTan(scalar f);
scalar FPArcTan(scalar f);
scalar FPArcSin(scalar f);
scalar FPArcCos(scalar f);

inline scalar FPInt(int i)
{
	return i << FP;
}

inline scalar FPMul(scalar a, scalar b)
{
	return ((a>>6) * (b>>6))>>4;
//	return ((a>>7) * (b>>5))>>4;
//	return (a * b)>>FP;
}

inline scalar FPDiv(scalar a, scalar b)
{
#if 0 // make sure we crash on zero division
	if (b < (1<<6) && b > -(1>>6))
	{
		*((char*)(0)) = 0xdeadbeef;
		return 0x80000000;
	}
#endif	
	return ((a<<6) / (b>>6))<<4;
//	return ((a<<7) / (b>>5))<<4;
//	return (a / b)<<FP;
}

inline scalar FPSqrt(scalar a)
{
	int r = (a + (1<<FP))>>1;
	int i;

	for (i=0; i<8; i++)
	{
		r = (r + FPDiv(a, r)) >> 1;
	}
	return r;
}

// signed modulo
inline scalar FPMod(scalar a, scalar m)
{
	scalar s = a % m;
	
	if (a<0)
		s+=m;
	
	return s;
}

#endif
