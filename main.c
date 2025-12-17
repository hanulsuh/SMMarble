//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

//board configuration parameters
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;

//player information variables
typedef struct {
	char name[MAX_CHARNAME];
	int pos;
	int credit;
	int energy;
	int flag_graduated;
	//실험실 관련 변수
	int flag_experiment; //1 : 실험 중 상태
	int exp_threshold; 	 //실험 성공 기준값
} smm_player_t;

smm_player_t *smm_players;

//function prototypes
void generatePlayers(int n, int initEnergy); 	//generate a new player
void printPlayerStatus(void);					//print all players status at the beginning of each turn
void goForward(int player, int step); 			//make player go "step" steps on the board (check if player is graduated)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); 					//print all the grade history of the player
float calcAverageGrade(int player); 			//calculate average grade of the player

//function prototypes
#if 0
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
#endif

//fucntion definitions
float calcAverageGrade(int player)	//calculate average grade of the player
{
	int i;
	int count = smmdb_len(LISTNO_OFFSET_GRADE + player); //들은 수업 개수를 저장할 변수
	float totalScore = 0.0;
	
	if (count == 0) 
		return 0.0; // 들은 수업이 없으면 0
		
	for (i=0; i<count; i++)
	{
		void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        int gradeNum = smmObj_getObjectGrade(gradePtr);
        
        //성적(A+~F)을 점수(4.3~0.0)으로 변환
		switch(gradeNum) {
			
            case 0: totalScore += 4.3; //A+
			break; 
			
            case 1: totalScore += 4.0; //A0
			break; 
			
            case 2: totalScore += 3.7; //A-
			break; 
			
            case 3: totalScore += 3.3; //B+
			break; 
			
     	    case 4: totalScore += 3.0; //B0
			break; 
			
            case 5: totalScore += 2.7; //B-
			break; 
			
            case 6: totalScore += 2.3; //C+
			break; 
			
            case 7: totalScore += 2.0; //C0
			break; 
			
            case 8: totalScore += 1.7; //C-
			break; 
			
			case 9: totalScore += 1.3; //D+
			break; 
			
            case 10: totalScore += 1.0; //D0
			break; 
			
            case 11: totalScore += 0.7; //D-
			break;
			
            default: totalScore += 0.0; //F 
			break;  
		}	
	}
	
	return (totalScore/count);
}


void printGrades(int player) //print all the grade history of the player
{
	int i;
	void *gradePtr;
	
	for(i=0; i<smmdb_len(LISTNO_OFFSET_GRADE+player); i++);
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		printf("%s : %i\n", smmObj_getObjectName(gradePtr), smmObj_getObjectGrade(gradePtr)); 
	}
}


void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
	int size = smmdb_len(LISTNO_OFFSET_GRADE+player);
	int i;
	
	for(i=0; i<size; i++)
	{
		void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE+player, i);
		if(strcmp(smmObj_getObjectName(ptr), lectureName) == 0) //0이면 같은거임
		{
			return ptr;
		}
	} 
	
	return NULL;
}


int isGraduated(void) //check if any player is graduated
{
	int i;
	for (i=0; i<smm_player_nr; i++)
	{
		if (smm_players[i].flag_graduated == 1)
			return 1;  //game end
	}
	
	return 0;
}


void generatePlayers(int n, int initEnergy) //generate a new player 
{
	int i;
	
	smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));
	
	//player information initialization
	for (i=0; i<n; i++)
	{
		smm_players[i].pos = 0; 					 	// position initial value = 0
 		smm_players[i].credit = 0; 					 	// credit initial value = 0
		smm_players[i].energy = initEnergy; 			// energy initial value = set initial energy
		smm_players[i].flag_graduated = 0;	 			// is graduated? -> not yet
		
		printf("Input %i-th player name : ", i);	
		scanf("%s", &smm_players[i].name[0]); 			// receive player name using scanf function
		fflush(stdin);
	}
}


void printPlayerStatus(void) //print all players status at the beginning of each turn 
{
	int i;
	
	//print all players name, position, credit, energy
	printf("\n\n========================== PLAYER STATUS ===========================\n");
    
    for (int i=0; i < smm_player_nr; i++) 
    {
        //각 플레이어 위치의 노드 정보 가져오기
        void *ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
        
        printf("%s at %i.%s, credit: %i, energy: %i\n", 
                smm_players[i].name, 
                smm_players[i].pos, 
                smmObj_getObjectName(ptr),
                smm_players[i].credit, 
                smm_players[i].energy);
    }
    printf("====================================================================\n\n");	
}


void goForward(int player, int step) //make player go "step" steps on the board (check if player is graduated)
{ 
	//실험 중이면 이동하지 않아야 함
	if (smm_players[player].flag_experiment == 1)
    {
        return; 
    }
	
	int i;
	void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos) ;
	
	ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
	printf("start from %s\n", smmObj_getObjectName(ptr));
										
	//loop : go 1 step at a time
	//print player destination position
	for (i=0; i<step; i++)
	{
		smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr;
		ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
		
		printf("  => moved to %i (%s)\n", smm_players[player].pos, 
										smmObj_getObjectName(ptr));
	}
}


int rolldie(int player) //roll dice
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);

#if 1
    if (c == 'g')
        printGrades(player);
#endif   
 
    return (rand()%MAX_DIE + 1);
}


void actionNode(int player) //action code when a player stays at a node
{	
	void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
	
	int type = smmObj_getObjectType(ptr);
	int credit = smmObj_getObjectCredit(ptr);
	int energy = smmObj_getObjectEnergy(ptr);
	int grade;
	void *gradePtr;
	
    switch(type)
    {
    	case SMMNODE_TYPE_LECTURE:
    	//1. 강의 수강 여부 확인
    	if(findGrade(player, smmObj_getObjectName(ptr)) == NULL) 
    	{ //2. 현재 에너지가 소요에너지 이상인지 확인
    		if (smm_players[player].energy >= energy)
    		{
    			char answer[30]; //플레이어의 대답(join or drop)을 저장할 문자열
    			int flag_valid = 0; //유효한 대답(join or drop)인지 확인하는 변수
    			
    			do
    			{
					printf("   -> Do you want to join this lecture? or drop? : ");
    				scanf("%s", answer);
    				fflush(stdin);
    			
    				//"join"이라고 답한 경우
    				if (strcmp(answer, "join") == 0)
    				{
    					smm_players[player].credit += credit;
    					smm_players[player].energy -= energy;
    				
    					//성적 랜덤 생성
    					grade = rand() % SMMNODE_MAX_GRADE;
    					gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE, 
                                                    type, credit, energy, grade);
                        smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                        
                        char *gradeStr[] = {"A+", "A0", "A-",  "B+", "B0", "B-", 
    										"C+", "C0", "C-", "D+", "D0", "D-", "F"};
        				float avg = calcAverageGrade(player);
                        
                        printf("   -> %s successfully takes the lecture %s with grade %s (average : %f), remained energy : %i)\n", 
               			 	smm_players[player].name, smmObj_getObjectName(ptr),     
               				gradeStr[grade], avg, smm_players[player].energy);   
                        
                        flag_valid = 1; //반복 종료
					}
					//"drop"이라고 답한 경우
					else if (strcmp(answer, "drop") == 0)
					{
						printf("   -> You dropped the lecture.\n");
						
						flag_valid = 1; //반복 종료
					}
					//유효하지 않은 대답을 한 경우
					else
                    {
                        printf("   -> Invalid input! input \"drop\" or \"join\"!\n");
                        
                        flag_valid = 0; // 잘못된 입력이므로 다시 반복해야 함
                    }
				} while (flag_valid == 0);
			}
    		else
            {
                printf("Not enough energy to join this lecture.\n");
            }
    	}
    		break;
    	
		case SMMNODE_TYPE_RESTAURANT:
			//보충 에너지만큼 플레이어의 현재 에너지 더하기
    		smm_players[player].energy += energy;
    		
    		printf("   -> Let's eat in %s and charge %i energies (remained energy : %i)\n", 
                		smmObj_getObjectName(ptr), energy, smm_players[player].energy);
			break;
			
		case SMMNODE_TYPE_LABORATORY:
			//실험 중 상태인지 확인
			//실험 중인 경우
			if (smm_players[player].flag_experiment == 1)
			{
				printf("   -> Experiment time! Let's see if you can satisfy professor (threshold: %i)\n", 
                  		smm_players[player].exp_threshold);
                printf("   Press any key to roll a die (press g to see grade): ");
            	getchar(); 
				fflush(stdin);
				//주사위 굴리기
				int exp_die = rand() % MAX_DIE + 1;
				printf("   -> Experiment result : %i, ", exp_die);
				//실험 성공 기준값을 넘었는지 확인
				if (exp_die >= smm_players[player].exp_threshold)
            	{
               		printf("success! %s can exit this lab!\n", smm_players[player].name);
                	smm_players[player].flag_experiment = 0; // 실험 종료 (탈출 성공)
            	}
            	else
            	{
                	printf("fail! %s is still stuck in the lab.\n", smm_players[player].name);
            	}
			}
			//실험 중이 아닌 경우 (그냥 지나감)
			else
        	{
           		printf("   -> This is not experiment time. You can go through this lab.\n");
        	}
			break;
				
		case SMMNODE_TYPE_HOME:
			smm_players[player].energy += energy;
			
			printf("   -> %s is at HOME! Energy charged by %i (total : %i)\n", 
						smm_players[player].name, energy, smm_players[player].energy);
			
			//졸업 조건을 충족한 경우인
			if (smm_players[player].credit >= GRADUATE_CREDIT)
			{
				smm_players[player].flag_graduated = 1;
				printf("   -> Congratulations! %s has graduated!!\n", smm_players[player].name);
			}	
			break;
			
		case SMMNODE_TYPE_GOTOLAB:
			printf("   -> OMG! This is experiment time! Player %s goes to the lab.\n", smm_players[player].name);
			//실험실로 보내기
			for (int i=0; i<smm_board_nr; i++)
        	{
            	void *node = smmdb_getData(LISTNO_NODE, i);
            
            	if (smmObj_getObjectType(node) == SMMNODE_TYPE_LABORATORY)
            	{
              	  smm_players[player].pos = i; 
              	  break;
            	}
        	}
        	//실험 상태 설정
        	smm_players[player].flag_experiment = 1;         
        	smm_players[player].exp_threshold = rand() % MAX_DIE + 1; //실험 성공 기준값 랜덤 설정
			break;
			
		case SMMNODE_TYPE_FOODCHANCE:
			printf("   -> %s gets a food chance! press any key to pick a food card:", smm_players[player].name);
			getchar();  
       		fflush(stdin); 
       		
       		//음식 카드 중 한장 랜덤 선택
       		int foodCard = rand() % smm_food_nr; 
       		void *foodPtr = smmdb_getData(LISTNO_FOODCARD, foodCard);
       		//선택된 음식 관련 정보 가져오기 (이름, 에너지)
       		char *foodName = smmObj_getFoodName(foodPtr);
       		int foodEnergy = smmObj_getFoodEnergy(foodPtr);
        	//플레이어 에너지 충전
        	smm_players[player].energy += foodEnergy;
        	
        	printf("   -> %s picks %s and charges %i (remained energy : %i)\n", 
               smm_players[player].name, foodName,  foodEnergy, smm_players[player].energy);
        	
			break;
			
		case SMMNODE_TYPE_FESTIVAL:
			//축제 참가 문구 출력
			printf("   -> %s participates to School Festival! press any key to pick a festival card:", smm_players[player].name);
			getchar();
    		fflush(stdin);
    		
    		//축제 카드 중 한장 랜덤 선택
    		int festCard = rand() % smm_festival_nr; 
        	void *festPtr = smmdb_getData(LISTNO_FESTCARD, festCard);
        	
        	printf("   -> MISSION : %s !!\n", smmObj_getFestName(festPtr));
        	
        	printf("   (Press any key when mission is ended.)");
      	 	getchar();  
       		fflush(stdin); 
			break;

        default:
            break;
    }
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int turn = 0;
    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);   
        
        smmdb_addTail(LISTNO_NODE, ptr);  
        
        smm_board_nr++;
    }
    
    fclose(fp);
    
    for (int i=0; i<smm_board_nr; i++)
    {
    	void *boardObjPtr = smmdb_getData(LISTNO_NODE, i);
    	
    	printf("=> %i. %s (%s), credit : %i, energy : %i\n", i, smmObj_getObjectName(boardObjPtr), 
															smmObj_getTypeName(smmObj_getObjectType(boardObjPtr)), 
															smmObj_getObjectCredit(boardObjPtr), smmObj_getObjectEnergy(boardObjPtr));
	}
    
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    
    while ( fscanf(fp, "%s %i", name, &energy) == 2 ) //read a food parameter set
    {
        //store the parameter set
        void* ptr;
        
        ptr = smmObj_genFood(name, energy);
        
       	smmdb_addTail(LISTNO_FOODCARD, ptr);  
       	
       	smm_food_nr++;
    }
    
    fclose(fp);
    
    for (int i=0; i<smm_food_nr; i++)
    {
    	void *foodPtr = smmdb_getData(LISTNO_FOODCARD, i);
    	
    	printf("=> %i. %s, charge : %i\n", i, smmObj_getFoodName(foodPtr), smmObj_getFoodEnergy(foodPtr));
	}
    
    printf("Total number of food cards : %i\n", smm_food_nr);
    
   
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    
    while ( fscanf(fp, "%s", name) == 1 ) //read a festival card string
    {
        //store the parameter set
        void* ptr = smmObj_genFest(name);
		
		smmdb_addTail(LISTNO_FESTCARD, ptr);
		
		smm_festival_nr++;        
    }
    
    fclose(fp);
    
    for (int i=0; i<smm_festival_nr; i++)
    {
    	void *festPtr = smmdb_getData(LISTNO_FESTCARD, i);
    	
    	printf("=> %i. %s\n", i, smmObj_getFestName(festPtr));
	}
    
    printf("Total number of festival cards : %i\n", smm_festival_nr);
  
    
    //2. Player configuration ---------------------------------------------------------------------------------    
    printf("\n\n\n\n=======================================================================\n");
    printf("-----------------------------------------------------------------------\n");
    printf("       Sookmyung Marble ~^^~ Let's Graduate (total credit : 30)!!      \n");
    printf("-----------------------------------------------------------------------\n");
    printf("=======================================================================\n\n");

	
	do
    {
        //input player number to player_nr
        printf("\nInput player number : ");
        scanf("%i", &smm_player_nr);
        fflush(stdin);
        
        if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
        	printf(" -> Invalid player number!\n");
    }
    while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);
    
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData(SMMNODE_OBJTYPE_BOARD, 0)));  
    	
	turn = 0;
	
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);
			
		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        
		turn = (turn + 1)%smm_player_nr;        
    }
    
    free(smm_players);
 
    return 0;
}
