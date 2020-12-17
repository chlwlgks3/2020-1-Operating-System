#include <stdio.h>
#include <string.h>

//definition of struct which has information about student
typedef struct _Student{
	char name[10]; 
	char number[10];
}Student;

//declaration of function surt()
char* surf(Student* arr,char* search);

int main(void){
	char search[10]; //array to contain search content
	Student arr[10] = {{"Johnson","1111"},{"David","2222"},                    {"Michale","3333"},{"Charlie","4444"},{"Obama","5555"},                    {"Thrump","6666"},{"Julie","7777"},{"Hapbon","8888"},                      {"Joly","9999"},{"Manson","1010"}}; //array of 10 student
	
	printf("SEARCH: ");
	scanf("%s", search);
	printf("--> %s", surf(arr,search));

	return 0;
}

//definition of fuction which returns string number or name  if found search  in array
char* surf(Student* arr,char* search){
	int i=0; //i is index of array
	do{
		if(!strcmp(search,arr[i].name))
			return arr[i].number;
		else if(!strcmp(search,arr[i].number))
			return arr[i].name;
		else
			i++; //if not found i=i+1 and repeat
	}while(1);
}
