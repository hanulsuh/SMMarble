//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR		100
#define MAX_NODETYPE    7

#define GRADE_AP		0	//A+
#define GRADE_A0		1	//A0
#define GRADE_AM		2	//A-
#define GRADE_BP		3	//B+
#define GRADE_B0		4	//B0
#define GRADE_BM		5	//B-
#define GRADE_CP		6	//C+
#define GRADE_C0		7	//C0
#define GRADE_CM		8	//C-
#define GRADE_DP		9	//D+
#define GRADE_D0		10	//D0
#define GRADE_DM		11	//D-
#define GRADE_F			12	//F

//노드 타입 이름을 문자열로 나타내기
static char smmObj_NodeName[MAX_NODETYPE][MAX_CHARNAME] = {
	"lecture",
	"restaurant",
	"laboratory",
	"home",
	"gotoLab",
	"foodChance",
	"festival"
};

//성적을 문자열로 나타내기
static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
	"A+",
	"A0",
	"A-",
	"B+",
	"B0",
	"B-",
	"C+",
	"C0",
	"C-",
	"D+",
	"D0",
	"D-",
	"F"
};


//구조체 타입 정의

//1. 일반 객체 구조체
typedef struct {
	char name[MAX_CHARNAME];
	int objType; 	
	int type;		
	int credit;
	int energy;	
	int grade;
} smmObj_object_t;

//2. 음식 카드 구조체
typedef struct { 
    char name[MAX_CHARNAME];
    int energy;
} smmObj_food_t;

//3. 축제 카드 구조체
typedef struct {
	char name[MAX_CHARNAME];
} smmObj_fest_t;


// 객체 생성 함수

//1. 일반 객체 생성
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade) 
{
	smmObj_object_t* ptr;
	ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
	
	strcpy(ptr->name, name);
    ptr->type = type;
    ptr->objType = objType;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ((void*)ptr);
}

//2. 음식 카드 객체 생성
void* smmObj_genFood(char* name, int energy)
{   
    smmObj_food_t* ptr;
    
    ptr = (smmObj_food_t*)malloc(sizeof(smmObj_food_t));

    strcpy(ptr->name, name);
    ptr->energy = energy;
    
    return ((void*)ptr);
}

//3. 축제 카드 객체 생성
void* smmObj_genFest(char* name)
{
	smmObj_fest_t* ptr;
	ptr = (smmObj_fest_t*)malloc(sizeof(smmObj_fest_t));
	
	strcpy(ptr->name, name);
	
	return (void*)ptr;
}


//노드 관련 정보를 가져오는 함수

//1. 객체 이름 반환
char* smmObj_getObjectName(void *ptr)	 	 		
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->name);
}

//2. 객체 에너지 반환
int smmObj_getObjectEnergy(void *ptr)		 		
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->energy);
}

//3. 객체 타입 번호 반환
int smmObj_getObjectType(void *ptr)					
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->type);
}

//4. 객체 타입 문자열 반환
char* smmObj_getTypeName(int node_type)   	 		
{
	
	return (smmObj_NodeName[node_type]);
}

//5. 객체 학점 반환
int smmObj_getObjectCredit(void *ptr)				
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->credit);
}

//6. 객체 성적 반환
int smmObj_getObjectGrade(void *ptr)			
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->grade);
}

//7. 음식 카드 이름 반환
char* smmObj_getFoodName(void *ptr)
{
    smmObj_food_t* foodPtr = (smmObj_food_t*)ptr;

    return (foodPtr->name);
}

//8. 음식 카드 에너지 반환
int smmObj_getFoodEnergy(void *ptr)
{
    smmObj_food_t* foodPtr = (smmObj_food_t*)ptr;

    return (foodPtr->energy);
}

//9. 축제 카드 이름 반환
char* smmObj_getFestName(void *ptr)
{
	smmObj_fest_t* festPtr = (smmObj_fest_t*)ptr;
	
	return (festPtr->name);
}

#if 0
//element to string
char* smmObj_getNodeName(smmNode_e type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif
