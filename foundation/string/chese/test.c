#include <stdio.h>
char chese[20] = "下一站";
int main()
{
	printf("%s\n", chese);
	int i = 0;
	for (i; i < 20; i++) {
		printf("0x%x ", chese[i]&0xff);
	}
	putchar('\n');
	return 0;
}
