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
	int sock;
	char message[BUF_SIZE]; // 메시지 전송을 위한 배열 
	int str_len;
	socklen_t adr_sz;
	
	struct sockaddr_in serv_adr, from_adr; // 목적지 주소정보 구조체 변수, 송신지 확인을 위한 구조체 변수 
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);  // 소켓을 생성하여 파일 디스크립터를 저장 
	if(sock==-1)
		error_handling("socket() error");
	
	// 목적지에 대한 주소정보 할당 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	while(1)
	{
		fputs("Insert message(q to quit): ", stdout);
		fgets(message, sizeof(message), stdin);     
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))	
			break;
		
		// sendto() : 목적지 주소정보가 정해져있을 때 사용 
		// 데이터 송신 함수 
		sendto(sock, message, strlen(message), 0, 
					(struct sockaddr*)&serv_adr, sizeof(serv_adr));
		// 송신지 확인을 위해 송신지 주소 정보 길이로 초기화 
		adr_sz=sizeof(from_adr);

		// 데이터를 수신받는 함수 
		// 데이터 길이를 반환받는다. 
		str_len=recvfrom(sock, message, BUF_SIZE, 0, 
					(struct sockaddr*)&from_adr, &adr_sz);

		message[str_len]=0; // 끝에 0을 넣어줌으로 print시 문자열의 끝인 NULL을 표시해주기 위함 
		printf("Message from server: %s", message);
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