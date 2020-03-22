#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	char *addr1="127.212.124.78";
	char *addr2="127.212.124.256";
	
	// inet_addr() 함수 : Dotte-Decimal Natation 형식을 빅엔디안 32bit 값으로 변환시켜줌
	// unsigned long inet_addr(const char *string);
	// 성공 시 빅엔디안 형식의 32bit 값, 실패 시 INADDR_NONE을 리턴한다.
	unsigned long conv_addr=inet_addr(addr1);

	if(conv_addr==INADDR_NONE) // 실패 시 INADDR_NONE 리턴 
		printf("Error occured! \n");
	else // 성공 시 conv_addr에 저장된 빅엔디안 형식 32bit 값 출력
		printf("Network ordered integer addr: %#lx \n", conv_addr);
	
	// 두 번째 IP 변환
	// 127.212.124.256 으로 잘못된 값 입력 시 결과 
	conv_addr=inet_addr(addr2);
	
	if(conv_addr==INADDR_NONE)
		printf("Error occureded \n");
	else
		printf("Network ordered integer addr: %#lx \n\n", conv_addr);
	
	
	return 0;
}