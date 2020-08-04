#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);
void display();

char name[NAME_SIZE] = "[NULL]";
int offset=0;

int main(int argc, char *argv[]){
	int sock;
	char buf[BUF_SIZE];
	char chat_buf[BUF_SIZE];
	char Login[BUF_SIZE];

	struct sockaddr_in serv_adr;

	struct timeval timeout;
	
	fd_set reads, cpy_reads;

	int fd_max, fd_num;

	// 로그인, 회원가입 인스턴스
	int fd;
	
	if(argc != 4){
		printf("Usage : %s <IP> <Port> <Nick name> \n", argv[0]);
		exit(1);
	}
	sprintf(name, "[%s]", argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");

	display();
	if(offset == 0){
			read(sock, buf, BUF_SIZE); // 로그인, 회원가입 선택하는 메시지 
			printf("%s\n", buf);

			fgets(buf, BUF_SIZE, stdin);
			write(sock, buf, strlen(buf)); // 둘중하나 선택 

			read(sock, buf, BUF_SIZE);	// 로그인, 회원가입 여부 확인받음 
			
			if(buf[0] == '1'){	// 로그인 선택했다면 
				printf("id, pass 입력 : \n");
				printf("ex) UserID | Password\n");
				fgets(Login, BUF_SIZE, stdin); // 사용자 로그 입력 
				write(sock, Login, strlen(Login)); // 사용자 로그 서버로 전달 

				read(sock, buf, BUF_SIZE);	// 로그인 결과 받기 

				if(buf[0] == '3'){	 //로그인 성공 
					printf("%s \n", buf); // 1. 로그인성공, 챗방 입장 
					offset=1; // 다음 루프에 offset==0 무시, 채팅방 입장 
				}
				else if(buf[0] == '4'){ // 로그인 실패 
					printf("재접속 요구");	// 2. 로그인 실패, 재접속 요구 
					offset=0;		
					close(sock);
					return 0;
				}
			}
			else if(buf[0] == '2'){ // 회원가입 선택했다면 
				printf("id, pass 입력 : \n");
				printf("ex) UserID | Password\n");
				fgets(Login, BUF_SIZE, stdin);
				write(sock, Login, strlen(Login));
				read(sock, buf, BUF_SIZE);
				fputs(buf, stdout);

				if(buf[0] == '5')
					offset = 1;
			}
				
		}

	if(offset == 1){
		FD_ZERO(&reads);
		FD_SET(0, &reads);
		FD_SET(sock, &reads);
		
				
		fd_max = sock;
		while(1){
			cpy_reads = reads;
			
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			
			
				
			fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout);
			
			if(fd_num == -1)
				error_handling("select() error");
			else if(fd_num == 0) // time out
				continue;
				
			else{
				if(FD_ISSET(0, &cpy_reads)){
					write_routine(sock, chat_buf);
					FD_CLR(0, &cpy_reads);
				}
				if(FD_ISSET(sock, &cpy_reads)){
					read_routine(sock, chat_buf);
					FD_CLR(sock, &cpy_reads);
				}
			}
		} // end of while
	}
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void read_routine(int sock, char *buf){
	char total_buf[NAME_SIZE + BUF_SIZE];
	
	int str_len = read(sock, total_buf, NAME_SIZE + BUF_SIZE);
	if(str_len == 0)
		return;
	total_buf[str_len]=0;
	fputs(total_buf, stdout);
}

void write_routine(int sock, char *buf){

		char total_buf[NAME_SIZE + BUF_SIZE];
	
		fgets(buf, BUF_SIZE, stdin);
		if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){
			shutdown(sock, SHUT_WR);
			exit(1);
		}
		sprintf(total_buf, "%s %s", name, buf);
		write(sock, total_buf, strlen(total_buf));
	
}

void display(){
	printf("해당 터미널로 재접속 가능====>'q' 또는 'Q'입력 시 연결 종료\n");
}

