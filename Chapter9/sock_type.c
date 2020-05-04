#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]) 
{
	int tcp_sock, udp_sock; // tcp, udp 파일 디스크립터 저장을 위한 변수
	int sock_type;	// 소켓 타입을 저장할 변수 
	socklen_t optlen;	// sock_type 변수로 전달된 옵션 정보의 바이트 크기를 담기 위함 
	int state;	// 상태를 저장할 변수 
	
	optlen=sizeof(sock_type);	// 변경할 옵션 정보를 저장할 변수의 크기를 받아온다. 
	tcp_sock=socket(PF_INET, SOCK_STREAM, 0);	// SOCK_STREAM : 연결 지향형 소켓 생성
	udp_sock=socket(PF_INET, SOCK_DGRAM, 0);	// SOCK_DGRAM : 비연결 지향형 소켓 생성 
	
	// 소켓 특성 파일 디스크립터 출력 
	printf("SOCK_STREAM: %d \n", SOCK_STREAM); 
	printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);
	
	// state : 메소드의 성공 및 실패에 대한 반환 값을 받는다. 
	// sock, level, optname, optval, optlen
	/* 
	sock : 옵션 확인을 위한 소켓의 파일 디스크립터 
	level : 확인할 옵션의 프로토콜 레벨 전달
	optname : 확인할 옵션의 이름 전달
	optval : 확인 결과의 저장을 위한 버퍼의 주소 값 전달
	optlen : optval로 전달된 주소 값의 버퍼 크기를 담고 있는 변수의 주소 값 전달 
	*/
	// tcp 소켓 타입 얻어오기 
	// SO_TYPE은 소켓 타입의 정보 확인만 가능, 변경 불가 
	state=getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("Socket type one: %d \n", sock_type);	// 소켓 타입 출력
	
	// udp 소켓 타입 얻어오기 
	state=getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("Socket type two: %d \n", sock_type);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}