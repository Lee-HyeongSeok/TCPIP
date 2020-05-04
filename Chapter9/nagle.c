#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[]){
	int sock;	// 소켓 파일 디스크립터 
	int state;	// 소켓의 상태
	int opt_val=0;	// 옵션 값
	socklen_t opt_len; // 소켓의 옵션 길이 

	sock = socket(PF_INET, SOCK_STREAM, 0);
	opt_len = sizeof(opt_val);

	// Nagle 알고리즘의 설정 상태를 확인하는 코드이다.
	// TCP_NODELAY 옵션을 할당 
	if(getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &opt_len))
		error_handling("getsockopt() error");
	else
		printf("getsockopt : TCP_NODELAY : opt_val = %d \n", (int)opt_val);

	// Nagle 알고리즘의 중단(OFF)을 명령 
	// TCP_NODELAY 옵션을 전달하여 Nagle 알고리즘의 설정을 변경하는 동작이다.
	opt_val = 1; // opt_val을 1로 설정 
	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val)))
		error_handling("setsockopt() error");
	else
		printf("setsockopt: TCP_NODELAY : opt_val = %d \n", (int)opt_val);
	// 변경된 Nagle 알고리즘의 상태를 확인하는 코드 
	if(getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &opt_len))
		error_handling("setsockopt() error");
	else
		printf("getsockopt : TCP_NODELAY : opt_val = %d \n", (int)opt_val);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
