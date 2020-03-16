#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main(void)
{	
	int fd1, fd2, fd3;
	/* 파일 디스크립터
	0 : 0을 이용해 데이터를 읽으면 키보드로 부터 데이터를 읽게된다.(표준 입력) 
	1 : 0을 이용해 데이터 출력 시 모니터로 데이터 출력이 이루어진다.(표준 출력) 
	2 : 표준 에러 

	// 소켓 생성 후 소켓 번호 반환 
	fd1=socket(PF_INET, SOCK_STREAM, 0);

	// open함수는 파일을 여는 함수, 파일 디스크립터를 반환한다.
	// 인자로는 파일 이름과 성격을 전달한다.
	// O_CREAT : 필요하면 파일 생성
	// O_WRONLY : 쓰기 전용으로 파일 오픈
	// O_TRUNC : 기존 데이터 전부 삭제 
	fd2=open("test.dat", O_CREAT|O_WRONLY|O_TRUNC);

	// 소켓 생성 후 소켓 번호 반환 
	fd3=socket(PF_INET, SOCK_DGRAM, 0);
	
	printf("file descriptor 1: %d\n", fd1);
	printf("file descriptor 2: %d\n", fd2);
	printf("file descriptor 3: %d\n", fd3);
	
	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}