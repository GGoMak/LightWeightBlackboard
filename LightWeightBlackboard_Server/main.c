#include "./blackboard.h"

int main(int argc,char * argv[])
{
	FILE * fp;
	pid_t pid;
	int i,clnt_sock;
	int sockopt = 1;
	socklen_t clnt_addr_len;
	char ip[BUFFER_SIZE];
	char buf[BUFFER_SIZE];
	struct sockaddr_in serv_addr,clnt_addr;
	//pthread_t accept_thread;

	signal(SIGINT,signal_handler);

	if(argc != 1)
	{
		fprintf(stderr,"Usage : %s\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_init(&mutex,NULL);

	system("clear");

	if((serv_sock = socket(PF_INET,SOCK_STREAM,0)) == -1)
	{
		fprintf(stderr,"socket() error\n");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	memset((char *)ip,0,BUFFER_SIZE);
	sprintf(ip,"ip addr | grep \"inet \"");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	/* bind() error 방지 */

	if(setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,&sockopt,sizeof(sockopt)) == -1)
	{
		fprintf(stderr,"setsockopt() error\n");
		exit(EXIT_FAILURE);
	}

	if(bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
	{
		fprintf(stderr,"bind() error\n");
		exit(EXIT_FAILURE);
	}

	/* ip 추출하기 */

	fp = freopen("temp.txt","w",stdout);
	system(ip);
	fflush(fp);
	fp = freopen("/dev/tty","a",stdout);
	memset((char *)ip,0,BUFFER_SIZE);

	if((fp = fopen("temp.txt","r")) == NULL)
	{
		fprintf(stderr,"fopen error\n");
		exit(EXIT_FAILURE);
	}

	fgets(ip,BUFFER_SIZE,fp);
	memset((char *)ip,0,BUFFER_SIZE);
	fgets(ip,BUFFER_SIZE,fp);
	ip[strlen(ip)-1] = '\0';
	fclose(fp);
	remove("temp.txt");

	serv_init(ip);
	printf("Server IP address : %s\n", ip);

	/* listen 함수 */

	if(listen(serv_sock,BACKLOG) == -1)
	{
		fprintf(stderr,"listen() error\n");
		exit(EXIT_FAILURE);
	}
/*
	if(pthread_create(&accept_thread,NULL,accept_handler,NULL) == -1)
	{
		fprintf(stderr,"accept handler error\n");
		exit(EXIT_FAILURE);
	}
	pthread_detach(accept_thread);
*/
	while(1)
	{
		bool table = FALSE;
		bool room = FALSE;
		int num_fields; // for db
		char * ptr;
		char test[6];
		char id[16],pw[16];
		char login[IDPW];
		char message[BUFFER_SIZE*2];
		char fname[BUFFER_SIZE];
		int i,temp;

		memset((char *)id,0,16);
		memset((char *)pw,0,16);
		memset((char *)login,0,IDPW);
		memset((char *)message,0,BUFFER_SIZE*2);
		
		printf("before accept\n");
		clnt_addr_len = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_addr,&clnt_addr_len); // accept
		if(clnt_sock == -1)
		{
			fprintf(stderr,"accept() error\n");
			printf("Blocked Client IP : %s\n", inet_ntoa(clnt_addr.sin_addr));
			continue;
		}
		printf("clnt sock : %d\n", clnt_sock);
		printf("after accept\n");

		if(write(clnt_sock,"ACPT",5) == -1)
		{
			fprintf(stderr,"write error\n");
			continue;
		}
		else
		{
			if(read(clnt_sock,login,IDPW) == -1)
			{
				fprintf(stderr,"read error\n");
				continue;
			}
			ptr = login;
			ptr++;
			memcpy(id,ptr,16);
			memcpy(pw,(ptr+15),16);
			printf("id : %s\n", id);
			printf("pw : %s\n", pw);

			/* DB에서 log in 과정 처리 */

			MYSQL_ROW row;
			MYSQL * conn = mysql_init(NULL);

			if(!conn)
			{
				fprintf(stderr,"mysql_init error\n");
				continue;
			}

			if(mysql_real_connect(conn,"localhost","root","95@#$flachldud","lwp_db",0,NULL,0) == NULL)
			{
				fprintf(stderr,"mysql_real_connect error\n");
				continue;
			}

			if(mysql_query(conn,"SELECT id FROM user_tb"))
			{
				fprintf(stderr,"mysql_query\n");
				continue;
			}

			MYSQL_RES * result = mysql_store_result(conn);
			if(result == NULL)
			{
				fprintf(stderr,"mysql_store_result error\n");
				continue;
			}

			num_fields = mysql_num_fields(result);

			char tmp[100];

			while(row = mysql_fetch_row(result))
			{
				//printf("mysql_fetch_row test\n");

				if(!strcmp(row[0],id))
				{
					printf("id match!\n");
					table = TRUE;

					sprintf(tmp,"SELECT pw FROM user_tb WHERE id='%s'", id);
					if(mysql_query(conn,tmp))
					{
						fprintf(stderr,"mysql_query error\n");
						continue;
					}

					MYSQL_RES * result_id = mysql_store_result(conn);
					MYSQL_ROW row_pw = mysql_fetch_row(result_id);

					if(!strcmp(row_pw[0],pw))
					{
						printf("login success!\n");
						memset((char *)test,0,6);
						//pthread_mutex_lock(&mutex);
						sprintf(test,"%d", (room_count+1));
						//pthread_mutex_unlock(&mutex);
						printf("test : %s\n", test);
						if(write(clnt_sock,test,6) == -1)
						{
							fprintf(stderr,"write error\n");
							continue;
						}
						break;
					}
				}
			}
			if(!table)
			{
				sprintf(tmp,"INSERT INTO user_tb VALUES('%s','%s')",id,pw);
				if(!mysql_query(conn,tmp))
				{
					//printf("success!\n");
					memset((char *)test,0,6);
					//pthread_mutex_lock(&mutex);
					sprintf(test,"%d", (room_count+1));
					//pthread_mutex_unlock(&mutex);
					printf("test : %s\n", test);
					if(write(clnt_sock,test,6) == -1)
					{
						fprintf(stderr,"write error\n");
						continue;
					}
				}
			}

			mysql_free_result(result);
			mysql_close(conn);

			temp = 0;
			/*
			sprintf(message,"%d. create new room\n", temp++);
			if(write(clnt_sock,message,BUFFER_SIZE*2) == -1)
			{
				fprintf(stderr,"write error\n");
				continue;
			}
			*/
			for(i = 0; i < MEMBER_MAX; i++)
			{
				if(rooms[i].roomnum != -1)
				{
					printf("rooms[%d].roomnum : %d\n", i, rooms[i].roomnum);
					room = TRUE;
					memset((char *)message,0,BUFFER_SIZE*2);
					sprintf(message,"th room.\n");
					if(write(clnt_sock,message,BUFFER_SIZE) == -1)
					{
						fprintf(stderr,"write error\n");
						continue;
					}
				}
			}
			memset((char *)test,0,6);

			if(read(clnt_sock,test,6) == -1)
			{
				fprintf(stderr,"read error\n");
				continue;
			}
			ptr = test;

			if(test[0] == '4')
			{
				char temp[200];
				ptr++;

				if(!strcmp(ptr,"MAKE")) // create new room
				{
					pthread_mutex_lock(&mutex);
					room_count++;
					conn = mysql_init(NULL); // db init
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
					memset((char *)temp,0,200);
					memset((char *)fname,0,BUFFER_SIZE);

					sprintf(temp,"CREATE TABLE chat_tb%d( room_name int(10) NOT NULL, user_name char(16) NOT NULL, message char(128))", room_count);

					if(mysql_query(conn,temp))
						fprintf(stderr,"chat_tb%d is already exist\n", room_count);

					mysql_close(conn);

					sprintf(fname,"%d.c", room_count);
					if(room_count >= MEMBER_MAX)
					{
						fprintf(stderr,"all rooms are already using\n");
						write(clnt_sock,"ROMF",5);
						room_count--;
						continue;
					}
					rooms[room_count].count++;
					if(rooms[room_count].count >= MEMBER_MAX)
					{
						fprintf(stderr,"room is full\n");
						write(clnt_sock,"FULL",5);
						rooms[room_count].count--;
						continue;
					}
					rooms[room_count].clnt_socks[rooms[room_count].count] = clnt_sock;
					rooms[room_count].roomnum = room_count;
					strcpy(rooms[room_count].ids[rooms[room_count].count],id);

					if(pthread_create(&rooms[room_count].t_id,NULL,room_handler,(void *)&room_count) != 0)
					{
						fprintf(stderr,"pthread_create error in room handler\n");
						continue;
					}
					pthread_detach(rooms[room_count].t_id);
					pthread_mutex_unlock(&mutex);
				}
				else if(ISNUM(*ptr)) // enter a chatting room
				{
					bool isAccept = FALSE;
					int client_choice = atoi(ptr);
				
					printf("client socket : %d\n", clnt_sock);
					printf("client_choice : %d\n", client_choice);
					for(i = 0; i <= room_count; i++)
					{
						if(client_choice == i)
						{
							isAccept = TRUE;
							pthread_mutex_lock(&mutex);
							rooms[i].count++;
							pthread_mutex_unlock(&mutex);

							if(rooms[i].count >= MEMBER_MAX)
							{
								if(write(clnt_sock,"FULL",5) == -1)
								{
									fprintf(stderr,"write error\n");
									break;
								}
							}
							if(write(clnt_sock,"NOTF",5) == -1)
							{
								fprintf(stderr,"write error\n");
								break;
							}

							pthread_mutex_lock(&mutex);
							rooms[i].clnt_socks[rooms[i].count] = clnt_sock;
							strcpy(rooms[i].ids[rooms[i].count],id);
							pthread_mutex_unlock(&mutex);
							// thread ?
							break;
						}
					}
					if(!isAccept)
					{
						fprintf(stderr,"invalid room number!\n");
						continue;
					}
				}
				else
				{
					fprintf(stderr,"invalid message\n");
					continue;
				}
			}
			else
			{
				fprintf(stderr,"invalid message\n");
				continue;
			}
		}
	}
	close(serv_sock);
	pthread_mutex_destroy(&mutex);
	exit(EXIT_SUCCESS);
}
