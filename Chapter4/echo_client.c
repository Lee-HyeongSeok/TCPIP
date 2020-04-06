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
	int sock;	// 소켓번호를 저장하기 위한 변수 선언
	char message[BUF_SIZE];	// 메시지를 저장하기 위한 문자형 배열 선언 
	int str_len;
	struct sockaddr_in serv_adr;	// 서버의 주소정보를 저장하기 위한 구조체 
	// 인자가 3개가 아닐 시 에러메시지 출력 
	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	// socket() : 해당 소켓을 가리키는 소켓 디스크립터를 반환한다.
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	// 서버 소켓의 주소정보를 설정한다.
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2])); // 문자 스트링 형태로 전달된 인자를 정수형으로 변환한다.
	
	// connect() : 연결 대기중인 서버로 실제 연결을 요청하는 함수 
	// sock : 소켓 디스크립터 
	// serv_adr : 주소정보(접속하고자 하는 IP 및 server port정보)
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin); // 메시지를 입력받는다.
		
		// strcmp함수로 입력한 문자가 끝내는 문자인 q나 Q인지 검사한다.
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;	// 맞으면 while문 종료 
		
		// 서버에 메시지 전송 
		write(sock, message, strlen(message));
	
		// 서버로부터 메시지를 읽는다.
		str_len=read(sock, message, BUF_SIZE-1);
		message[str_len]=0;	// 문자열 스트링에 끝을 의미하는 0을 넣어준다.
		printf("Message from server: %s", message);
	}
	
	close(sock); // 모든 작업이 끝나고 생성된 소켓은 해제한다.
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}