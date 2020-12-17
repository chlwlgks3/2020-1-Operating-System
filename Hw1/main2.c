#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//definition of struct which has information about student and struct pointer link
typedef struct _Student{
	char name[10];
	char number[10];
	struct _Student* link;
}Student;

//declaration of function
char* surf(Student* head,char* search);

int main(void){
	Student* head; //pointer pointing to first node
	//10 pointer to store information 
	Student *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10; 
	char search[10]; //array to contain search content
	
	//memory allocation of 10 pointer
	p1 = (Student*)malloc(sizeof(Student));
	p2 = (Student*)malloc(sizeof(Student));
	p3 = (Student*)malloc(sizeof(Student));
	p4 = (Student*)malloc(sizeof(Student));
	p5 = (Student*)malloc(sizeof(Student));
	p6 = (Student*)malloc(sizeof(Student));
	p7 = (Student*)malloc(sizeof(Student));
	p8 = (Student*)malloc(sizeof(Student));
	p9 = (Student*)malloc(sizeof(Student));
	p10 = (Student*)malloc(sizeof(Student));
	head = p1;
	//entering student's information into the structure pointed by pointer
	strcpy(p1->name,"Johnson"); strcpy(p1->number,"1111");
	strcpy(p2->name,"Michael"); strcpy(p2->number,"2222");
	strcpy(p3->name,"Bush"); strcpy(p3->number,"3333");
	strcpy(p4->name,"Obama"); strcpy(p4->number,"4444");
	strcpy(p5->name,"Trump"); strcpy(p5->number,"5555");
	strcpy(p6->name,"Jolie"); strcpy(p6->number,"6666");
	strcpy(p7->name,"Benzamin"); strcpy(p7->number,"7777");
	strcpy(p8->name,"Lotus"); strcpy(p8->number,"8888");
	strcpy(p9->name,"Amanda"); strcpy(p9->number,"9999");
	strcpy(p10->name,"Linkon"); strcpy(p10->number,"1010");
	//each pointer points to the next node
	p1->link = p2; p2->link = p3; p3->link = p4; p4->link = p5; p5->link = p6;p6->link = p7; p7->link = p8;p8->link = p9; p9->link = p10; p10->link = NULL;
	
	printf("search: ");
	scanf("%s", search);
	printf("--> %s", surf(head,search));
	//disable all pointer memory allocation
	free(p1); free(p2); free(p3); free(p4); free(p5); free(p6); free(p7); free(p8); free(p9); free(p10);

	return 0;
}

//definition of function which returns string number or name if found search  in the linked list with the pointer head pointing to the first node 
char* surf(Student* head,char* search){
	Student* p;
	p = head;
	while(p != NULL){
		if(!strcmp(search,p->name))
			return p->number;
		else if(!strcmp(search,p->number))
			return p->name;
		else
			p = p->link; //if not found point to the next node
	}
	exit(EXIT_FAILURE); //if linked list does not have string search exit program
}


