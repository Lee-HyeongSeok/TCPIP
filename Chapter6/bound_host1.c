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
	char message[BUF_SIZE];
	// 송신지 주소 정보, 목적지 주소 정보 
	struct sockaddr_in my_adr, your_adr;
	socklen_t adr_sz;
	int str_len, i;

	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	// SOCK_DGRAM : 비연결 지향형 소켓으로 소켓 생성 
	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
		error_handling("socket() error");
	
	// 프로그램 인자로 전달된 주소 정보를 할당 
	memset(&my_adr, 0, sizeof(my_adr));
	my_adr.sin_family=AF_INET;
	my_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	my_adr.sin_port=htons(atoi(argv[1]));
	
	// 데이터를 수신받기 위한 송신지에서 참조할 주소 정보를 할당 
	if(bind(sock, (struct sockaddr*)&my_adr, sizeof(my_adr))==-1)
		error_handling("bind() error");
	
	for(i=0; i<3; i++) // 데이터를 3회 수신 받음 
	{
		// 수신받은 데이터가 한 번에 버퍼로 전부 들어가는지 확인하기 위함 
		// 데이터의 경계가 존재하는가 
		sleep(5);	// delay 5 sec.
		adr_sz=sizeof(your_adr); // 송신지 주소 정보 사이즈로 초기화 
		// recvfrom() 함수 호출과 동시에 송신지 주소정보 할당 
		str_len=recvfrom(sock, message, BUF_SIZE, 0, 
								(struct sockaddr*)&your_adr, &adr_sz);     
	
		printf("Message %d: %s \n", i+1, message);
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

/*
root@my_linux:/home/swyoon/tcpip# gcc bound_host1.c -o host1
root@my_linux:/home/swyoon/tcpip# ./host1 
Usage : ./host1 <port>
root@my_linux:/home/swyoon/tcpip# ./host1 9190
Message 1: Hi! 
Message 2: I'm another UDP host! 
Message 3: Nice to meet you 
root@my_linux:/home/swyoon/tcpip# 

*/
