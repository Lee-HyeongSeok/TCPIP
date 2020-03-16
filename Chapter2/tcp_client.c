#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock; //소켓 생성을 위한 변수 선언 
	struct sockaddr_in serv_addr; // 서버 소켓 주소정보를 위한 구조체 변수 선언 
	char message[30]; // 메시지 배열 
	int str_len=0; // 소켓 정보 길이를 위한 변수 선언 
	int idx=0, read_len=0;
	
	if(argc!=3){ // 인자 전달이 부족할 때 오류 메시지 출력 후 프로그램 종료 
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	// 소켓 생성 후 해당 소켓 번호 반환 
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	// 서버 소켓 주소정보를 받기위한 구조체 변수 초기화 
	memset(&serv_addr, 0, sizeof(serv_addr));

	// 서버 소켓 주소정보 세팅 
	// AF_INET : IPv4 인터넷 프로토콜을 의미 
	// htons 함수 : 호스트 시스템에서 네트워크로 short형 데이터를 보낼 때 바이트 오더를 바꿔주는 함수 
	serv_addr.sin_family=AF_INET;

	// 인자로 전달된 IP를 서버 소켓 구조체 변수에 삽입하기 위한 동작
	// inet_addr 함수 : 숫자와 점으로 이루어진 IP 문자열을 long형의 숫자 IP주소로 바꿔준다.
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	// connect 함수를 통해서 서버에 연결요청 
	// 서버 구조체 변수에 저장된 주소로 해당 인자로 전달된 소켓 번호와 연결 
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");
	
	// read함수로 서버에서 보내는 데이터를 한 문자씩 읽는다. 
	while(read_len=read(sock, &message[idx++], 1))
	{
		if(read_len==-1)
			error_handling("read() error!");
		
		str_len+=read_len; // 문자열 길이 합산 
	}
	
	// 받아온 메시지와 길이 출력 
	printf("Message from server: %s \n", message);
	printf("Function read call count: %d \n", str_len);
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
