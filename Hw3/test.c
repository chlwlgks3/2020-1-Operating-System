#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // 리눅스에서 스레드를 생성하기위해 pthread.h의 헤더파일 포함

volatile int iNum; // 언제든지 값이 변경될 수 있으니 변수의 위치를 외장메인메모리에 				   // 위치시키는 키워드인 volatile을 이용하여 전역변수 iNum선언
void* add(); // add()함수 선언
void* sub(); // sub()함수 선언
// int argc: 메인함수에 전달되는 데이터의 갯수
// char* argv[]: 메인함수에 전달되는 실제적인 데이터로 char형 포인터배열로 구성
//               즉 문자열의 배열이다. 첫번째 문자열은 프로그램의 실행경로!
int main(int argc,char* argv[]){
	pthread_t tid[10000]; // 10000개의 스레드를 만들것이므로 pthread_t 배열변수선언
	for(int i=0;i<30;i++){
		iNum = atoi(argv[1]); // 입력한 값은 문자열이므로 atoi를 이용하여 int로 변환
							  // atoi는 입력한 ASCII값을 int로 바꿔주는 함수이다
		for(int j=0;j<5000;j++) //5000개의 add() thread생성
			if(pthread_create(&tid[j],NULL,add,NULL)<0){ //pthread_create()함수는 
				//성공시에 thread초기화함과 동시에 0반환하고 오류시에 에러값반환
				//오류발생시 오류메세지 출력후 exit(1)실행
				fprintf(stderr,"add() thread 생성도중 오류발생\n");
				exit(EXIT_FAILURE);
			}
		for(int j=5000;j<10000;j++) //5000개의 sub() thread생성
			if(pthread_create(&tid[j],NULL,sub,NULL)<0){
				fprintf(stderr,"sub() thread 생성도중 오류발생\n");
				exit(EXIT_FAILURE);
			}
	 	for(int j=0;j<10000;j++) //tid[0]~tid[9999]의 스레드가 종료될때까지 기다린다
			pthread_join(tid[j],NULL); 
		printf("iNum = %d\n",iNum); // 모든스레드가 종료된후 전역변수 iNum값 출력
	}
	return 0;
}
void* add(){ // 전역변수 iNum에 1을 10번 반복하여 더하는 함수 add()정의
			 // 반환형이 void* 인 이유는 pthread_create()함수의 인자로 쓰기위함
	int i;
	for(i=0;i<1000;i++)
		iNum = iNum + 1;
}
void* sub(){ // 전역변수 iNum에 1을 10번 반복하여 빼는 함수 sub()정의
			 // 반환형이 void* 인 이유는 pthread_create()함수의 인자로 쓰기위함
	int i;
	for(i=0;i<1000;i++)
		iNum = iNum - 1;
}

