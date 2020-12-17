#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[]){
	int a = atoi(argv[1]);
	int sum=1;
	for(int i=1;i<=a;i++)
		sum *= i;
	printf("%d\n",sum);
	return 0;
}
