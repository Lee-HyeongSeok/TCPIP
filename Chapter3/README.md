## 주소체계와 데이터 정렬   
#### 출처 : 윤성우 TCPIP   
#### URL : orentec.co.kr   
#### 사용환경 : 우분투 16   

> Chapter3
>	> inet_addr.c
>	> inet_aton.c
>	> inet_ntoa.c
>	> endian_conv.c   

## * 호스트 바이트 순서
	* 컴퓨터 시스템에 따라 빅 엔디언, 리틀 엔디언을 사용한다.   
## * 네트워크 바이트 순서   
	* 빅 엔디언 방식을 적용한다.   
======================================================================================================
#### * inet_addr.c 사용 헤더   
#### stdio.h   
#### arpa/inet.h   

## inet_addr.c 소스 코드   
<pre>
<code>
int main(int argc, char *argv[])
{
	char *addr1="127.212.124.78";	// 점으로 표현한 십진수(Dotted-Decimal Notation)
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
</code>
</pre>   

#### inet_addr.c 주 사용 함수   
* inet_addr() : Dotted-Decimal Notation 형식을 빅 엔디안 32비트 값으로 변환시켜준다.
	* 자료형은 32비트 값이므로 unsigned long을 따른다.
	* 원형 : unsigned long inet_addr(const char *string);
	* 성공 시 32비트의 값, 실패 시 INADDR_NONE을 리턴한다.   
====================================================================================================

#### * inet_aton.c 사용 헤더   
#### stdio.h  
#### stdlib.h
#### arpa/inet.h   

## inet_aton.c 소스 코드   
<pre>
<code>
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
</code>
</pre>

#### inet_aton 주 사용 함수
* inet_aton()
	* inet_addr()의 개선된 버전이다.
	* 점으로 표현된 십진수 문자열을 빅 엔디언 32비트 값으로 변환한다.
	* 성공 시 1, 실패 시 0을 반환한다.
* ntohl()
	* 네트워크 정렬 방식을 호스트 정렬 방식으로 변환한다.
	* 4바이트 long형의 자료형을 따른다.   
====================================================================================================

#### * inet_ntoa.c 사용 헤더   
#### stdio.h
#### string.h
#### arpa/inet.h   

## inet_ntoa.c 소스 코드 
<pre>
<code>
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

</code>
</pre>   

#### inet_ntoa 주 사용 함수 
* htonl()
	* 호스트 정렬 방식을 네트워크 정렬 방식으로 변환한다.(long형)
	* h : 호스트, n : 네트워크, l : long 자료형을 나타낸다.   
* inet_ntoa()
	* 네트워크 바이트 순서의 32비트 값을 Dotted-Decimal Notaion의 주소값으로 변환한다. 
==================================================================================================

#### * endian_conv.c 사용 헤더   
#### stdio.h
#### arpa/inet.h   

## endian_conv.c 소스 코드 
<pre>
<code>
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
</code>
</pre>

#### endian_conv 주 사용 함수 
* htons()
	* 호스트 바이트 정렬 방식의 2바이트 데이터를 네트워크 바이트 정렬 방식으로 변환한다.
	* h : 호스트, n : 네트워크, s : short 자료형을 의미한다.   
* htonl()
	* 호스트바이트 정렬 방식의 4바이트 데이터를 네트워크 바이트 정렬 방식으로 변환한다. 
	* h : 호스트, n : 네트워크, l : long 자료형을 의미한다.
