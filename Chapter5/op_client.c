#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char opmsg[BUF_SIZE];
	int result, opnd_cnt, i;
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

	fputs("Operand count: ", stdout);
	scanf("%d", &opnd_cnt); // 피연산자가 몇개인지 입력받는다.
	opmsg[0]=(char)opnd_cnt; // 전송하기 위해 1바이트로 형변환 
	
	for(i=0; i<opnd_cnt; i++) // 전송 데이터 형식을 채우는 코드 
	{
		printf("Operand %d: ", i+1); 
		// OPSZ뒤에 +1 : 피연산자의 개수 입력받은 뒤부터 채운다는 의미이다.
		scanf("%d", (int*)&opmsg[i*OPSZ+1]); // 피연산자를 입력받는다.
	}
	// 앞에서 입력했던 내용 중 입력한 피연산자가 아닌 다른 버퍼는 가져오지 않는다.
	// fgetc : 스트림으로부터 하나의 문자만 가져온다.
	fgetc(stdin);

	// 연산자를 입력받는다.
	fputs("Operator: ", stdout);

	// 3개를 받았을 때 3*4+1 = 13위치에 입력한 연산자가 저장된다.
	scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);

	// 서버소켓으로 write한다.
	// opnd_OPSZ : 피연산자들 
	// +2 : 피연산자 개수, 맨 뒤에 연산자를 보내기 위한 길이이다.
	write(sock, opmsg, opnd_cnt*OPSZ+2);
	read(sock, &result, RLT_SIZE);
	
	printf("Operation result: %d \n", result);
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}