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
	int sd;
	FILE *fp;
	
	char buf[BUF_SIZE];
	int read_cnt;
	struct sockaddr_in serv_adr;
	if(argc!=3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	fp=fopen("receive.dat", "wb"); // 파일 오픈함수로 파일 오픈 
	sd=socket(PF_INET, SOCK_STREAM, 0);  // TCP 특성 소켓 생성   

	// 서버 주소정보 초기화 및 할당 
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	// 서버에 연결 요청 
	connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	
	// read() 함수로 30이 넘지 않도록 파일의 끝 까지 데이터를 읽어온다.
	while((read_cnt=read(sd, buf, BUF_SIZE ))!=0)
		fwrite((void*)buf, 1, read_cnt, fp); // 읽어옴과 동시에 파일에 써준다.
	
	puts("Received file data");
	write(sd, "Thank you", 10); // 서버에 문자열 전송 
	fclose(fp); // 파일 닫기
	close(sd); // 소켓 파일디스크립터로 닫기 
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}