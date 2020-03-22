#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	// sockaddr_in 구조체의 주소멤버의 데이터 타입 : unsigned long
	struct sockaddr_in addr1, addr2;
	char *str_ptr;
	char str_arr[20];
   
	// 호스트 정렬 방식을 네트워크 정렬 방식으로 변환(long형)
	addr1.sin_addr.s_addr=htonl(0x1020304); 
	addr2.sin_addr.s_addr=htonl(0x1010101);
	
	// 문자형 포인터 변수에 저장 
	// inet_ntoa() : 네트워크 바이트 순서의 32bit 값을 Dotted-Decimal Notation의 주소값으로 변환 
	str_ptr=inet_ntoa(addr1.sin_addr);

	strcpy(str_arr, str_ptr); // 포인터에 있는 값을 20바이트 배열에 복사 
	// 10진수 주소 값으로 표현 
	printf("Dotted-Decimal notation1: %s \n", str_ptr);
	
	inet_ntoa(addr2.sin_addr);
	// 포인터의 값 출력, 동적 할당이므로 값이 바뀜 
	printf("Dotted-Decimal notation2: %s \n", str_ptr);
	printf("Dotted-Decimal notation3: %s \n", str_arr);
	return 0;
}


