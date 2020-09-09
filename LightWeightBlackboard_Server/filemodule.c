#include "blackboard.h"

void FileModule (char *str, int flag, UD *ud, int roomnum)
{
	// 파일 수정 //
	printf("flag : %d str : %s\n", flag , str);

	if (flag == FM_MODIFIED) {
		if (str == NULL) { // 입력된 값이 없으면
	//		SendMassage("ERROR : 수정값이 입력되지 않았습니다.", ERR_MSG, ud, roomnum);
			return;
		}
		if (rooms[roomnum].fp != NULL) { // 파일이 열려있으면
			FMmodified(str, ud, roomnum);
		}
		else { // 파일이 열려있지 않으면
	//		SendMassage("ERROR : 파일이 열려있지 않습니다.", ERR_MSG, ud, roomnum);
			return;
		}
	}
	// 파일 수정 권한 //
	else if (flag == FM_RIGHTS) {
		//pthread_mutex_lock(&mutex); // 동기화
		if (rooms[roomnum].fp != NULL) { // 파일이 열려있으면
			FMrights(ud, roomnum);
		printf("FM1\n");
		}
		else { // 파일이 열려있지 않으면
	//		SendMassage("ERROR : 파일이 열려있지 않습니다.", ERR_MSG, ud, roomnum);
			return;
		}
		//pthread_mutex_unlock(&mutex);
	}
	// 컴파일 //
	else if (flag == FM_COMPILE) {
		if (str == NULL) { // 입력값이 없으면
	//		SendMassage("ERROR : 값이 입력되지 않았습니다.", ERR_MSG, ud, roomnum);
			return;
		}
		FMcompile(str, ud, roomnum); 
	}
	// 파일 오픈 //
	else if (flag == FM_OPEN) {
		if (rooms[roomnum].fp != NULL) {
			FileSave(rooms[roomnum].fp, rooms[roomnum].fname);
	//		sleep(1);
			fclose(rooms[roomnum].fp);
		}

		char msg[MSGBUF];
		memset(msg, 0x00, MSGBUF);
		memset(rooms[roomnum].fname, 0x00, BUFF_SIZE);

		if(access(str, F_OK) == -1) {
			strcpy(msg, OPEN_OOBF);
			write(ud->clnt_socket, msg, MSGBUF);
			return;
		}

		strcpy(msg, OPEN_OOBT);
		write(ud->clnt_socket,msg, MSGBUF);
		strcpy(rooms[roomnum].fname, str); // 파일 이름 복사
		rooms[roomnum].fp = fopen(str, "r+"); // 파일 오픈
		TransFileData(NULL, roomnum); // 클라이언트에게 파일 데이터 전송
		return;
	}
	// 파일 닫기 //
	else if (flag == FM_CLOSE) {
		if (rooms[roomnum].fp != NULL) { // 파일이 열려있으면
		//	FileSave(rooms[roomnum].fp, rooms[roomnum].fname); // 파일 백업
			fclose(rooms[roomnum].fp); 
			memset(rooms[roomnum].fname, 0x00, FILE_NAME_SIZE); // 방 정보 내의 파일 이름 초기화
			return;
		}
		else {// 파일이 열려있지 앟으면
			SendMassage("ERROR : 파일이 열려있지 않습니다.", ERR_MSG, ud, roomnum);
			return;
		}
	}
	// 파일 생성 //
	else if (flag == FM_CREATE) {
		if (str == NULL) { // 입력값이 없으면
//			SendMassage("ERROR : 파일명이 입력되지 않았습니다.", ERR_MSG, ud, roomnum);
			return;
		}
		if (rooms[roomnum].fp != NULL) {
			FileSave(rooms[roomnum].fp, rooms[roomnum].fname);
	//		sleep(1);
			fclose(rooms[roomnum].fp);
			printf("create:close\n");
		}
		FMcreate(str, ud, roomnum); // 파일 생성
	}
	// 파일 복원 //
	else if (flag == FM_RECOVER) {
		if (str == NULL) { // 입력값이 없으면
		//	SendMassage("ERROR : 파일명이 입력되지 않았습니다.", ERR_MSG, ud, roomnum);
			return;
		}
		FMrecover(str, ud, roomnum);
		return;
	}
}
// 유저에게 메시지 전송 함수 //
void SendMassage(char *str, int flag, UD *ud, int roomnum)
{
	int i;

	if (flag == ERR_MSG) { // 에러 메시지인 경우
		char errmsg[MSGBUF]; // 에러 메시지
		memset(errmsg, 0x00, MSGBUF); // 에러 메시지 초기화

	//	strcpy(errmsg, ERR_OOB); // 헤더 추가
	//	strcat(errmsg, str); // 입력값 추가
		strcpy(errmsg, str); // 입력값 추가

		// 모든 유저에게 메시지 보내기
		if (ud == NULL) {
			for(i = 0; i <= rooms[roomnum].count; i++) { // 모든 클라이언트에게 전송
				write(rooms[roomnum].clnt_socks[i], errmsg, MSGBUF);
			}
			return;
		}

		// 특정 유저에게 메시지 보내기
		write(ud->clnt_socket, errmsg, MSGBUF);
		return;
	}
	else if (flag == RES_MSG) {
		char resmsg[MSGBUF]; // 결과 메시지 전송
		memset(resmsg, 0x00, MSGBUF); // 결과 메시지 초기화

	//	strcpy(resmsg, RES_OOB); // 헤더 입력
	//	strcat(resmsg, str); // 값입력
		strcpy(resmsg, str); // 입력값 추가

		// 모든 유저에게 메시지 보내기
		if (ud == NULL) {
			for(i = 0; i <= rooms[roomnum].count; i++) { // 모든 클라이언트에게 전송
				write(rooms[roomnum].clnt_socks[i], resmsg, MSGBUF);
			}
			return;
		}

		// 특정 유저에게 메시지 보내기
		write(ud->clnt_socket, resmsg, MSGBUF);
		return;
	}
}
// 파일 수정 함수 //
void FMmodified(char *str, UD *ud, int roomnum)
{
//	char tmpfname[FILE_NAME_SIZE]; // 임시 파일
//	FILE *tmpfp; // 임시 파일 포인터
	char buf[BUFF_SIZE]; // 버퍼

	// 수정권한을 가지고 있는 경우
	if (rooms[roomnum].modify_rights == TRUE && ud->modify_rights == TRUE) {
//		memset(tmpfname, 0x00, FILE_NAME_SIZE); // 초기화
		memset(buf, 0x00, BUFF_SIZE); // 초기화
//		FileSave(rooms->fp, rooms->fname); // 백업파일 생성

		// 임시 파일 이름 설정
//		strcpy(tmpfname, rooms->fname);
//		strcat(tmpfname, "_tmpfile");

		// 임시 파일 오픈
//		tmpfp = fopen(tmpfname, "w");

		// 첫 입력값 입력
//		fputs(str, tmpfp);

		// 값을 읽어드리고 해당 값을 임시파일에 입력
/*		while(recv(ud->clnt_socket, buf, BUFF_SIZE, 0) > 0) {
			// 헤더 파일 이후 널값이 오면 마무리
			if (buf[1] == '\0') {
				fclose(rooms->fp); // 기존 파일 닫음
				remove(rooms->fname); // 기존 파일 삭제
				fclose(tmpfp); // 임시파일 닫음
				rename(tmpfname, rooms->fname); // 임시파일 이름을 기존파일 이름으로 변경
				rooms->fp = fopen(rooms->fname, "r"); // 임시파일 오픈
				TransFileData(NULL); // 파일 정보 전송
				break;
			}

			fputs(buf, tmpfp); // 임시파일에 입력
			memset(buf, 0x00, BUFF_SIZE);// 버퍼 초기화
		}
*/
		strcpy(buf, FILE_OOB);
		strcat(buf, str);
		int sd;
		printf("before modify\n");
		for (sd = 0; sd <= rooms[roomnum].count; sd++) {
			if (ud->clnt_socket == rooms[roomnum].clnt_socks[sd]) continue;
			write(rooms[roomnum].clnt_socks[sd], buf, BUFF_SIZE);
			printf("buf : %s\n", buf);
		}
		printf("after modify\n");

		fseek(rooms[roomnum].fp, 0, SEEK_SET);
		fputs(str, rooms[roomnum].fp);
		fflush(rooms[roomnum].fp);
		return;
	}
}
// 파일 수정 권한 함수 //
void FMrights(UD *ud, int roomnum) {
	char msg[MSGBUF];
	memset(msg, 0x00, MSGBUF);
	// 수정 권한을 누구도 갖고 있지 않을 때
	if (rooms[roomnum].modify_rights == FALSE) 
	{
		ud->modify_rights = TRUE;
		rooms[roomnum].modify_rights = TRUE;
		//SendMassage("TRUE", RES_MSG, ud, roomnum);
		strcpy(msg, "5TRUE");
		printf("%s\n", msg);
		//int schk = send(ud->clnt_socket, msg, 6, 0);
		//printf("before write\n");
		if(write(ud->clnt_socket, msg, MSGBUF) == -1)
		{
			fprintf(stderr,"write error\n");
			return;
		}
		//printf("ud->clnt_socket : %d\n", ud->clnt_socket);
		return;
	}
	// 수정 권한을 누군가 갖고 있을 때
	else {
		// 수정 권한을 가지고 있는 유저의 경우 권한 회수
		if (ud->modify_rights == TRUE) {
			ud->modify_rights = FALSE;
			rooms[roomnum].modify_rights = FALSE;
			//SendMassage("TRUE", RES_MSG, ud, roomnum);
			strcpy(msg, "6TRUE");
			printf("msg : %s\n", msg);
			if(write(ud->clnt_socket, msg, MSGBUF) == -1)
			{
				fprintf(stderr,"write error\n");
				return;
			}
			return;
		}

		// 수정 권한이 없는 유저의 경우 에러메시지 전송
		//SendMassage("FALSE", ERR_MSG, ud, roomnum);
		strcpy(msg, "5FALSE");
		write(ud->clnt_socket, msg, MSGBUF);
		return;
	}
}
// 컴파일 함수 //
void FMcompile(char *str, UD *ud, int roomnum)
{
	char compilename[256];
	char exename[256];
	char *redirectionErr = "errRedir.txt"; // 에러 리다이렉션 파일이름
	char *redirectionRes = "resRedir.txt"; // 결과 리다이렉션 파일이름
	FILE *redir = NULL;
	char Cmsg[BUFF_SIZE];
	int i;
	printf("%s\n", rooms[roomnum].fname);

	if (rooms[roomnum].fp != NULL) {
		//FileSave(rooms[roomnum].fp, rooms[roomnum].fname);
		fclose(rooms[roomnum].fp);
	}

	sleep(1);
	printf("%s\n", rooms[roomnum].fname);
	// 각 스트링 초기화
	memset(compilename, 0x00, 256);
	memset(exename, 0x00, 256);
	memset(Cmsg, 0x00, BUFF_SIZE);
	strcpy(Cmsg, COMPILE_OOB);
	// make가 입력되었을때
	if (strncmp(str, "make ", 5) == 0 || strcmp(str, "make") == 0) {
		strcpy(compilename, rooms[roomnum].fname);
		strcat(compilename, " 2> "); // 에러 메시지는 에려 리다이렉션 파일에 저장
		strcat(compilename, redirectionErr);
		system(compilename); // make 실행
	}
	else { //make 외의 값이 입력되었을때
//		if (access(str, F_OK) == -1) { // 파일이 존재하는지 확인
//			SendMassage("ERROR : 파일이 존재하지 않습니다.", ERR_MSG, NULL, roomnum);
//			return;
//		}
		//** 컴파일 관련 ...
		strcpy(compilename, "gcc ");
	//	strcat(compilename, str);
		strcat(compilename, rooms[roomnum].fname);
		strcat(compilename, " 2> ");
		strcat(compilename, redirectionErr);
		system(compilename);
	}

	// 에러가 있엇으면 그 정보를 유저에게 전달
	redir = fopen(redirectionErr, "r");
	fseek(redir, 0, SEEK_END);
	if (ftell(redir) > 0) {
		fseek(redir, 0, SEEK_SET);
		// 에러 정보 전달
	/*	while (!feof(redir)) {
			strcat(Cmsg, COMPILE_OOB);
			memset(Cmsg+1, 0x00, BUFF_SIZE);
			fgets(Cmsg+1, BUFF_SIZE, redir);
			for(i = 0; i < rooms[roomnum].count; i++) {
				write(rooms[roomnum].clnt_socks[i], Cmsg, strlen(Cmsg));
			}
		}
		*/
		printf("before error\n");
		fread(Cmsg+1, BUFF_SIZE-1, 1, redir);
		write(ud->clnt_socket, Cmsg, BUFF_SIZE);
		printf("atfer error\n");

		fclose(redir);
		remove(redirectionErr);
		redir = NULL;
		return;
	}
	else {
		fclose(redir);
		redir = NULL;
	}
	printf("before result send\n");
	// 파일 실행
	strcpy(exename, "./a.out > ");
	strcat(exename, redirectionRes);
	system(exename);

	// 실행 결과 전달
	redir = fopen(redirectionRes, "r");
	fseek(redir, 0, SEEK_SET);
/*	while (!feof(redir)) {
		strcpy(Cmsg, COMPILE_OOB);
		memset(Cmsg+1, 0x00, BUFF_SIZE);
		fgets(Cmsg+1, BUFF_SIZE, redir);
		for(i = 0; i < rooms[roomnum].count; i++) {
			write(rooms[roomnum].clnt_socks[i], Cmsg, strlen(Cmsg));
		}
	}
	*/
	printf("before result\n");
	fread(Cmsg+1, BUFF_SIZE-1, 1, redir);
	write(ud->clnt_socket, Cmsg, BUFF_SIZE);
	printf("after result\n");
	fclose(redir);
	remove(redirectionRes);
	redir = NULL;
}
// 파일 생성 함수 //
void FMcreate(char *fname, UD *ud, int roomnum)
{
	char path[128];
	char file_db[256];
	char msg[MSGBUF];
	MYSQL * conn;
	memset(msg, 0x00, MSGBUF);

	printf("%s\n", fname);
	if (access(fname, F_OK) == -1) { // 파일이 존재하는지 확인
		if ((rooms[roomnum].fp = fopen(fname, "w+")) == NULL) { // 파일 생성, 오픈
		//	SendMassage("ERROR : 파일을 여는데 오류가 발생했습니다.", -1, ud, roomnum);
			return;
		}
		conn = mysql_init(NULL);
		if(mysql_real_connect(conn,"localhost","root","95@#$flachldud","lwp_db",0,NULL,0) == NULL)
		{
			fprintf(stderr,"mysql_real_connect error\n");
			return;
		}
		memset((char *)path,0,128);
		memset((char *)file_db,0,256);
		if(realpath(rooms[roomnum].fname,path) == NULL)
		{
			fprintf(stderr,"realpath error\n");
			return;
		}
		printf("absolute path : %s\n", path);
		sprintf(file_db,"INSERT INTO file VALUES(%d,'%s')", roomnum, path);
		if(mysql_query(conn,file_db))
		{
			fprintf(stderr,"mysql_query error\n");
			return;
		}
		mysql_close(conn);
		strcpy(msg, CREATE_OOBT);
		write(ud->clnt_socket, msg, MSGBUF);
		memset(rooms[roomnum].fname, 0x00, BUFF_SIZE);
		strcpy(rooms[roomnum].fname, fname); // 방 정보에 파일 이름 저장
		TransFileData(NULL, roomnum); // 파일 데이터 전송
	}
	else { // 파일이 존재하면
		strcpy(msg, CREATE_OOBF);
		write(ud->clnt_socket, msg, MSGBUF);
//		strcpy(rooms[roomnum].fname, fname); // 방 정보에 파일 이름 저장
//		rooms[roomnum].fp = fopen(fname, "r+");
//		TransFileData(NULL, roomnum); // 파일 데이터 전송
//		SendMassage("ERROR : 해당 이름의 파일이 존재하지 않습니다.", -1, ud, roomnum);
		return;
	}
}
// 파일 데이터 전송 함수 //
void TransFileData(UD *ud, int roomnum)
{
	int i = 0;
	char tmpbuf[BUFF_SIZE];
	char *backuppath = BACKUP_NAME;

	memset(tmpbuf, 0x00, BUFF_SIZE); // 초기화

	if (ud == NULL) { // 모든 유저에게 데이터 전송
		fseek(rooms[roomnum].fp, 0, SEEK_SET);
		strcpy(tmpbuf, FILE_OOB); // 헤더 입력
		fread(tmpbuf+1, BUFF_SIZE - 1, 1, rooms[roomnum].fp);
		for (i = 0; i <= rooms[roomnum].count; i++) {
			write(rooms[roomnum].clnt_socks[i], tmpbuf, BUFF_SIZE);
		}
/*		while (!feof(rooms->fp)) { // 전송
			memset(tmpbuf+1, 0x00, BUFF_SIZE + 1);
			fgets(tmpbuf+1, BUFF_SIZE, rooms->fp);
			for (i = 0; i < rooms->count; i++) {
				send(rooms->clnt_socks[i], tmpbuf, strlen(tmpbuf), 0);
			}
		}
*/
		return;
	}
	else { // 특정 유저에게 데이터 전송
		fseek(rooms[roomnum].fp, 0, SEEK_SET);
		strcpy(tmpbuf, FILE_OOB); // 헤더 입력
		fread(tmpbuf+1, BUFF_SIZE - 1, 1, rooms[roomnum].fp);
		write(ud->clnt_socket, tmpbuf, BUFF_SIZE);
/*		while (!feof(rooms[roomnum].fp)) { // 전송
			memset(tmpbuf+1, 0x00, BUFF_SIZE - 1);
			fgets(tmpbuf+1, BUFF_SIZE-1, rooms[roomnum].fp);
			send(ud->clnt_socket, tmpbuf, BUFF_SIZE, 0);
		}
		*/
		return;
	}
}
// 파일 백업 함수 //
void FileSave(FILE *fp, char *fname)
{
	char backupfname[BUFF_SIZE];
	time_t tm;
	struct tm *tm_info;
	char btime[15];
	FILE *backfp = NULL;
	char buf[BUFF_SIZE];

	fseek(fp, 0, SEEK_END);
	if (ftell(fp) == 0) return;
	if (access(BACKUP_NAME, F_OK) == -1) {
		mkdir(BACKUP_NAME, 0755);
	}

	// 초기화
	memset(backupfname, 0x00, BUFF_SIZE);
	memset(btime, 0x00, 15);
	memset(buf, 0x00, BUFF_SIZE);

	// 시간 설정
	tm = time(NULL);
	tm_info = localtime(&tm);

	// 백업파일 이름 설정
	getcwd(backupfname, BUFF_SIZE);
	strcat(backupfname, "/");
	strcat(backupfname, BACKUP_NAME);
	strcat(backupfname, "/");
	strcat(backupfname, fname);
	char *ptr = strstr(backupfname, ".c");
	strcpy(ptr, "_backup.c");
	
//	sprintf(btime, "_%2d%02d%02d%02d%02d%02d",
//			(tm_info->tm_year + 1900) % 1000, tm_info->tm_mon + 1, tm_info->tm_mday,
//			tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

//	strcat(backupfname, btime);

	printf("backup1\n");
	// 백업파일 오픈
	backfp = fopen(backupfname, "w");

	// 백업
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp)) {
		fgets(buf, BUFF_SIZE, fp);
		fputs(buf, backfp);
	}
	fclose(backfp);
}
// 파일 복원 함수 //
void FMrecover(char *fname, UD *ud, int roomnum)
{
	char *dirname = BACKUP_NAME;
	DIR *dir;
	char num[100];
	struct dirent *file;
	int cnt = 1;

	memset(num, 0x00, 100);

	if (access(fname, F_OK) == -1) { // 파일이 존재하는지 확인
		SendMassage("ERROR : 해당 이름의 파일이 존재하지 않습니다", ERR_MSG, NULL, roomnum);
		return;
	}

	// 백업 파일 오픈 //
	if ((dir = opendir(dirname)) == NULL) {
		fprintf(stderr, "opendir error\n");
		SendMassage("ERROR : 디렉토리를 여는데 오류가 발생하였습니다.", ERR_MSG, NULL, roomnum);
		return;
	}

	// 백업 파일내 파일의 이름을 가진 파일 유저에게 전달
	printBackup(dir, fname, roomnum);

	// 원하는 백업 파일의 선택을 기다림
	if (read(ud->clnt_socket, num, 100) == -1) {
		fprintf(stderr, "read error\n");
		SendMassage("ERROR : 파일을 읽는데 오류가 발생하였습니다.", ERR_MSG, NULL, roomnum);
		return;
	}
	
	// 0 선택시 종료
	if (num[0] == '0') {
		SendMassage("복원을 종료합니다.", RES_MSG, NULL, roomnum);
		return;
	}
	else { // 복원 시작
		cnt = 1;
		rewinddir(dir);
		int sel = atoi(num);
		char backfname[BUFF_SIZE];
		FILE *backfp;
		char buf[BUFF_SIZE];
		memset(backfname, 0x00, BUFF_SIZE);

		// 복원할 파일 선택
		while ((file = readdir(dir)) != NULL) {
			if (strstr(file->d_name, fname) == NULL) {
				continue;
			}

			if (cnt == sel) {
				strcpy(backfname, file->d_name);
				break;
			}

			cnt++;
		}

		closedir(dir);

		// 선택한 백업파일 오픈
		backfp = fopen(backfname, "r");
		if (backfp == NULL) {
			fprintf(stderr, "fopen error for recoaver");
			SendMassage("ERROR : 파일을 여는 도중 오류가 발생했습니다.", ERR_MSG, NULL, roomnum);
			return;
		}
		fseek(backfp, 0, SEEK_SET);
		// 현재 열려있는 파일과 같은 파일이면
		if (strcmp(fname, rooms[roomnum].fname) == 0) {
	//		FileSave(rooms[roomnum].fp, rooms[roomnum].fname);  //백업파일생성
			if (rooms[roomnum].fp != NULL) {
				fclose(rooms[roomnum].fp);
			}

			// 파일 오픈
			rooms[roomnum].fp = fopen(fname, "w");
			if (rooms[roomnum].fp == NULL) {
				fprintf(stderr, "fopen error for recoaver");
				SendMassage("ERROR : 파일을 여는 도중 오류가 발생했습니다.", ERR_MSG, NULL, roomnum);
				return;
			}
			fseek(rooms[roomnum].fp, 0, SEEK_SET);

			while (!feof(backfp)) {
				memset(buf, 0x00, BUFF_SIZE);
				fgets(buf, BUFF_SIZE, backfp);
				fputs(buf, rooms[roomnum].fp);
			}
			fclose(backfp);
			TransFileData(NULL, roomnum);
			SendMassage("복원이 완료되었습니다.", RES_MSG, NULL, roomnum);
			return;

		}
		// 현재 열려있지 않는 파일이면
		else {
			FILE *fp;

			fp = fopen(fname, "r");
	//		FileSave(fp, fname); // 백업파일생성
			fclose(fp);

			fp = fopen(fname, "w");
			if (fp == NULL) {
				fprintf(stderr, "fopen error for recoaver");
				SendMassage("ERROR : 파일을 여는 도중 오류가 발생했습니다.", ERR_MSG, NULL, roomnum);
				return;
			}
			fseek(rooms[roomnum].fp, 0, SEEK_SET);

			while (!feof(backfp)) {
				memset(buf, 0x00, BUFF_SIZE);
				fgets(buf, BUFF_SIZE, backfp);
				fputs(buf, rooms[roomnum].fp);
			}
			fclose(backfp);
			fclose(fp);
			SendMassage("복원이 완료되었습니다.", RES_MSG, NULL, roomnum);
			return;
		}
	}
}
// 백업 파일 출력 함수 //
void printBackup(DIR *dir, char *fname, int roomnum)
{
	struct dirent *file;
	bool chk = FALSE;
	int sd;
	char msg[BUFF_SIZE];
	int cnt = 1;

	// 백업 파일을 읽어 원하는 파일이름을 가진 파일을 유저에게 전송
	while((file = readdir(dir)) != NULL) {
		if (strstr(file->d_name, fname) == NULL) {
			continue;
		}

		memset(msg, 0x00, BUFF_SIZE);
		int flen = strlen(file->d_name);
		flen -= 12;

		if (chk == FALSE) {
			chk = TRUE;
			strcpy(msg, RECOVER_OOB);
			strcat(msg, "0. EXIT\n");
			for (sd = 0; sd < rooms[roomnum].count; sd++) {
				send(rooms[roomnum].clnt_socks[sd], msg, BUFF_SIZE, 0);
			}
		}

		strcpy(msg, RECOVER_OOB);
		sprintf(msg+1, "%d. ", cnt);
		strcat(msg, file->d_name + flen);
		for (sd = 0; sd < rooms[roomnum].count; sd++) {
			send(rooms[roomnum].clnt_socks[sd], msg, BUFF_SIZE, 0);
		}

		cnt++;
	}
}
