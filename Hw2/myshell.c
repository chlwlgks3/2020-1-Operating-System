#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
	pid_t pid;
	char buffer[100]; //입력한 문자열을 저장할 변수선언
	char* a; //인수를 저장할 변수
	char* b; //인수를 저장할 변수
	//4가지 실행파일중 어떤것을 실행할것인지 물어보는 문장출력
	printf("you can enter one from the following.a and b is the argument(int).\nsum.exe a b\nsub.exe a b\nfact.exe a\ndiv.exe a b\n");
	printf("======================\n");
	while(1){ //실행파일을 한번 실행한 후 계속해서 실행파일을 실행할 수 있게
		//무한반복
		fgets(buffer,100,stdin); //사용자가 입력한 한줄을 읽어들인다
		char* tok = strtok(buffer," "); //문자열buffer를 공백전까지 토큰 단위로 읽어										//문자열변수tok에 저장 
		pid = fork();
		if(pid == 0){	//프로세스ID가 0인경우만
			if(!strcmp(tok,"sum.exe")){
				a = strtok(NULL," "); //sum.c의 인수는 2개 
				b = strtok(NULL," ");
				execl("/home/doorumi/sum.exe","/home/doorumi/sum.exe",a,b,NULL);
			}
			else if(!strcmp(tok,"sub.exe")){
				a = strtok(NULL," "); //sub.c의 인수는 2개
				b = strtok(NULL," ");
				execl("/home/doorumi/sub.exe","/home/doorumi/sub.exe",a,b,NULL);
			}
			else if(!strcmp(tok,"fact.exe")){
				a = strtok(NULL," "); //fact.c의 인수는 1개
				execl("/home/doorumi/fact.exe","/home/doorumi/fact.exe",a,NULL);
			}
			else if(!strcmp(tok,"div.exe")){
				a = strtok(NULL," "); //div.c의 인수는 2개
				b = strtok(NULL," ");
				execl("/home/doorumi/div.exe","/home/doorumi/div.exe",a,b,NULL);
			}
		}
		else if(pid < 0 ) //pid가 -1인경우 에러처리
			printf("error\n");
	}
	return 0;
}
			
