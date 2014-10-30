#include<stdio.h>

int main() {
	double d = 123456789.987654321;
	printf("d:%lf\n", d);
	long l0 = (long)d;
	double d2 = d - l0;
		
	printf("l0:%ld\n", l0);
	printf("d2:%lf\n", d2);

	d2 *= 10000000;
	
	printf("d2:%lf\n", d2);
}


