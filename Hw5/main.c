#include <stdio.h>
#include <string.h>
#include <fcntl.h> //open()함수와 close()함수를 사용하기위한 헤더파일
#include <stdlib.h>

void main(){
	char buf[BUFSIZ];
	char i = 0;
	int fd = -1;
	char input;
	char num[5];
	fd =open("/dev/ledtest", O_RDWR);
	printf("Enter A or B\n");
	scanf("%c",&input);
	//int open(const char* path,int oflag,...);
	//새로운 파일기술자를 생성한 후 파일경로path의 파일과 연결시킨다. 파일이 열리면 생성된 파일기술자 반환하고
	//그렇지않다면 -1을반환함 1
	if(input == 'B'){
		printf("on 1 off 0\n");
		scanf("%s",num);
		write(fd,num,strlen(num),NULL);//자료를 device로 보낸다-> 0인경우 끄고 1인경우 킨다
		memset(buf,0,BUFSIZ);//메모리초기화
		read(fd,buf,strlen(num),NULL); //device로부터 자료를 읽어들임
	}
	if(input == 'A'){
		for(int j=0;j<3;j++){ //led on,off를 3번 반복
			write(fd,"1",1,NULL); //켠다
			memset(buf,0,BUFSIZ);
			read(fd,buf,1,NULL);
			usleep(500000); //0.5초동안 켠상태로 대기
			write(fd,"0",1,NUL); //끈다
			memset(buf,0,BUFSIZ);
			read(fd,buf,1,NULL);
			usleep(500000);
		}
	}
	close(fd); 
}
