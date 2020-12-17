#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PROTOPORT 5193
#define QLEN 6

pthread_mutex_t mut;
void* socketthread(void* parm);

int visits = 0;
int main(int argc,char* argv[])
{
	struct hostent *ptrh;
	struct protoent* ptrp;
	struct sockaddr_in sad;
	struct sockaddr_in cad;
	int sd,sd2;
	int port;
	int alen;
	pthread_t tid;
	pthread_mutex_init(&mut,NULL);
	memset((char*)&sad,0,sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = INADDR_ANY;
	if(argc>1){
		port = atoi(argv[1]);
	}else{
		port = PROTOPORT;
	}
	if(port>0)
		sad.sin_port = htons((u_short)port);
	else{
		fprintf(stderr,"bad port number %s\n",argv[1]);
		exit(1);
	}
	if( (ptrp = getprotobyname("tcp"))==0) {
		fprintf(stderr,"cannot map \"tcp\"to protocol number");
		exit(1);
	}
	sd = socket(PF_INET,SOCK_STREAM,ptrp->p_proto);
	if(sd<0){
		fprintf(stderr,"socket creation failed\n");
		exit(1);
	}

	if(bind(sd,(struct sockaddr*)&sad,sizeof(sad))<0){
		fprintf(stderr,"bind failed\n");
		exit(1);
	}
	if(listen(sd,QLEN)<0){
		fprintf(stderr,"listen failed\n");
		exit(1);
	}
	alen = sizeof(cad);
	fprintf(stderr,"Server up and running.\n");
	while(1){
		printf("SERVER: Waiting for concat ...\n");

		if( (sd2 = accept(sd,(struct sockaddr*)&cad,&alen))<0){
			fprintf(stderr,"accept failed\n");
			exit(1);
		}
	}
	exit(1);
}

void* socketthread(void* parm){
	int tsd,tvisits;
	char buf[100];
	pthread_mutex_lock(&mut);
	tvisits = ++visits;
	pthread_mutex_unlock(&mut);

	sprintf(buf,"This server has been contacted %d time%s\n",tvisits,tvisits ==1?".":"s.");
	printf("SERVER thread: %s",buf);
	send(tsd,buf,strlen(buf),0);
	pthread_exit(0);
}


