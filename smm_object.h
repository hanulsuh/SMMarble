//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/

//1. 노드 타입 정의
#define SMMNODE_TYPE_LECTURE 			0	//강의 노드
#define SMMNODE_TYPE_RESTAURANT			1	//식당 노드
#define SMMNODE_TYPE_LABORATORY 		2	//실험실 노드
#define SMMNODE_TYPE_HOME 				3	//집 노드
#define SMMNODE_TYPE_GOTOLAB 			4	//실험실 이동 노드
#define SMMNODE_TYPE_FOODCHANCE 		5	//음식 찬스 노드
#define SMMNODE_TYPE_FESTIVAL			6	//축제 노드

//2. 객체 타입 정의
#define SMMNODE_OBJTYPE_BOARD   0	//보드 칸
#define SMMNODE_OBJTYPE_GRADE   1	//성적
#define SMMNODE_OBJTYPE_FOOD 	2	//음식 카드
#define SMMNODE_OBJTYPE_FEST	3	//축제 카드

//최대 성적
#define SMMNODE_MAX_GRADE       13

/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-,
    D+,
    D0,
    D-,
    F
*/

//3. 함수 선언

//객체 생성 함수
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade);
void* smmObj_genFood(char* name, int energy);
void* smmObj_genFest(char* name);

//정보 가져오는 함수
char* smmObj_getObjectName(void *ptr);
int smmObj_getObjectType(void *ptr);
int smmObj_getObjectEnergy(void *ptr);
char* smmObj_getTypeName(int node_type);
int smmObj_getObjectCredit(void *ptr);
int smmObj_getObjectGrade(void *ptr);

char* smmObj_getFoodName(void *ptr);
int smmObj_getFoodEnergy(void *ptr);
char* smmObj_getFestName(void *ptr);

#endif /* smm_object_h */
