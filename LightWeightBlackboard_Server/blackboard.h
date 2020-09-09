#ifndef _BLACK_BOARD_HEADER_
#define _BLACK_BOARD_HEADER_

#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "/usr/include/mysql/mysql.h"

#define TRUE 1
#define FALSE 0
#define BACKLOG 10
#define MEMBER_MAX 16
#define PROCESS_MAX 16

#define ID 16
#define IDPW 33
#define BUFFER_SIZE 128

#define PORT 42195

#define ISNUM(ch) (ch >= 48 && ch <= 57)

//// 파일 기능을 위한 flag ////
#define FM_MODIFIED '7' // 파일 수정
#define FM_RIGHTS '5' // 파일 수정 권한
#define FM_COMPILE '8' // 컴파일
#define FM_OPEN 'a' // 파일 오픈
#define FM_CLOSE '5' // 파일 클로즈
#define FM_CREATE '9' // 파일 생성
#define FM_RECOVER '7' // 파일 복원
// 파일 모듈내 메시지 전송 구분을 위한 flag //
#define ERR_MSG 0// 에러 메시지
#define RES_MSG 1 // 결과 메시지
// 파일 모듈 내 메크로 //
#define BUFF_SIZE 1024 // 크기
#define MSGBUF 1024 // 메시지 크기
#define FILE_NAME_SIZE 512 // 파일 이름 크기
#define BACKUP_NAME "BACKUP" // 백업 폴더 이름
// 메시지 전송때 사용되는 헤더 // 
//#define ERR_OOB "0" // 에러 메시지 전송할 때
//#define RES_OOB "1" // 결과 메시지 전송할 때
#define FILE_OOB "7" // 파일 내용 전송할 때
#define COMPILE_OOB "8" // 컴파일 결과 전송할 때
#define RECOVER_OOB "5" // 복원결과 전송할 때
#define CREATE_OOBT "9TRUE"
#define CREATE_OOBF "9FAIL"
#define OPEN_OOBT "aTRUE"
#define OPEN_OOBF "aFAIL"

/* 필요한 데이터 정의 */

typedef _Bool bool;

typedef struct _info
{
	int roomnum;
	int clnt_sock;
	int clnt_num; // index number
}Info;

typedef struct _rooms // member's id - for managing process
{
	pthread_t t_id;
	int roomnum;
	int count; // the number of clients
	int clnt_socks[MEMBER_MAX]; // socket descriptors
	pthread_t tids[MEMBER_MAX];
	char ids[MEMBER_MAX][ID];
	FILE *fp; //// 방에 열려있는 파일
	char fname[BUFF_SIZE]; //// 방에 열려있는 파일 이름
	bool modify_rights; //// 파일 수정 권한을 누군가가 가지고 있는지
}Rooms;

// 유저 데이터 정보 //
typedef struct _UserData 
{
	int clnt_socket; // 클라이언트 소켓
	bool modify_rights; // 수정 권한
}UD;

Rooms * rooms; // 프로세스 채팅방 멤버 id
pthread_mutex_t mutex; // mutex
int room_count; // 채팅 방 개수 세기
int serv_sock;

/* 필요 함수 목록 */

void signal_handler(int signo);

void * socket_handler(void * arg); // client 관리

void * room_handler(void * arg); // 채팅방 관리

void serv_init(char * ip); // 서버 관련 데이터 초기화

void FileModule(char *str, int flag, UD *ud, int roomnum);

void SendMassage(char *str, int flag, UD *ud, int roomnum);

void FMmodified(char *str, UD *ud, int roomnum);

void FMrights(UD *ud, int roomnum);

void FMcompile(char *str, UD *ud, int roomnum);

void FMcreate(char *fname, UD *ud, int roomnum);

void TransFileData(UD *ud, int roomnum);

void FileSave(FILE *fp, char *fname);

void FMrecover(char *fname, UD *ud, int roomnum);

void printBackup(DIR *dir, char *fname, int roomnum);

#endif
