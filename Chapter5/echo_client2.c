#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len, recv_len, recv_cnt;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;
		
		// 데이터 전송의 단편화를 보완하기 위한 코드 
		// str_len : 보낸 데이터의 길이 
		str_len=write(sock, message, strlen(message));
		
		recv_len=0;	// 받고자 하는 데이터의 길이를 계속 더해간다는 뜻이다.
		while(recv_len<str_len) // 받은 데이터가 보낸 데이터보다 작으면 
		{
			recv_cnt=read(sock, &message[recv_len], BUF_SIZE-1);
			if(recv_cnt==-1) // 상대방이 통신을 끊었을 때 
				error_handling("read() error!");
			recv_len+=recv_cnt; // 서버로 부터 받은 길이를 더한다.
		}
		
		message[recv_len]=0; // 문자열의 끝을 나타내는 0을 표시해준다.
		printf("Message from server: %s", message); // 보낸 데이터 출력 
	}
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
