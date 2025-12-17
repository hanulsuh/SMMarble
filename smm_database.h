//
//  smm_database.h
//  infection Path
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_database_h
#define smm_database_h

#define LISTNO_NODE             0 //보드 칸 리스트
#define LISTNO_FOODCARD         1 // 음식 카드 리스트
#define LISTNO_FESTCARD         2 // 축제 카드 리스트
#define LISTNO_OFFSET_GRADE     3 // 플레이어 성적 리스트

int smmdb_addTail(int list_nr, void* obj);          //add data to tail
int smmdb_deleteData(int list_nr, int index);       //delete data
int smmdb_len(int list_nr);                  		//get database length
void* smmdb_getData(int list_nr, int index);        //get index'th data

#endif /* smm_database_h */
