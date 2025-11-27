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
#define MAX_GRADE       9


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

//board node number
static int smmObj_nodeNr = 0;	 

//node structure type definition
typedef struct {
	char name[MAX_CHARNAME];
	int type;
	int credit;
	int energy;	
} smmObj_board_t;

//node structure instance array definition
static smmObj_board_t smmObj_board[MAX_NODENR];


//object generation
int smmObj_genNode(char* name, int type, int credit, int energy)
{
	strcpy(smmObj_board[smmObj_nodeNr].name, name);
    smmObj_board[smmObj_nodeNr].type = type;
    smmObj_board[smmObj_nodeNr].credit = credit;
    smmObj_board[smmObj_nodeNr].energy = energy;
    
    smmObj_nodeNr++;
    
    return (smmObj_nodeNr);
}

//node information (current position)
char* smmObj_getNodeName(int node_nr)	 	 //node name
{
	return (smmObj_board[node_nr].name);
}

int smmObj_getNodeEnergy(int node_nr)		 //node energy
{
	return (smmObj_board[node_nr].energy);
}

int smmObj_getNodeType(int node_nr)			 //node type number
{
	return (smmObj_board[node_nr].type);
}

char* smmObj_getTypeName(int node_type)   	 //node type name (enum to string)
{
	return (smmObj_board[node_type].name);
}

int smmObj_getNodeCredit(int node_nr)			 //node credit
{
	return (smmObj_board[node_nr].credit);
}


//member retrieving



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
