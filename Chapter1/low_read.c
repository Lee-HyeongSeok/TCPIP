#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 100

void error_handling(char* message);

int main(void)
{
	int fd;
	char buf[BUF_SIZE];
	
	// 읽기 전용으로 data.txt 오픈 
	fd=open("data.txt", O_RDONLY);
	if( fd==-1) // 실패 시 에러 핸들링 함수 실행 
		error_handling("open() error!");
	
	// 파일 디스크립터 출력 
	printf("file descriptor: %d \n" , fd);
	
	// 파일의 끝 까지 읽기, 실패 시 에러 메시지 출력 
	if(read(fd, buf, sizeof(buf))==-1)
		error_handling("read() error!");

	// buf 출력 
	printf("file data: %s", buf);
	
	close(fd);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*
root@com:/home/swyoon/tcpip# gcc low_read.c -o lread
root@com:/home/swyoon/tcpip# ./lread
file descriptor: 3 
file data: Let's go!
root@com:/home/swyoon/tcpip# 
*/
