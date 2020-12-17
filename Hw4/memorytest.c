#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h> //va_start(),vsprintf(),va_end()등등을 사용하기위한 헤더파일
#define ONE_MEG 1048576 //1MB = 1048576B
#define STACK_ALLOC ONE_MEG/20
#define BASE 16  //메모리주소를 16진수로 표현할것이므로 BASE는 16으로 정의
#define GROUP 4  //4자리씩 ,로 끊어서 출력할것이므로 GROUP을 4로 정의
#define MAXTEXT 25
long* MemoryPtr; //힙영역에 malloc으로 동적할당받은 후 그 주소를 저장할 변수 선언
unsigned long FirstStackLocation; //스택영역에 할당한 최초의 주소를 저장할 변수
void RecursiveRoutine(int RecursiveDepth); //스택영역에서 반복하여 호출할 재귀함수선언
char* commas(unsigned long amount); //amount인자를 16진수형식으로 4자리씩 끊은 문자열의 
									//시작주소를  반환하는 함수선언
//commas()함수에서 사용될 함수들 선언
int prepend(char* buf,unsigned offset,char* new_str); 
int preprintf(char* buf,unsigned offset,char* format,...); 

int main(int argc,char* argv[]){
	int TopOfStack; //스택영역에 저장될 첫번째 변수선언 
	long NumberOfMegaBytes, NumberOfAllocations = 0;
	long Temp; //TempPointer[0] 저장할 변수
	long *TempPointer; //동적할당후 반환된 시작주소를 저장할 포인터변수
	char MemoryFunction[32]; //인자로 입력한 문자열을 저장할 char배열 선언
	int Function = 0; //인자로 Read,Write,Read를 전달하지않을경우 변하지않고 0으로 유지됨

	//heap.c
	if(argc<3){ //프로그램실행시 전달한 인자가 2개보다 작은경우
				//기본적으로 첫번째 인자  argv[0]는 프로그램의 경로이다
		printf("Usage:%s<MemorySizeInMegabyte><Read|Write|Nothing>\n",argv[0]);
		exit(1); 
	}

	NumberOfMegaBytes = atoi(argv[1]); //전달한 첫번째인자가 문자열이므로 정수형으로 변경
	strcpy(MemoryFunction, argv[2]); //전달한 두번째문자열을 MemoryFunction에 복사
	if( strncmp(MemoryFunction,"Nothing",7)==0 ) Function = 1;
	if( strncmp(MemoryFunction,"Read",4)==0 ) Function = 2;
	if( strncmp(MemoryFunction,"Write",5) ==0) Function = 3;
	if( Function == 0){ //Function이 1,2,3으로 변하지않고 처음값 0그대로라면 오류출력
		printf("Unable to recognize the Read|Write|Nothing portion of the command\n");
		return 0;
	}
	while(NumberOfAllocations < NumberOfMegaBytes){ //인수로 입력해준 megabyte의 수만큼
													//계속 할당해준다 
		MemoryPtr = (long*)malloc(ONE_MEG); //MemoryPtr에 1megabyte크기 동적할당후 주소반환
											//동적할당 실패시 0을 주소대신 0반환
		if(NumberOfAllocations ==0) printf("First location on heap:%s\n",
				commas((unsigned long)MemoryPtr)); //힙영역에 첫번째 동적할당후 그 주소출력
		if(MemoryPtr == 0){ //더이상 heap영역에 메모리를 동적으로 할당할수 없다면 
			printf("The program is ending because we could allocate no more memory.\n");
			//Heap영역이 가득찼을경우 총 몇 MB를 할당한지 출력
			printf("Total megabytes allocated = %ld\n",NumberOfAllocations);
			exit(0); // 코드0 반환하고 프로그램 종료시킨다
		}
		NumberOfAllocations++; //동적할당한 갯수 1개증가시킴
		if( (NumberOfAllocations % 2000) == 0 ) //2000개동적할당할때마다 몇MB할당한지 출력 
			printf("We have allocated %ld Megabytes\n", NumberOfAllocations);
		TempPointer = MemoryPtr; //TempPointer에 동적할당한 MemoryPtr의 주소저장
		if(Function == 2){ // Read from each page in the megabyte
			while( (unsigned long)TempPointer < (unsigned long)( (long)MemoryPtr+ONE_MEG)){
				Temp = TempPointer[0];
				TempPointer = (long*)((unsigned long)TempPointer + 4096);//4KB 더함
			}
		}
		if(Function == 3){ // Write to each page in the megabyte
			while( (unsigned long)TempPointer < (unsigned long)((long)MemoryPtr + ONE_MEG)){
				TempPointer[0] = Temp;
				TempPointer = (long*)((unsigned long)TempPointer + 4096); //4KB 더함 
			}
		}
	}
	//stack.c
	FirstStackLocation = (unsigned long)(&TopOfStack); //처음으로스택에 저장될 지역변수
													   //의 주소를 저장
	printf("First location on stack: %s\n",commas((unsigned long)FirstStackLocation));
	RecursiveRoutine(0); //스택영역에 계속해서 쌓일 재귀함수 호출
}

void RecursiveRoutine(int RecursiveDepth){ 
	char Temp[STACK_ALLOC]; //함수가 호출될때마다 계속하여 생성될 문자열배열
	char StringTop[32];
	char StringBottom[32];
	strcpy(StringTop,commas((unsigned long)(FirstStackLocation)));
	strcpy(StringBottom,commas((unsigned long)&(Temp[STACK_ALLOC])));
	//첫번째 반복이후 1000번째 반복때마다 출력실행
	if(RecursiveDepth != 0 && RecursiveDepth % 1000 ==0)
		printf("Iteration = %3d: StackTop/Bottom/Bytes: %s %s %ld\n",RecursiveDepth,StringTop,StringBottom,FirstStackLocation-(unsigned long)&(Temp[STACK_ALLOC]));
	RecursiveDepth++; 
	RecursiveRoutine(RecursiveDepth); //자기자신 다시 호출
}

char* commas(unsigned long amount){ //보고서2.1에서 설명
	short i;
	short offset = MAXTEXT -1;
	short place;
	static char text[MAXTEXT];
	for(i = 0;i<MAXTEXT;i++)
		text[i] = '\0'; // char배열 text의 모든요소를 널문자로 초기화 
	for(place = 0;amount>0;++place){ 
		if(place % GROUP == 0 && place>0) 
			offset = prepend(text,offset,",");
		offset = preprintf(text,offset,"%X",amount% BASE);
		amount /= BASE;
	}
	return (offset >=0) ? text+offset : NULL;
}

int prepend(char* buf,unsigned offset, char* new_str){ //보고서2.1에서 설명
	int new_len = strlen(new_str);
	int new_start = offset - new_len;
	if(new_start >= 0)
		memcpy(buf+new_start,new_str,new_len);
	return new_start;
}

int preprintf(char* buf,unsigned offset,char* format,...){ //보고서2.1에서 설명
	int pos = offset;
	char* temp = malloc(BUFSIZ);

	if(temp){
		va_list args;
		va_start(args,format);
		vsprintf(temp,format,args);
		pos = prepend(buf,offset,temp);
		va_end(args);
		free(temp);
	}
	return pos;
}

		
	
