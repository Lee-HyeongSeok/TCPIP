#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 연결을 요청하는 소켓의 구현//
// 에러 핸들링 함수
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock; 연결에 사용할 소켓의 번호를 받기위한 선언 
	struct sockaddr_in serv_addr; // 연결할 서버 주소정보 구조체 변수 선언 
	char message[30];
	int str_len;
	
	if(argc!=3){ // 인자 전달 실패 시 메시지 출력 후 프로그램 종료 
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	// socket 함수를 통한 소켓 생성, IPv4 인터넷 프로토콜, SOCK_STREAM 타입 
	sock=socket(PF_INET, SOCK_STREAM, 0);

	if(sock == -1) // 소켓 생성 실패 시 에러 핸들링 함수로 에러문자열 출력 
		error_handling("socket() error");
	
	// 소켓 주소정보 구조체 변수 초기화 
	// 위 socket함수로 생성한 소켓이 아닌 연결할 소켓의 주소정보를 초기화 
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
		
	// connect 함수는 연결 요청 함수이며, 전화를 거는 상황에 비유 
	// sock에 저장된 소켓의 번호를 사용해 serv_addr구조체 변수에 
	// 저장된 주소정보를 가진 소켓에 연결하는 동작 
	// 서버 쪽에서는 accept 함수 반환, 연결된 상태에서는 write
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");
	
	// 클라이언트 쪽에서는 연결된 상태에서 read로 받고있다.
	str_len=read(sock, message, sizeof(message)-1);
	if(str_len==-1)
		error_handling("read() error!");
	
	// 읽은 데이터 출력
	printf("Message from server: %s \n", message);  
	close(sock); // 클라이언트 소켓 해제 요청 
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
