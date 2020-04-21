#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t clnt_adr_sz;
	
	struct sockaddr_in serv_adr, clnt_adr; // 발신지 주소정보, 목적지 주소정보
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	// SOCK_DGRAM : 비연결 지향형 소켓 상수 값 
	// 전송되는 순서에 상관없이 가장 빠른 전송 지향 
	// 데이터 손실 및 에러 발생 가능
	// 한 번에 전송되는 데이터의 크기는 제한 
	// 데이터의 경계가 존재 
	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock==-1)
		error_handling("UDP socket creation error");
	
	// 자신의 주소 정보 할당 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	// bind 이유 : 데이터를 전송 받기위해 송신자가 참조할 주소 정보가 필요하기 때문 
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	while(1) 
	{
		clnt_adr_sz=sizeof(clnt_adr); // 송신자의 주소정보 길이로 초기화, 확인하기 위함 
		// 송신지 주소 정보를 할당한다.
		// 데이터를 받는 함수 
		// 데이터의 길이를 한번에 반환받는다. -> 데이터의 경계 존재  
		str_len=recvfrom(serv_sock, message, BUF_SIZE, 0, 
								(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		// recvfrom() 함수의 호출로인해 송신지 주소 정보가 할당된 상태 
		// 송신지 주소 정보를 목적지 주소 정보로 하여 받은 데이터 재전송(TCP 개념x)
		sendto(serv_sock, message, str_len, 0, 
								(struct sockaddr*)&clnt_adr, clnt_adr_sz);
	}	
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
