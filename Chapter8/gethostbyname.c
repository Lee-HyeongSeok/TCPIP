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
