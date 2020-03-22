#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
	char *addr="127.232.124.79";
	
	// sockaddr_in 구조체의 주소멤버의 데이터 타입 : unsigned long
	// inet_aton()은 inet_addr()의 개선된 버전 
	// inet_aton() : 문자열을 빅 엔디언 32bit 값으로 변환 
	// 성공 시 1, 실패 시 0 반환 
	struct sockaddr_in addr_inet;
	
	if(!inet_aton(addr, &addr_inet.sin_addr))
		error_handling("Conversion error");
	else
		printf("Network ordered integer addr: %#x \n", addr_inet.sin_addr.s_addr);
	
	// 네트워크 정렬 방식을 호스트 정렬 방식으로 변환, 4바이트 long형 
	unsigned long host_addr = ntohl(addr_inet.sin_addr.s_addr);
	printf("Host ordered integer addr : %#lx \n", host_addr);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}