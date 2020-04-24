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
	int serv_sd, clnt_sd;
	FILE * fp;
	char buf[BUF_SIZE];
	int read_cnt;
	
 	// 서버 자신의 주소정보, 클라이언트 주소정보 구조체 변수 선언 
	struct sockaddr_in serv_adr, clnt_adr; 
	socklen_t clnt_adr_sz; // 서버 주소정보 사이즈를 위한 변수 선언 
	
	if(argc!=2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	fp=fopen("file_server.c", "rb"); 
	// 서버 소켓 파일디스크립터 반환 
	// TCP형 서버 소켓 생성 
	serv_sd=socket(PF_INET, SOCK_STREAM, 0);   
	
	// 서버 주소정보 초기화 및 할당 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	// bind() 함수로 소켓 파일디스크립터에 서버 주소정보를 할당해준다.
	bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	listen(serv_sd, 5); // 연결 요청 리스너 
	
	clnt_adr_sz=sizeof(clnt_adr);    

	// accept 함수로 클라이언트의 요청을 허락한다.
	// 파일디스크립터를 반환하여 클라이언트 소켓 파일 디스크립터에 복사한다.
	clnt_sd=accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
	while(1)
	{
		// fread를 통해 파일의 내용을 읽는다.
		// 내용을 버퍼에 입력 
		read_cnt=fread((void*)buf, 1, BUF_SIZE, fp);
		if(read_cnt<BUF_SIZE)
		{
			// 파일 내용을 가진 버퍼를 클라이언트에게 전송 
			write(clnt_sd, buf, read_cnt);
			break;
		}
		// 버퍼를 클라이언트에게 전송 
		write(clnt_sd, buf, BUF_SIZE);
	}
	// 서버의 데이터 전송은 끝난 상태 

	// shutdown 함수로 클라이언트 소켓에게 서버의 전송이 끝남을 알림 
	// 출력 스트림 닫음 
	// 소켓 재활용 불가 
	shutdown(clnt_sd, SHUT_WR);	
	read(clnt_sd, buf, BUF_SIZE); // 클라이언트로부터 버퍼 입력받는다.
	printf("Message from client: %s \n", buf);
	
	fclose(fp);
	close(clnt_sd); close(serv_sd);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
