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

//보드 및 게임 구성 요소의 개수를 저장하는 전역 변수
static int smm_board_nr;		//총 노드 개수
static int smm_food_nr;			//총 음식 카드 개수
static int smm_festival_nr;		//총 축제 카드 개수
static int smm_player_nr;		//총 플레이어 수

//플레이어 정보를 담는 구조체
typedef struct {
	char name[MAX_CHARNAME];	//플레이어 이름
	int pos;					//플레이어 위치
	int credit;					//플레이어 누적 학점
	int energy;					//플레이어 누적 에너지
	int flag_graduated;			//졸업 여부 (1 : 졸업, 0 : 재학 중)
	
	//실험실 관련 변수
	int flag_experiment; //1 : 실험 중 상태
	int exp_threshold; 	 //실험 성공 기준값
} smm_player_t;

//플레이어 배열 포인터
smm_player_t *smm_players;

//함수 선언
void generatePlayers(int n, int initEnergy); 	//플레이어 생성 및 초기화
void printPlayerStatus(void);					//턴 시작할 때마다 모든 플레이어 상태 출력
void goForward(int player, int step); 			//플레이어 이동 : 보드에서 step만큼 이동
void* findGrade(int player, char *lectureName); //특정 강의 수강 여부 확인
void printGrades(int player); 					//플레이어의 모든 수강 과목과 성적 출력
float calcAverageGrade(int player); 			//플레이어의 평균 학점 계산

#if 0
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
#endif

//함수 정의

//플레이어의 평균 학점 계산
float calcAverageGrade(int player) {	
	int i;
	int count = smmdb_len(LISTNO_OFFSET_GRADE + player); //들은 수업 개수를 저장할 변수
	float totalScore = 0.0; //총점을 저장할 변수
	
	//들은 수업이 없으면 0.0 반환
	if (count == 0)
		return 0.0; 

	for (i=0; i<count; i++) {
		void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		int gradeNum = smmObj_getObjectGrade(gradePtr);

		//성적(A+~F)을 점수(4.3~0.0)으로 변환
		switch(gradeNum) {
			case 0:
				totalScore += 4.3; //A+
				break;

			case 1:
				totalScore += 4.0; //A0
				break;

			case 2:
				totalScore += 3.7; //A-
				break;

			case 3:
				totalScore += 3.3; //B+
				break;

			case 4:
				totalScore += 3.0; //B0
				break;

			case 5:
				totalScore += 2.7; //B-
				break;

			case 6:
				totalScore += 2.3; //C+
				break;

			case 7:
				totalScore += 2.0; //C0
				break;

			case 8:
				totalScore += 1.7; //C-
				break;

			case 9:
				totalScore += 1.3; //D+
				break;

			case 10:
				totalScore += 1.0; //D0
				break;

			case 11:
				totalScore += 0.7; //D-
				break;

			default:
				totalScore += 0.0; //F
				break;
		}
	}
	
	//총점을 들은 수업 개수로 나누어 평균 계산
	return (totalScore/count);
}

//플레이어의 모든 수강 과목과 성적 출력
void printGrades(int player) { 
	int i;
	void *gradePtr;
	
	//성적 문자열
	char *gradeStr[] = {"A+", "A0", "A-",  "B+", "B0", "B-",
	                    "C+", "C0", "C-", "D+", "D0", "D-", "F"
	                	};
	
	//수강 과목 수만큼 반복
	for(i=0; i<smmdb_len(LISTNO_OFFSET_GRADE+player); i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		int gradeNum = smmObj_getObjectGrade(gradePtr); //과목의 성적(숫자) 저장
		
		printf("%s : %s\n", smmObj_getObjectName(gradePtr), gradeStr[gradeNum]);
	}
}

//특정 강의 수강 여부 확인
void* findGrade(int player, char *lectureName) { 
	int size = smmdb_len(LISTNO_OFFSET_GRADE+player); //플레이어의 수강 과목 개수를 저장
	int i;
	
	//수강 과목 수만큼 반복
	for(i=0; i<size; i++) {
		void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE+player, i);
		
		//강의 이름이 같은지 확인 (0이면 같음)
		if(strcmp(smmObj_getObjectName(gradePtr), lectureName) == 0) { 
			return gradePtr;
		}
	}
	
	//없는 경우 NULL 반환
	return NULL; 
}

//플레이어가 졸업했는지 확인
int isGraduated(void) { 
	int i;
	
	//플레이어 수만큼 반복하여 확인
	for (i=0; i<smm_player_nr; i++) {
		if (smm_players[i].flag_graduated == 1)
			return 1;  //게임 종료
	}

	return 0;
}

//플레이어 생성 및 초기화
void generatePlayers(int n, int initEnergy) { 
	int i;

	smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));

	//플레이어 정보 초기화
	for (i=0; i<n; i++) {
		smm_players[i].pos = 0; 					 	//시작 위치 : 0번 노드(집)
		smm_players[i].credit = 0; 					 	//초기 학점 : 0
		smm_players[i].energy = initEnergy; 			//초기 에너지 : 집 노드의 보충 에너지
		smm_players[i].flag_graduated = 0;	 			//졸업하지 않은 상태로 초기화
		smm_players[i].flag_experiment = 0;				//실험 상태 초기화

		printf("Input %i-th player name : ", i+1);
		scanf("%s", &smm_players[i].name[0]); 			//플레이어 이름 입력 받기
		fflush(stdin);
	}
}

//턴 시작할 때마다 모든 플레이어 상태 출력
void printPlayerStatus(void) { 
	int i;

	//모든 플레이어의 이름, 위치, 학점, 에너지 출력
	printf("\n\n========================== PLAYER STATUS ===========================\n");

	for (int i=0; i < smm_player_nr; i++) {
		//각 플레이어 위치의 노드 정보 가져오기
		void *ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);

		printf("%s at %i.%s",
		       smm_players[i].name,
		       smm_players[i].pos,
		       smmObj_getObjectName(ptr));
		
		//플레이어가 실험 중 상태인 경우 (exp.) 출력       
		if (smm_players[i].flag_experiment == 1) 
			printf("(exp.)");
				
		printf(", credit : %i, energy : %i\n",
		       smm_players[i].credit,
		       smm_players[i].energy);	       
	}
	printf("====================================================================\n\n");
}

//플레이어 이동 : 보드에서 step만큼 이동
void goForward(int player, int step) { 
	//1. 실험 중이면 이동하지 않아야 함
	if (smm_players[player].flag_experiment == 1) {
		return;
	}

	int i;
	void *nodePtr;

	//2. 반복문 : 한 번에 한 칸씩 이동
	for (i=0; i<step; i++) {
		//플레이어 위치 이동
		smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr;
		nodePtr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
		
		//이동 메시지 출력
		printf("  => moved to %s\n", smmObj_getObjectName(nodePtr));

		//'우리집'을 지나가거나 '우리집'이 목적지인 경우
		if(smm_players[player].pos == 0) {
			int charge = smmObj_getObjectEnergy(nodePtr); //집의 에너지 값 확인
			smm_players[player].energy += charge;     //에너지 충전

			printf("   -> returned to HOME! energy charged by %i (total : %i)\n", charge, smm_players[player].energy);
		}
	}
}

//주사위 굴리기
int rolldie(int player) { 
	char c;
	
	printf("This is %s's turn~~ Press any key to roll a die! (Press g to see grade) : ", smm_players[player].name);
	c = getchar();
	fflush(stdin);

	//'g'가 입력된 경우, 플레이어의 성적 이력 출력
	if (c == 'g')
		printGrades(player);

	return (rand()%MAX_DIE + 1);
}

//플레이어가 머무르는 노드에 따른 행동
void actionNode(int player) { 
	//현재 위치 노드 정보 가져오기
	void *nodePtr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
	
	//노드 정보(타입, 학점, 에너지)를 변수에 저장
	int type = smmObj_getObjectType(nodePtr);
	int credit = smmObj_getObjectCredit(nodePtr);
	int energy = smmObj_getObjectEnergy(nodePtr);
	
	//강의 수강 후 성적 처리에 쓰일 변수
	int grade;
	void *gradePtr;

	switch(type) {
		case SMMNODE_TYPE_LECTURE:
			//강의 노드
			//1. 수강한 적 있는 강의인 경우
			if(findGrade(player, smmObj_getObjectName(nodePtr)) != NULL) {
				printf("   -> %s has already taken the lecture %s!!\n", smm_players[player].name, smmObj_getObjectName(nodePtr));
			}
			//2. 현재 에너지가 적어 강의를 들을 수 없는 경우
			else if (smm_players[player].energy < energy) {
				printf("  -> %s does not have enough energy to join the lecture %s! (need : %i, current : %i).\n",
				       smm_players[player].name, smmObj_getObjectName(nodePtr), energy, smm_players[player].energy);
			}
			//3. 이전에 강의를 들은 적 없고, 에너지가 충분한 경우
			else {
				char answer[30]; 	//플레이어의 대답(join or drop)을 저장할 문자열
				int flag_valid = 0; //유효한 대답(join or drop)인지 확인하는 변수

				do {
					printf("   -> Lecture %s(credit : %i, energy : %i) starts~! Are you going to join? or drop? : ",
					       smmObj_getObjectName(nodePtr), credit, energy);
					scanf("%s", answer);
					fflush(stdin);

					//"join"이라고 답한 경우
					if (strcmp(answer, "join") == 0) {
						smm_players[player].credit += credit;
						smm_players[player].energy -= energy;

						//성적 랜덤 생성 (A+~F)
						grade = rand() % SMMNODE_MAX_GRADE;
						gradePtr = smmObj_genObject(smmObj_getObjectName(nodePtr), SMMNODE_OBJTYPE_GRADE,
						                            type, credit, energy, grade);
						smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

						char *gradeStr[] = {"A+", "A0", "A-",  "B+", "B0", "B-",
						                    "C+", "C0", "C-", "D+", "D0", "D-", "F"
						                   };
						float avg = calcAverageGrade(player);

						printf("   -> %s successfully takes the lecture %s with grade %s (average : %f) (remained energy : %i)\n",
						       smm_players[player].name, smmObj_getObjectName(nodePtr), gradeStr[grade], avg, smm_players[player].energy);

						flag_valid = 1; //반복 종료
					}
					//"drop"이라고 답한 경우
					else if (strcmp(answer, "drop") == 0) {
						printf("   -> Player %s dropped the lecture %s!\n", smm_players[player].name, smmObj_getObjectName(nodePtr));

						flag_valid = 1; //반복 종료
					}
					//유효하지 않은 대답을 한 경우
					else {
						printf("   -> Invalid input! Input \"drop\" or \"join\"!\n");

						flag_valid = 0; //잘못된 입력이므로 다시 반복해야 함
					}
				} while (flag_valid == 0);
			}
			break;

		case SMMNODE_TYPE_RESTAURANT:
			//식당 노드
			//보충 에너지만큼 플레이어의 현재 에너지 더하기
			smm_players[player].energy += energy;			
			printf("   -> Let's eat in %s and charge %i energies (remained energy : %i)\n",
			       smmObj_getObjectName(nodePtr), energy, smm_players[player].energy);
			break;

		case SMMNODE_TYPE_LABORATORY:
			//실험실 노드
			//1. 실험 중인 경우
			if (smm_players[player].flag_experiment == 1) {
				printf("   -> Experiment time! Let's see if you can satisfy professor (threshold : %i)\n",
				       smm_players[player].exp_threshold);
				//실험 시도마다 에너지 소모됨
				smm_players[player].energy -= energy;
				printf("   Press any key to roll a die (Press g to see grade) : ");
				getchar();
				fflush(stdin);
				
				//주사위 굴리기
				int exp_die = rand() % MAX_DIE + 1;
				printf("   -> Experiment result : %i, ", exp_die);
				
				//실험 성공 기준값을 넘었는지 확인
				if (exp_die >= smm_players[player].exp_threshold) {
					printf("Success! %s can exit this lab!\n", smm_players[player].name);
					smm_players[player].flag_experiment = 0; // 실험 종료 (탈출 성공)
				} else {
					printf("Fail! %s can't escape this lab.\n", smm_players[player].name);
				}
			}
			//2. 실험 중이 아닌 경우 (그냥 지나감)
			else {
				printf("   -> This is not experiment time. You can go through this lab.\n");
			}
			break;

		case SMMNODE_TYPE_HOME:
			//집 노드 (goForward 함수에서 에너지 받기, 도착 문구 출력하기 등의 동작을 수행함)
			//졸업 조건을 충족한 경우
			if (smm_players[player].credit >= GRADUATE_CREDIT) {
				smm_players[player].flag_graduated = 1;
				printf("   -> Congratulations! %s graduated!!\n", smm_players[player].name);
			}
			break;

		case SMMNODE_TYPE_GOTOLAB:
			//실험실 이동 노드
			printf("   -> OMG! This is experiment time! Player %s goes to the lab.\n", smm_players[player].name);
			
			//실험실로 보내기
			for (int i=0; i<smm_board_nr; i++) {
				void *node = smmdb_getData(LISTNO_NODE, i);

				if (smmObj_getObjectType(node) == SMMNODE_TYPE_LABORATORY) {
					smm_players[player].pos = i;
					break;
				}
			}
			
			//실험 상태 설정
			smm_players[player].flag_experiment = 1;
			smm_players[player].exp_threshold = rand() % MAX_DIE + 1; //실험 성공 기준값 랜덤 설정
			break;

		case SMMNODE_TYPE_FOODCHANCE:
			//음식 찬스 노드
			printf("   -> %s gets a food chance! Press any key to pick a food card : ", smm_players[player].name);
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
			//축제 노드
			//축제 참가 문구 출력
			printf("   -> %s participates to School Festival! Press any key to pick a festival card : ", smm_players[player].name);
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


	//1. 설정 파일 불러오기 ---------------------------------------------------------------------------------
	//1-1. 보드 구성
	if ((fp = fopen(BOARDFILEPATH,"r")) == NULL) {
		printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
		getchar();
		return -1;
	}

	printf("Reading board component......\n");
	
	//정보 읽어오기
	while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) { 
		//파일로부터 보드 관련 정보 저장
		void* ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);

		smmdb_addTail(LISTNO_NODE, ptr);
		smm_board_nr++;
	}

	fclose(fp);

	for (int i=0; i<smm_board_nr; i++) {
		void *boardObjPtr = smmdb_getData(LISTNO_NODE, i);

		printf("=> %i. %s (%s), credit : %i, energy : %i\n", i, smmObj_getObjectName(boardObjPtr),
		       smmObj_getTypeName(smmObj_getObjectType(boardObjPtr)),
		       smmObj_getObjectCredit(boardObjPtr), smmObj_getObjectEnergy(boardObjPtr));
	}

	printf("Total number of board nodes : %i\n", smm_board_nr);


	//2. 음식 카드 구성
	if ((fp = fopen(FOODFILEPATH,"r")) == NULL) {
		printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
		return -1;
	}

	printf("\n\nReading food card component......\n");
	
	//정보 읽어오기
	while ( fscanf(fp, "%s %i", name, &energy) == 2 ) { 
		//파일로부터 음식 관련 정보 저장
		void* ptr = smmObj_genFood(name, energy);

		smmdb_addTail(LISTNO_FOODCARD, ptr);
		smm_food_nr++;
	}

	fclose(fp);

	for (int i=0; i<smm_food_nr; i++) {
		void *foodPtr = smmdb_getData(LISTNO_FOODCARD, i);

		printf("=> %i. %s, charge : %i\n", i, smmObj_getFoodName(foodPtr), smmObj_getFoodEnergy(foodPtr));
	}

	printf("Total number of food cards : %i\n", smm_food_nr);

	//3. 축제 카드 구성
	if ((fp = fopen(FESTFILEPATH,"r")) == NULL) {
		printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
		return -1;
	}

	printf("\n\nReading festival card component......\n");

	while ( fscanf(fp, "%s", name) == 1 ) { //read a festival card string
		//store the parameter set
		void* ptr = smmObj_genFest(name);

		smmdb_addTail(LISTNO_FESTCARD, ptr);
		smm_festival_nr++;
	}

	fclose(fp);

	for (int i=0; i<smm_festival_nr; i++) {
		void *festPtr = smmdb_getData(LISTNO_FESTCARD, i);

		printf("=> %i. %s\n", i, smmObj_getFestName(festPtr));
	}

	printf("Total number of festival cards : %i\n", smm_festival_nr);


	//2. 플레이어 구성 ---------------------------------------------------------------------------------
	printf("\n\n\n\n=======================================================================\n");
	printf("-----------------------------------------------------------------------\n");
	printf("       Sookmyung Marble ~^^~ Let's Graduate!! (total credit : 30)      \n");
	printf("-----------------------------------------------------------------------\n");
	printf("=======================================================================\n\n");


	do {
		//플레이어 수를 입력받아 smm_player_nr에 저장
		printf("\nInput player number (1~10) : ");
		scanf("%i", &smm_player_nr);
		fflush(stdin);
		
		//플레이어 수가 음수이거나 MAX_PLAYER보다 많은 경우
		if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
			printf(" -> Invalid player number!\n");
	} while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);

	generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData(SMMNODE_OBJTYPE_BOARD, 0)));

	turn = 0;

	//3. SM Marble 게임 시작 ---------------------------------------------------------------------------------
	//누군가 졸업할 때까지 반복
	while (isGraduated() == 0) 
	{ 
		int die_result;

		//4-1. 플레이어 상태 출력
		printPlayerStatus();

		//4-2. 주사위 굴리기
		die_result = rolldie(turn);

		printf("--> die result : %i\n", die_result);

		//4-3. 이동
		goForward(turn, die_result);

		//4-4. 도착한 노드에서 행동 수행
		actionNode(turn);

		//4-5. 다음 턴으로 넘김
		turn = (turn + 1)%smm_player_nr;
	}
	
	//4. SM Marble 게임 종료 ---------------------------------------------------------------------------------
	char *gradeStr_result[] = {"A+", "A0", "A-",  "B+", "B0", "B-",
	                           "C+", "C0", "C-", "D+", "D0", "D-", "F"
	                          };
	                          
	//누가 졸업했는지 확인 후 결과 출력
	for (int i=0; i<smm_player_nr; i++) {
		if (smm_players[i].flag_graduated == 1) {
			printf("\n\n");
			printf("=======================================================================\n");
			printf("                 Congratulations~ Player %s Graduated!                 \n", smm_players[i].name);
			printf("-----------------------------------------------------------------------\n");
			printf("                       [ Winner %s's History ]                         \n", smm_players[i].name);
			
			//플레이어가 수강한 과목 총 개수
			int count = smmdb_len(LISTNO_OFFSET_GRADE + i);
			
			//플레이어 수강 이력 출력
			for (int j=0; j<count; j++) {
				//과목 성적 가져오기
				void *gradeNode = smmdb_getData(LISTNO_OFFSET_GRADE + i, j);

				//강의 이름, 학점, 성적 가져오기
				char *lecName = smmObj_getObjectName(gradeNode);
				int lecCredit = smmObj_getObjectCredit(gradeNode);
				int lecGrade = smmObj_getObjectGrade(gradeNode);

				//플레이어 이력 출력
				printf("%s - credit : %i, grade : %s\n", lecName, lecCredit, gradeStr_result[lecGrade]);
			}

			printf("=======================================================================\n");
		}
	}

	free(smm_players);

	return 0;
}
