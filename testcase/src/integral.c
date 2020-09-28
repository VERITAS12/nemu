#include "trap.h"
#include "FLOAT.h"
// FLOAT F_mul_F(FLOAT a, FLOAT b) {
// 	long long c = (long long)a * (long long)b;
// 	return (FLOAT)(c >> 16);
// }
FLOAT f(FLOAT x) { 
	/* f(x) = 1/(1+25x^2) */
	return F_div_F(int2F(1), int2F(1) + F_mul_int(F_mul_F(x, x), 25));
}

FLOAT computeT(int n, FLOAT a, FLOAT b, FLOAT (*fun)(FLOAT)) {
	int k;
	FLOAT s,h;
	h = F_div_int((b - a), n);
	s = F_div_int(fun(a) + fun(b), 2 );
	for(k = 1; k < n; k ++) {
		s += fun(a + F_mul_int(h, k));
	}
	s = F_mul_F(s, h);
	return s;
}

int main() { 
	FLOAT a = computeT(10, f2F(-1.0), f2F(1.0), f);
	FLOAT ans = f2F(0.551222);

	nemu_assert(Fabs(a - ans) < f2F(1e-4));
	

	// nemu_assert(0x00008000 == f2F(0.5));
	// nemu_assert(Fabs(11 - 10)< 2);
	// nemu_assert(f2F(0.001)<f2F(0.1));
	// nemu_assert(Fabs(f(0)- f2F(1))<f2F(0.1));
	// nemu_assert(Fabs(F_mul_F(0.2, 0.2))<0x00008000);
	// nemu_assert(0x00010000==f2F(1));
	// nemu_assert(Fabs(F_mul_int(0x00010000, 1) - 0x00010000)<0x00000800);
	// nemu_assert(F_mul_F(f2F(1), f2F(1))==f2F(1));
	// nemu_assert((((long long)0x10000 * (long long)0x10000)>>16)==0x00010000);
	// nemu_assert((((long long)0x10000 * (long long)0x10000)>>16)==F_mul_F(0x00010000, 0x00010000));

	// nemu_assert(Fabs(F_mul_F(0x00010000, 0x00010000) - 0x00010000)<0x00000800);
	// nemu_assert(Fabs(F_mul_F(f2F(1), f2F(1)) - f2F(1))<0x00000800);
	// nemu_assert(Fabs(F_mul_F(f2F(0.2), f2F(0.2)) - f2F(0.04))<0x00000800);
	// nemu_assert(Fabs(F_mul_int(F_mul_F(0.2, 0.2), 25) - f2F(1))<0x00008000);
	// nemu_assert(Fabs(f(0.2))<0x0000f000);
	// nemu_assert(Fabs(f(0.2))<f2F(0.6));
	// nemu_assert(Fabs(f(0.2))<f2F(0.5));
	// nemu_assert(Fabs(f(0.2)- f2F(0.5))<f2F(5));
	// nemu_assert(Fabs(F_mul_int(f(0.2), 1) - f2F(0.5))<f2F(0.1));
	return 0;
}
