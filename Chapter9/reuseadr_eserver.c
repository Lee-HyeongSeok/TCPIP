#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock; // 서버 소켓, 클라이언트 소켓 
	char message[30];
	int option, str_len;	// 옵션 저장 변수, 문자열 길이 저장 변수 
	socklen_t optlen, clnt_adr_sz;	// 옵션 길이 정보 저장 변수, 클라이언트 주소 정보 사이즈 
	struct sockaddr_in serv_adr, clnt_adr;	// 서버, 클라이언트 소켓 주소 정보 구조체 변수 
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓 생성 
	if(serv_sock==-1)
		error_handling("socket() error");
	
	// 주소 재할당 설정 
	// setsockopt 메소드에 SO_REUSEADDR 옵션을 할당하여 주소의 재할당을 설정하는 동작 
	/*
	optlen=sizeof(option);
	option=TRUE;	
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, optlen);
	*/

	// 기존의 tcp 통신 방식 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
		error_handling("bind() error ");
	
	if(listen(serv_sock, 5)==-1)
		error_handling("listen error");
	clnt_adr_sz=sizeof(clnt_adr);    
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);

	while((str_len=read(clnt_sock,message, sizeof(message)))!= 0)
	{
		write(clnt_sock, message, str_len);
		write(1, message, str_len);
	}
	close(clnt_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}