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

#define GRADE_AP		0
#define GRADE_A0		1
#define GRADE_AM		2
#define GRADE_BP		3
#define GRADE_B0		4
#define GRADE_BM		5
#define GRADE_CP		6
#define GRADE_C0		7
#define GRADE_CM		8
#define GRADE_DP		9
#define GRADE_D0		10
#define GRADE_DM		11
#define GRADE_F			12

//node type name definition
static char smmObj_NodeName[MAX_NODETYPE][MAX_CHARNAME] = {
	"lecture",
	"restaurant",
	"laboratory",
	"home",
	"gotoLab",
	"foodChance",
	"festival"
};

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


//구조체 타입 정의 (노드, 음식, 축제)
typedef struct {
	char name[MAX_CHARNAME];
	int objType; 	//board or grade or food or festival
	int type;		//board node type : lecture or ...
	int credit;
	int energy;	
	int grade;
} smmObj_object_t;

typedef struct { 
    char name[MAX_CHARNAME];
    int energy;
} smmObj_food_t;

typedef struct {
	char name[MAX_CHARNAME];
} smmObj_fest_t;


//object generation
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

void* smmObj_genFood(char* name, int energy)
{   
    smmObj_food_t* ptr;
    
    ptr = (smmObj_food_t*)malloc(sizeof(smmObj_food_t));

    strcpy(ptr->name, name);
    ptr->energy = energy;
    
    return ((void*)ptr);
}

void* smmObj_genFest(char* name)
{
	smmObj_fest_t* ptr;
	ptr = (smmObj_fest_t*)malloc(sizeof(smmObj_fest_t));
	
	strcpy(ptr->name, name);
	
	return (void*)ptr;
}


//노드 관련 정보를 가져오는 함수
char* smmObj_getObjectName(void *ptr)	 	 		//name
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->name);
}

int smmObj_getObjectEnergy(void *ptr)		 		//energy
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->energy);
}

int smmObj_getObjectType(void *ptr)					//type number
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->type);
}

char* smmObj_getTypeName(int node_type)   	 		//type name (enum to string)
{
	
	return (smmObj_NodeName[node_type]);
}

int smmObj_getObjectCredit(void *ptr)				//credit
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->credit);
}

int smmObj_getObjectGrade(void *ptr)				//player grade
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->grade);
}

//음식 관련 정보를 가져오는 함수
char* smmObj_getFoodName(void *ptr)
{
    smmObj_food_t* foodPtr = (smmObj_food_t*)ptr;

    return (foodPtr->name);
}

int smmObj_getFoodEnergy(void *ptr)
{
    smmObj_food_t* foodPtr = (smmObj_food_t*)ptr;

    return (foodPtr->energy);
}

//축제 관련 정보를 가져오는 함수
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
