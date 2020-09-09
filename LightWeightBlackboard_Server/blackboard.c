#include "./blackboard.h"

void signal_handler(int signo)
{
	if(signo == SIGINT)
	{
		int fd,maxfd;

		maxfd = getdtablesize();
		for(fd = 3; fd < maxfd; fd++)
			close(fd);
		pthread_mutex_destroy(&mutex);
		exit(EXIT_SUCCESS);
	}
}

void * room_handler(void * arg) // 채팅방 관리
{
	int i,j,clnt_sock;
	int roomnum = *((int *)arg);
	char message[1024];

	while(1)
	{
		for(i = 0; i <= rooms[roomnum].count; i++)
		{
			Info info;
		
			if(rooms[roomnum].clnt_socks[i] != -1)
			{
				if(rooms[roomnum].tids[i] == -1)
				{
					for(j = 0; j < MEMBER_MAX; j++)
					{
						if(rooms[roomnum].clnt_socks[j] != -1)
						{
							memset((char *)message,0,1024);
							message[0] = '1';
							strcat(message,rooms[roomnum].ids[i]);
							if(write(rooms[roomnum].clnt_socks[j],message,1024) == -1)
							{
								fprintf(stderr,"write error\n");
								break;
							}
						}
					}
					info.roomnum = roomnum;
					pthread_mutex_lock(&mutex);
					info.clnt_sock = rooms[roomnum].clnt_socks[i];
					info.clnt_num = i;
					if(pthread_create(&rooms[roomnum].tids[i],NULL,socket_handler,(void *)&info) != 0)
					{
						fprintf(stderr,"pthread_create error in socket handler\n");
						continue;
					}
					pthread_detach(rooms[roomnum].tids[i]);
					pthread_mutex_unlock(&mutex);
				}
			}
		}
	}
}

void * socket_handler(void * arg) // client 관리
{
	bool first = TRUE;
	int i = 0,index;
	char buf[1024];
	Info info = *((Info *)arg);
	int clnt_sock = info.clnt_sock;
	int clnt_num = info.clnt_num; // client index number 
	MYSQL * conn;
	MYSQL_RES * result;
	MYSQL_ROW row;
	int num_fields;
	char log[1024];
	char * ptr;
	// user data initializer //
	UD ud;
	ud.clnt_socket = clnt_sock;
	ud.modify_rights = FALSE;

	index = info.roomnum;
	//index = 0;
	memset((char *)buf,0,1024);
	memset((char *)log,0,1024);

	printf("socket handler start\n");
	printf("rooms[%d].clnt_socks[%d] : %d\n", index, clnt_num, rooms[index].clnt_socks[clnt_num]);
	printf("rooms[%d].tids[%d] : %ld\n", index, clnt_num, rooms[index].tids[clnt_num]);
	printf("client socket : %d\n", clnt_sock);
	printf("%dth room's client number : %d\n", index, clnt_num);

	conn = mysql_init(NULL);
	if(conn == NULL)
	{
		fprintf(stderr,"mysql_init error\n");
		return NULL;
	}

	if(mysql_real_connect(conn,"localhost","root","95@#$flachldud","lwp_db",0,NULL,0) == NULL)
	{
		fprintf(stderr,"mysql_real_connect error\n");
		return NULL;
	}

	sprintf(log,"SELECT * FROM chat_tb%d", index);
	if(mysql_query(conn,log))
	{
		fprintf(stderr,"mysql_query error\n");
		return NULL;
	}
	memset((char *)log,0,1024);

	result = mysql_store_result(conn);
	if(result == NULL)
	{
		fprintf(stderr,"mysql_store_result is null\n");
		first = FALSE;
	}

	if(first)
	{
		num_fields = mysql_num_fields(result);
		while(row = mysql_fetch_row(result))
		{
			memset((char *)log,0,1024);
			ptr = log;
			log[0] = '2';
			ptr++;
			strncpy(ptr,row[1],ID);
			strncpy((ptr+ID+1),row[2],BUFFER_SIZE);
			printf("saved log : %s\n", log);
			if(write(clnt_sock,log,1024) == -1)
			{
				fprintf(stderr,"write error\n");
				break;
			}
		}
	}

	first = TRUE;

	// 파일 생성 후 오픈 //
	if (clnt_sock == 4 || rooms[index].fp == NULL) 
	{
		char path[128];
		char file_db[256];

		memset(rooms[index].fname, 0x00, 128);
		sprintf(rooms[index].fname, "%d.c", index);
		rooms[index].fp = fopen(rooms[index].fname, "w+");
		conn = mysql_init(NULL);
		if(mysql_real_connect(conn,"localhost","root","95@#$flachldud","lwp_db",0,NULL,0) == NULL)
		{
			fprintf(stderr,"mysql_real_connect error\n");
			return NULL;
		}
		memset((char *)path,0,128);
		memset((char *)file_db,0,256);
		if(realpath(rooms[index].fname,path) == NULL)
		{
			fprintf(stderr,"realpath error\n");
			return NULL;
		}
		printf("absolute path : %s\n", path);
		sprintf(file_db,"INSERT INTO file VALUES(%d,'%s')", index, path);
		if(mysql_query(conn,file_db))
		{
			fprintf(stderr,"mysql_query error\n");
			return NULL;
		}
		mysql_close(conn);
	}
	else if (rooms[index].fp != NULL) 
	{
		TransFileData(&ud, index);
	}

	while(1)
	{
		if(first)
		{
			first = FALSE;
			printf("socket loop\n");
		}

		memset((char *)buf,0,1024);
		printf("before reading\n");
		printf("client socket : %d\n", clnt_sock);

		if(read(clnt_sock,buf,BUFFER_SIZE) == -1)
		{
			fprintf(stderr,"connection with client is broken\n");
			printf("before breaking clnt_sock : %d\n", clnt_sock);
			printf("before breaking client socket : %d\n", rooms[index].clnt_socks[clnt_num]);
			printf("before breaking clinet thread : %ld\n", rooms[index].tids[clnt_num]);
			printf("before breaking,client number : %d\n", rooms[index].count); 
			pthread_mutex_lock(&mutex);
			rooms[index].clnt_socks[clnt_num] = -1;
			rooms[index].tids[clnt_num] = -1;
			memset((char *)rooms[index].ids[rooms[index].count],0,ID);
			rooms[index].count--;
			close(clnt_sock);
			printf("after breaking clnt_sock : %d\n", clnt_sock);
			printf("after breaking client socket : %d\n", rooms[index].clnt_socks[clnt_num]);
			printf("after breaking clinet thread : %ld\n", rooms[index].tids[clnt_num]);
			printf("after breaking,client number : %d\n", rooms[index].count); 
			pthread_mutex_unlock(&mutex);
			fprintf(stderr,"socket handler exited!\n");
			return NULL;
		}

		if(buf[0] == '2') // chatting
		{
			printf("buf : %s\n", buf);
	
			for(i = 0; i <= rooms[index].count; i++)
			{
				if(rooms[index].clnt_socks[i] != -1)
				{
					printf("rooms[%d].clnt_socks[%d] : %d\n", index, i, rooms[index].clnt_socks[i]);
					
					memset((char *)log,0,1024);
					log[0] = '2';
					ptr = log;
					ptr++;
					strncpy(ptr,rooms[index].ids[clnt_num],ID);
					ptr += ID;
					strncpy(ptr,buf,BUFFER_SIZE-ID-1);
					write(rooms[index].clnt_socks[i],log,1024);
					fsync(rooms[index].clnt_socks[i]);
					printf("send buf : %s\n", buf);
				}
			}
			char tmp[100];
			char str[BUFFER_SIZE];
			ptr = buf;
			ptr++;

			memset((char *)str,0,BUFFER_SIZE);
			strncpy(str,ptr,BUFFER_SIZE-1);

			pthread_mutex_lock(&mutex);
			conn = mysql_init(NULL);
			if(conn == NULL)
			{
				fprintf(stderr,"mysql init error\n");
				continue;
			}

			if(mysql_real_connect(conn,"localhost","root","95@#$flachldud","lwp_db",0,NULL,0) == NULL)
			{
				fprintf(stderr,"mysql_real_connect error\n");
				continue;
			}
			memset((char *)tmp,0,100);
			sprintf(tmp,"INSERT INTO chat_tb%d VALUES(%d,'%s','%s')", index, index, rooms[index].ids[clnt_num], str);
			if(mysql_query(conn,tmp))
			{
				fprintf(stderr,"mysql_query error\n");
				continue;
			}
			pthread_mutex_unlock(&mutex);
		}
		else if(buf[0] == FM_RIGHTS || buf[0] == FM_MODIFIED || buf[0] == FM_COMPILE || buf[0] == FM_CREATE || buf[0] == FM_OPEN) // file module
		{
			int fmflag;
			int len = strlen(buf);
			char fmstr[BUFF_SIZE - 1];
			memset(fmstr, 0x00, BUFF_SIZE - 1);

			fmflag = buf[0];
			strcpy(fmstr, buf+1);
			fmstr[len-1] = 0;	
			printf("fmflag : %d\n", fmflag);
			printf("fmstr : %s\n", fmstr);
			FileModule(fmstr, fmflag, &ud, index);
		}
	}
}


void serv_init(char * ip)
{
	int i,j;
	char buf[BUFFER_SIZE];
	bool check = FALSE;

	memset((char *)buf,0,BUFFER_SIZE);

	room_count = -1;

	rooms = (Rooms *)malloc(sizeof(Rooms) * MEMBER_MAX);

	for(i = 0; i < MEMBER_MAX; i++)
	{
		rooms[i].t_id = -1;
		rooms[i].count = -1;
		rooms[i].roomnum = -1;

		for(j = 0; j < MEMBER_MAX; j++)
		{
			rooms[i].clnt_socks[j] = -1;
			rooms[i].tids[j] = -1;
			memset((char *)rooms[i].ids[j],0,ID);
		}
	}

	i = 0; j = 0;

	/* 서버의 ip 추출하기 */

	while(i < strlen(ip)) 
	{
		while(isdigit(ip[i]) || ip[i] == '.')
		{
			check = TRUE;
			buf[j] = ip[i];
			i++;
			j++;
		}
		if(check)
			break;
		i++;
	}
	memset((char *)ip,0,BUFFER_SIZE);
	strcpy(ip,buf);
}
