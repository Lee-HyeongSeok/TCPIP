## * DNS : Domain Name System
#### * 출처 : 윤성우 TCPIP
#### * 사용환경 : 우분투 16ver
#### * 컴파일 : gcc source.c -o exe

#### * DNS server/service
			1. Domain Name을 IP로 변환해주는 서버나 서비스(server/service that converts domain name to ip)
			2. TCP/IP를 운영하는 기관에서 무료로 서비스한다(free service from organizations operating tcp/ip)
			3. 클라이언트는 DNS 서버에 접속하고자 하는 해당 서버 도메인 입력
					-> 서버는 해당 도메인의 IP주소 반환
					-> 클라이언트는 해당 IP를 이용하여 서버에 접속 실행

			4. DNS는 일종의 분산 데이터베이스 시스템이다.

#### * Domain Name   
			1. IP주소를 대신하는 서버의 고유 도메인 이름
			2. 실제 접속에 사용되는 주소 x -> 서버 도메인 이름을 ip 주소로 변환 후 접속에 사용 가능

			
#### * 도메인 이름을 이용하여 IP 주소 얻어오기
			* #include <netdb.h> 헤더 사용
			* struct hostent * gethostbyname(const char * hostname); 
			* gethostbyname 함수의 인자로 도메인 이름을 전달하면 해당 도메인의 서버 정보가 hostent 구조체 변수에 채워지고, 그 변수의 주소 값이 반환된다.
			* 성공 시 hostent 구조체 변수의 주소 값, 실패 시 NULL 포인터 반환 

#### * gethostbyname 함수의 호출 예
			* inet_ntoa() : 네트워크 바이트 순서 IP주소를 문자열 정보로 변환
			* 반복문의 구성을 통해 반환된 모든 정보 출력
```c
host = gethostbyname(argv[1]);
if(!host)
	error_handling(~)
printf("Official name : %s \n", host->h_name);
for(int i=0; i<host->h_aliases[i]; i++){
	printf("Aliases %d : %s \n", i + 1, host->h_aliases[i]);
printf("Address type : %s \n", (host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

for(int i=0; host->h_addr_list[i]; i++){
	printf("IP addr %d : %s \n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
return 0;
```   

#### * IP주소를 이용해서 도메인 정보 얻어오기
			* #include <netdb.h> 헤더 사용
			* struct hostent * gethostbyaddr(const char * addr, socklen_t len, int family);
						- addr : IP주소를 지니는 in_addr 구조체 변수의 포인터 전달
						- len : 첫 번째 인자로 전달된 주소 정보의 길이
									- IPv4 : 4 반환
									- IPv6 : 16 반환
						- family : 주소체계 정보 전달
									- IPv4 : AF_INET 전달
									- IPv6 : AF_INET6 전달 
			* gethostbyname 함수의 반대 기능 제공
			* 성공 시 hostent 구조체 변수의 주소 값, 실패 시 NULL 포인터 반환 
			
#### * gethostbyaddr 함수의 호출 예

```c
memset(&addr, 0, sizeof(addr));
addr.sin_addr.s_addr=inet_addr(argv[1]);
host=gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
if(!host)
	error_handling(~);

printf("Official name : %s \n", host->h_name);
for(int i=0; host->h_aliases[i]; i++)
	printf("Aliases %d : %s \n", i+1, host->h_aliases[i]);
printf("Address type : %s \n", (host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");
for(int i=0; host->h_addr_list[i]; i++)
	printf("IP addr %d : %s \n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
return 0;

```   

#### * gethostbyname 전체 소스 코드

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
void error_handling(char *message);

/*
gethostbyname 함수의 인자로 도메인의 이름 정보를 넣어 함수를 호출하면
해당 도메인의 서버 정보가 hostent 구조체 변수에 채워지며, 그 변수의 주소 값이 반환된다.

ex) www.naver.com = 210.x.x.x
*/
int main(int argc, char *argv[])
{
	int i;
	struct hostent *host;
	
	/*
	struct hostent{
		char *h_name; // 공식적인 도메인 이름
		char ** h_aliases; // 단축된 이름 리스트
		int h_addrtype; // 반환된 IP의 정보, 프로토콜 
		int h_length; // 반환된 IP 정보의 크기
		char ** h_addr_list; // IP주소 정보(둘 이상일 경우 반환)
	}
	*/

	if(argc!=2) {
		printf("Usage : %s <addr>\n", argv[0]);
		exit(1);
	}
	
	// 프로그램의 실행 인자로 전달한 도메인 이름을 gethostbyname() 함수에 전달 
	host=gethostbyname(argv[1]);
	if(!host)
		error_handling("gethost... error");

	// 공식 도메인 이름 출력 
	printf("Official name: %s \n", host->h_name);
	
	// 별칭 도메인 이름 출력 
	for(i=0; host->h_aliases[i]; i++)
		printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);
	
	// 반환된 IP의 정보 출력 IPv4 : AF_INET
	printf("Address type: %s \n", 
		(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

	// inet_ntoa() : 네트워크 바이트 순서 IP 주소를 문자열 정보로 변환한다.
	for(i=0; host->h_addr_list[i]; i++)
		printf("IP addr %d: %s \n", i+1,
					inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

```   

#### * gethostbyaddr 전체 소스 코드

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
void error_handling(char *message);

/*
gethostbyaddr()

struct hostent * gethostbyaddr(const char * addr, socklen_t len, int family);
- addr : IP주소를 지니는 in_addr 구조체 변수의 포인터 전달 
	 * IPv4 이외 다양한 정보를 전달받도록 일반화 하기 위해 매개변수를 char형 포인터로 선언 
- len : 첫 번째 인자로 전달된 주소 정보의 길이, IPv4 : 4, IPv6 : 16 전달
- family : 주소체계 정보 전달.
	IPv4: AF_INET
	IPv6 : AF_INET6
*/
int main(int argc, char *argv[])
{
	int i;
	struct hostent *host;
	/*
		struct hostent{
			char *h_name; // 공식 도메인 이름
			char **h_aliase; // 별칭 도메인 이름
			int h_addrtype; // 반환된 IP 정보, IPv4 : AF_INET 
			int h_length; // 반환된 IP 정보의 크기 
			char ** h_addr_list; // IP의 주소 정보(둘 이상일 경우 반환) 
	*/
	struct sockaddr_in addr;
	if(argc!=2) {
		printf("Usage : %s <IP>\n", argv[0]);
		exit(1);
	}

	// 주소 정보 할당 
	memset(&addr, 0, sizeof(addr));

	// 문자열 주소 정보를 네트워크 바이트 순서의 정수로 변환한다.
	addr.sin_addr.s_addr=inet_addr(argv[1]);

	// gethostbyaddr 함수 사용 
	// 위에서 addr 구조체 변수 addr의 sin_addr 주소정보를 인자로 전달 
	host=gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
	if(!host)
		error_handling("gethost... error");

	printf("Official name: %s \n", host->h_name);

	for(i=0; host->h_aliases[i]; i++)
		printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);
	
	printf("Address type: %s \n", 
		(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

	// inet_ntoa() : 네트워크 바이트 순서 IP주소를 문자열 정보로 변환한다.
	for(i=0; host->h_addr_list[i]; i++)
		printf("IP addr %d: %s \n", i+1,
					inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
```