#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr; // 서버소켓의 주소정보
	struct sockaddr_in clnt_addr; // 클라이언트 소켓의 주소정보 
	socklen_t clnt_addr_size; // 클라이언트 소켓의 길이 

	char message[]="Hello World!"; // 서버에서 출력할 메시지 
	
	if(argc!=2){ // 인자 전달 실패시 오류 메시지 출력 
		printf("Usage : %s <port>\n", argv[0]);
		exit(1); // 종료 
	}
	
	// 서버 소켓 생성, socket 함수 호출
	// PF_INET은 IPv4 인터넷 프로토콜을 의미한다.

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	if(serv_sock == -1) // 소켓 생성 실패 시 에러 메시지 출력
		error_handling("socket() error");
	// 소켓 주소정보 초기화 
	memset(&serv_addr, 0, sizeof(serv_addr));

	// 서버소켓 주소정보 할당
	// INADDR_ANY : 이 컴퓨터에 존재하는 랜카드 중 사용가능한 랜카드의 IP주소를 사용하라는 뜻 
	// htonl 함수 : long 형 데이터의 바이트 오더를 바꿔주는 함수 
	serv_addr.sin_family=AF_INET; // AF_INET은 IPv4 인터넷 프로토콜
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	// bind 함수 호출로 인자로 전달한 서버 소켓 번호, 해당되는 주소로 IP, PORT 번호 할당
	// 서버 소켓 번호에 해당되는 소켓에 할당된 주소정보 삽입 


	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error"); 
	

	// listen함수로 연결 요청 가능 상태로 변경 


	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	// 클라이언트 주소정보 크기를 받아온다.
	clnt_addr_size=sizeof(clnt_addr);  

	// accept함수는 블로킹 함수라고도 한다.
	// accept함수 호출 후 클라이언트에서 부터 오는 소켓 연결 요청을 기다린다.

	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);


	if(clnt_sock==-1) // 실패 시 오류 메시지 출력
		error_handling("accept() error");  
	// 성공 시 클라이언트에게 메시지 데이터 전송 


	write(clnt_sock, message, sizeof(message));


	close(clnt_sock); // 클라이언트 소켓 닫기 
	close(serv_sock); // 서버 소켓 닫기 
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
