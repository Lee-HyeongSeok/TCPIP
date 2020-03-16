#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handling(char* message);

int main(void)
{
	int fd; // 파일 디스크립터를 받을 변수 선언 
	char buf[]="Let's go!\n";
	
	// 파일 오픈 함수 실행 
	// O_CREAT : 해당 이름의 파일이 없을 시 파일 생성
	// O_WRONLY : 쓰기 전용으로 파일 오픈
	// O_TRUNC : 기존 데이터 전부 삭제 
	fd=open("data.txt", O_CREAT|O_WRONLY|O_TRUNC);
	
	if(fd==-1)// 파일 오픈 실패 시 에러 핸들링 함수 실행 
		error_handling("open() error!");
	// 파일 디스크립터 출력 
	printf("file descriptor: %d \n", fd);
	
	// 파일에 buf 배열의 문자열 쓰기, 실패 시 에러 메시지 출력 
	if(write(fd, buf, sizeof(buf))==-1)
		error_handling("write() error!");
	// 파일 디스크립터 닫기 
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
root@com:/home/swyoon/tcpip# gcc low_open.c -o lopen
root@com:/home/swyoon/tcpip# ./lopen
file descriptor: 3 
root@com:/home/swyoon/tcpip# cat data.txt
Let's go!
root@com:/home/swyoon/tcpip# 
*/
