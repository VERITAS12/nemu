#include "trap.h"

long long qpow(long long a, long long n, long long mod){
	long long re = 1;
	while(n){
		if(n&1){
			re=(re*a);
			while(re>=mod)re-=mod;
		}
		n>>=1;
		a=(a*a);
		while(a>=mod)a-=mod;
	}while(re>=mod)re-=mod;
	return re;
}
int main(){
	/* code */
	nemu_assert(qpow(120, 1901, 31)==27);
	return 0;
}