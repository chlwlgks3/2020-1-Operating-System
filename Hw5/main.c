#include <stdio.h>
#include <string.h>
#include <fcntl.h> //open()�Լ��� close()�Լ��� ����ϱ����� �������
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
	//���ο� ���ϱ���ڸ� ������ �� ���ϰ��path�� ���ϰ� �����Ų��. ������ ������ ������ ���ϱ���� ��ȯ�ϰ�
	//�׷����ʴٸ� -1����ȯ�� 1
	if(input == 'B'){
		printf("on 1 off 0\n");
		scanf("%s",num);
		write(fd,num,strlen(num),NULL);//�ڷḦ device�� ������-> 0�ΰ�� ���� 1�ΰ�� Ų��
		memset(buf,0,BUFSIZ);//�޸��ʱ�ȭ
		read(fd,buf,strlen(num),NULL); //device�κ��� �ڷḦ �о����
	}
	if(input == 'A'){
		for(int j=0;j<3;j++){ //led on,off�� 3�� �ݺ�
			write(fd,"1",1,NULL); //�Ҵ�
			memset(buf,0,BUFSIZ);
			read(fd,buf,1,NULL);
			usleep(500000); //0.5�ʵ��� �һ��·� ���
			write(fd,"0",1,NUL); //����
			memset(buf,0,BUFSIZ);
			read(fd,buf,1,NULL);
			usleep(500000);
		}
	}
	close(fd); 
}
