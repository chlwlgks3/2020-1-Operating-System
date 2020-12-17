#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // 리눅스에서 스레드를 생성하기위해 그리고 mutex를 이용하기위해 					 // pthread.h의 헤더파일포함
#include <unistd.h> // mutex를 사용하기 위해 필요한 헤더
pthread_mutex_t mutex; // pthread_mutex_t 구조체변수 인 mutext객체 선언
volatile int iNum; // 언제든지 값이 변경될 수 있으니 변수의 위치를 외장메인메모리에 				   // 위치시키는 키워드인 volatile을 이용하여 전역변수 iNum선언
void* add(); // add()함수 선언
void* sub(); // sub()함수 선언
int swap(int x); // 전역변수 값이 깨질 확률을 더 높일수있는 swap()함수 선언!!!

// int argc: 메인함수에 전달되는 데이터의 갯수
// char* argv[]: 메인함수에 전달되는 실제적인 데이터로 char형 포인터배열로 구성
//               즉 문자열의 배열이다. 첫번째 문자열은 프로그램의 실행경로!
int main(int argc,char* argv[]){
	pthread_t tid[10]; // 10개의 스레드를 만들것이므로 pthread_t 배열변수선언 	
	pthread_mutex_init(&mutex,NULL); // mutex를 사용하기전에 첫번째인자로 주어진
									 // mutex초기화
	for(int i=0;i<30;i++){
		iNum = atoi(argv[1]); // 입력한 값은 문자열이므로 atoi를 이용하여 int로 변환
							  // atoi는 입력한 ASCII값을 int로 바꿔주는 함수이다
		for(int j=0;j<5;j++) //5개의 add() thread생성
			if(pthread_create(&tid[j],NULL,add,NULL)<0){ //pthread_create()함수는 
				//성공시에 thread초기화함과 동시에 0반환하고 오류시에 에러값반환
				//오류발생시 오류메세지 출력후 exit(1)실행
				fprintf(stderr,"add() thread 생성도중 오류발생\n");
				exit(EXIT_FAILURE);
			}
		for(int j=5;j<10;j++) //5개의 sub() thread생성
			if(pthread_create(&tid[j],NULL,sub,NULL)<0){
				fprintf(stderr,"sub() thread 생성도중 오류발생\n");
				exit(EXIT_FAILURE);
			}
	 	for(int j=0;j<10;j++) //tid[0]~tid[9]의 스레드가 종료될때까지 기다린다
			pthread_join(tid[j],NULL); 
		printf("iNum = %d\n",iNum); // 모든스레드가 종료된후 전역변수 iNum값 출력
	}
	pthread_mutex_destroy(&mutex); // 사용한 mutex 제거
	return 0;
}

int swap(int x){ // 전역변수 값이 깨질 확률을 높이기 위해 함수swap 안에서 int자료형				     // temp를 선언하고 함수의 인자로 전달받은 x의 값을 temp에 복사하고
				 // 다시 x에 temp를 저장하는 함수 swap()정의 
	int temp;
	for(int i=0;i<500;i++){
		temp = x; // 임시변수 temp에 x저장
		x = temp; // 다시 x에 temp에 저장되어있는 x의 값 저장
	}
	return x;
}

void* add(){ // 전역변수 iNum에 1을 10번 반복하여 더하는 함수 add()정의
			 // 반환형이 void* 인 이유는 pthread_create()함수의 인자로 쓰기위함
	int i;
	pthread_mutex_lock(&mutex); // Critical Section에 진입하기전 이 코드구역을 잠금
								// 으로써 공유되는 데이터(iNUm)를 보호한다
	for(i=0;i<1000;i++){		
		iNum = iNum + 1;
		iNum = swap(iNum);
	}
	pthread_mutex_unlock(&mutex); // Critical Section이 끝나면 다시 풀어서 다음 
								  // 스레드가 진입할 수 있도록 한다
}

void* sub(){ // 전역변수 iNum에 1을 10번 반복하여 빼는 함수 sub()정의
			 // 반환형이 void* 인 이유는 pthread_create()함수의 인자로 쓰기위함
	int i;
	pthread_mutex_lock(&mutex); // add()함수에서와 마찬가지로 임계구역에 진입하기전
								// 이 코드구역을 잠궈서 공유되는 데이터를 보호한다
	for(i=0;i<1000;i++){
		iNum = iNum - 1;
		iNum = swap(iNum);
	}
	pthread_mutex_unlock(&mutex); // 임계구역이 끝났으므로 다시 풀어서 다음 스레드가
								  // 가 진입할 수 있도록 한다.
}

