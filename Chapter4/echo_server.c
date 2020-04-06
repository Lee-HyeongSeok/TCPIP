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
	int serv_sock, clnt_sock; // 서버, 클라이언트 소켓 번호를 저장할 변수 선언
	// 메시지를 저장할 문자형 배열 선언 	
	char message[BUF_SIZE];
	int str_len, i;
	
	struct sockaddr_in serv_adr;	// 서버 소켓 주소정보 구조체
	struct sockaddr_in clnt_adr;	// 클라이언트 소켓 주소정보 구조체 
	socklen_t clnt_adr_sz;
	
	//인자가 2개가 아닌 경우 
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	// 서버 소켓의 번호를 저장한다. 
	// socket() : 해당 소켓을 가리키는 소켓 디스크립터를 반환한다.
	// 성공 시 소켓 디스크립터 반환, 실패 시 -1 반환 
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(serv_sock==-1)
		error_handling("socket() error");
	
	// 서버 소켓의 주소정보를 설정한다. 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);

	// atoi() : 문자 스트링을 정수값으로 변환하는 함수다.
	// main의 인자로 전달된 포트 스트링을 정수값으로 변환한다.
	// 소켓 주소정보의 포트정보에 저장하기 위해 htons함수를 통해 네트워크 바이트 순서로된 16비트 숫자로 변형 
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	// serv_sock의 번호를 통해 서버소켓에 IP정보와 포트번호를 지정한다.
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	
	// 연결요청 소켓이 대기하는 연결 대기열을 생성한다.
	// 5는 연결 대기열의 크기다.
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	// 클라이언트 소켓의 주소정보 사이즈를 저장한다.
	clnt_adr_sz=sizeof(clnt_adr);
	
	// 연결대기열이 5개로 loop를 5번 반복시킨다.
	for(i=0; i<5; i++)
	{
		// 클라이언트의 소켓 번호 : clnt_sock
		// accept함수를 통해 연결된 연결을 가져와 새로운 소켓을 만든다.
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);
		
		// 클라이언트로부터 전달된 메시지를 read()함수를 통해서 읽는다.
		// 읽어들이며 메시지의 길이를 저장한다.
		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
			write(clnt_sock, message, str_len); // write함수를 통해 전달된 메시지를 재전송한다.
		// 연결을 종료하며 accept함수를 통해 생성된 소켓을 해제한다.
		close(clnt_sock);
	}

	close(serv_sock); // 서버소켓을 해제한다.
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}