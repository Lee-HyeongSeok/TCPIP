#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock; // 소켓의 파일 디스크립터 
	// 출력 버퍼 : 3K, 입력 버퍼 : 3K로 설정 
	int snd_buf=1024*3, rcv_buf=1024*3;
	int state; // 메소드의 실행 상태를 저장할 변수 
	socklen_t len;
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	// setsockopt 메소드에 SO_RCVBUF 옵션을 전달하여 rcv_buf의 버퍼 크기를 설정하는 동작 
	state=setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));
	if(state)
		error_handling("setsockopt() error!");
	// setsockopt 메소드에 SO_SNDBUF 옵션을 전달하여 snd_buf의 버퍼 크기를 설정하는 동작 
	state=setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));
	if(state)
		error_handling("setsockopt() error!");
	
	// 버퍼의 크기를 받아오기 위해 snd_buf 파일 디스크립터의 크기를 저장한다. 
	len=sizeof(snd_buf);
	// getsockopt 메소드에 SO_SNDBUF 옵션을 전달하여 snd_buf의 크기를 전달 받는 동작 
	state=getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);
	if(state)
		error_handling("getsockopt() error!");
	
	// 버퍼의 크기를 받아오기 위해 rcv_buf 파일 디스크립터의 크기를 저장한다. 
	len=sizeof(rcv_buf);
	// getsockopt 메소드에 SO_RCVBUF 옵션을 전달하여 rcv_buf의 크기를 전달 받는 동작 
	state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
	if(state)
		error_handling("getsockopt() error!");
	
	printf("Input buffer size: %d \n", rcv_buf);
	printf("Output buffer size: %d \n", snd_buf);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
/*
root@com:/home/swyoon/tcpip# gcc get_buf.c -o getbuf
root@com:/home/swyoon/tcpip# gcc set_buf.c -o setbuf
root@com:/home/swyoon/tcpip# ./setbuf
Input buffer size: 2000 
Output buffer size: 2048 
*/


