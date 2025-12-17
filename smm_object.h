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

//node type number definition
#define SMMNODE_TYPE_LECTURE 			0
#define SMMNODE_TYPE_RESTAURANT			1
#define SMMNODE_TYPE_LABORATORY 		2
#define SMMNODE_TYPE_HOME 				3
#define SMMNODE_TYPE_GOTOLAB 			4
#define SMMNODE_TYPE_FOODCHANCE 		5
#define SMMNODE_TYPE_FESTIVAL			6

#define SMMNODE_OBJTYPE_BOARD   0
#define SMMNODE_OBJTYPE_GRADE   1
#define SMMNODE_OBJTYPE_FOOD 	2
#define SMMNODE_OBJTYPE_FEST	3

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

//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade);
void* smmObj_genFood(char* name, int energy);
void* smmObj_genFest(char* name);

//current position node information
char* smmObj_getObjectName(void *ptr);
int smmObj_getObjectType(void *ptr);
int smmObj_getObjectEnergy(void *ptr);
char* smmObj_getTypeName(int node_type);
int smmObj_getObjectCredit(void *ptr);
int smmObj_getObjectGrade(void *ptr);

char* smmObj_getFoodName(void *ptr);
int smmObj_getFoodEnergy(void *ptr);
char* smmObj_getFestName(void *ptr);

//member retrieving


//element to string



#endif /* smm_object_h */
