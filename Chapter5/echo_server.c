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
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	int str_len, i;
	
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_sz;
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);    // 서버 소켓의 번호 할당, 소켓 생성 
	if(serv_sock==-1)
		error_handling("socket() error");
	
	// 서버의 주소정보 세팅 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	
	if(listen(serv_sock, 5)==-1) // 연결 요청 허용, 최대 5개의 클라이언트 제한 
		error_handling("listen() error");
	
	clnt_adr_sz=sizeof(clnt_adr); // 클라이언트 주소 사이즈 

	for(i=0; i<5; i++)
	{
		// 소켓의 연결을 받아들인다.
		// 성공 시 파일 디스크립터 반환, 실패 시 -1 반환 
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); 
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);
	
		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0) // 클라이언트로부터 메시지를 읽어들인다.
			write(clnt_sock, message, str_len);	// 읽은 메시지를 그대로 클라이언트에게 전송한다.

		close(clnt_sock);
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