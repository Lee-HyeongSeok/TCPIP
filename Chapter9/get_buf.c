#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;  // 소켓의 파일 디스크립터 
	int snd_buf, rcv_buf, state;	// 입, 출력 버퍼의 크기를 저장하기 위함 
	socklen_t len;	// 해당 버퍼의 길이를 저장하기 위함 
	
	sock=socket(PF_INET, SOCK_STREAM, 0);	// 소켓 생성 

	// 출력 버퍼의 크기를 담고있는 snd_buf의 길이를 저장  
	len=sizeof(snd_buf);

	// 해당 소켓 파일 디스크립터를 통해 snd_buf로 전달된 주소 값의 버퍼 크기를 담고 있는 변수의 주소 값을 전달 
	state=getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);
	if(state)
		error_handling("getsockopt() error");
	
	// 입력 버퍼의 크기를 담고 있는 rcv_buf의 길이를 저장 
	len=sizeof(rcv_buf);
	// 해당 소켓 파일 디스크립터를 통해 rcv_buf로 전달된 주소 값의 버퍼 크기를 담고 있는 변수의 주소 값을 전달 
	state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
	if(state)
		error_handling("getsockopt() error");
	
	printf("Input buffer size: %d \n", rcv_buf); // 입력 버퍼 크기 출력 
	printf("Outupt buffer size: %d \n", snd_buf); // 출력 버퍼 크기 출력 
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}