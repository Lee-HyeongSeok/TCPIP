#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	unsigned short host_port=0x1234; // 호스트 바이트 정렬 방식의 2바이트 데이터 
	unsigned short net_port; // 변환된 값을 받기위한 변수
	unsigned long host_addr=0x12345678; // 호스트 바이트 정렬 방식의 4바이트 데이터
	unsigned long net_addr; // 변환된 값을 받기위한 변
	
	// htons : 호스트 바이트 정렬 방식의 2바이트 데이터를 네트워크 바이트 정렬 방식으로 변환 
	net_port=htons(host_port);
	// htonl : 호스트 바이트 정렬 방식의 4바이트 데이터를 네트워크 바이트 정렬 방식으로 변환 
	net_addr=htonl(host_addr);
	
	// short : 2바이트, 포트 정보를 바꿀 때 사용 가능 
	// unsigned short htons(unsigned short) : 호스트 바이트 순서를 네트워크 바이트 순서로 바꾼다.(short형)
	// unsigned short ntohs(unsigned short) : 네트워크 바이트 순서를 호스트 바이트 순서로 바꾼다.(short형)
	// long : 4바이트, 아이피 정보를 바꿀 때 사용 가능
	// unsigned long htonl(unsigned long) : 호스트 바이트 순서를 네트워크 바이트 순서로 바꾼다.(long형)
	// unsigned long ntohl(unsigned long) : 네트워크 바이트 순서를 호스트 바이트 순서로 바꾼다.(long형)

	// 0x12345678, 0x12 : 상위 바이트 
	// 빅 엔디안 : 상위 바이트의 값을 작은 번지수에 저장 
	// 리틀 엔디안 : 상위 바이트의 값을 큰 번지수에 저장 
	// 네트워크 바이트 순서 : 빅 엔디안
	// 빅 엔디안 -> 리틀 엔디안 : 역순으로 보낸다. 
	printf("Host ordered port: %#x \n", host_port); // 빅 엔디안 
	printf("Network ordered port: %#x \n", net_port); // 리틀 엔디안 
	printf("Host ordered address: %#lx \n", host_addr); // 빅 엔디안
	printf("Network ordered address: %#lx \n", net_addr); // 리틀 엔디안 
	return 0;
}