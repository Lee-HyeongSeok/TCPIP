#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 에러 메시지 출력 함
void error_handling(char *message);

int main(int argc, char *argv[])
{
	// 소켓을 이용할 때 이 소켓 번호가 필요하다.
	int serv_sock; // 소켓 생성함수 반환 값을 받기 위한 선언(서버)
	int clnt_sock; // 소켓 생성함수 반환 값을 받기 위한 선언(클라이언트)

	struct sockaddr_in serv_addr; // 서버소켓 주소정보 구조체
	struct sockaddr_in clnt_addr; // 클라이언트 소켓 주소정보 구조체
	socklen_t clnt_addr_size; // 클라이언트 소켓 길이
	
	// 서버에서 사용할 메시지
	char message[]="Hello World!";
	
	// 인자 전달이 잘못될 경우 메시지 출력 후 프로그램 종
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	// socket 함수 호출로 서버소켓 생성
	// PF_INET은 IPv4 인터넷 프로토콜
	// SOCK_STREAM은 소켓 타입
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	// 소켓 생성 실패 시 소켓 에러 메시지 함수 인자로 전달 및 출력
	if(serv_sock == -1)
		error_handling("socket() error");
	// 주소 정보를 할당하기 위한 구조체 변수 초기화
	memset(&serv_addr, 0, sizeof(serv_addr));
	// 소켓 주소정보 초기화
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	// bind 함수 호출로 IP와 포트번호 할당
	// serv_sock에 해당되는 소켓에 주소정보(serv_addr)를 할당해주는 동작
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error"); 

	// listen함수로 연결 요청 가능상태로 변경
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error"); // 실패 시 에러 메시지 출
	// 클라이언트 소켓 크기 정보
	clnt_addr_size=sizeof(clnt_addr); 

	// accept함수로 클라이언트 소켓 연결 요청에 대한 수락
	// accept함수 = 블로킹 함수, 일단 호출 후 OS에 확인 후 요청 대기상태가 된다.
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
	
	// 실패 시 에러 메시지 출	
	if(clnt_sock==-1) 
		error_handling("accept() error");  

	// 데이터 전송, hello world 메시지 클라이언트에 출력
	write(clnt_sock, message, sizeof(message));
	close(clnt_sock); // 운영체제에 클라이언트 소켓 해제 요청
	close(serv_sock); // 서버 소켓 해제
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr); // 인자로 전달받은 에러 메시지 출력
	fputc('\n', stderr);
	exit(1);
}
