###윤성우 TCPIP 코드 해석
## 출처 : orentec.co.kr
## 프로그래밍 환경 : 우분투 16.04.6 가상머신 환경 


#주 사용 구조체 			#관련 헤더
#sockaddr_in			#sys/socket.h
				#netinet/in.h
				#netinet/ip.h

#용도 : 소켓의 주소를 저장한다. 

```c
struct sockaddr_in{
	short sin_family;	// 주소 체계 : AF_INET, IPv4 주소 체계 사용 
	u_short sin_port;	// 16비트(2바이트) 포트 번호, unsigned short 형 
	struct in_addr sin_addr;	// 32비트(4바이트) IP 주소, unsigned long형 
	char sin_zero[8]; // 전체 크기를 16비트로 맞추기 위한 더미 
};
```
#sin_family : 항상 AF_INET을 설정한다.
#sin_port : 1. 포트 번호를 가진다. 
#	       2. 0~65535의 범위를 갖는 숫자 값
#	       3. 변수에 저장하기 위해서는 네트워크 바이트 정렬이어야 한다.
#	       4. 1024 이하의 포트 번호는 예약된 포트이기 때문에 권한을 가진 프로세스만이 바인딩할 수 있다.
#	          리눅스 기준 권한을 가진 프로세스 : CAP_NET_BIND_SERVICE라는 capability를 가진 사용자 영역의 프로세스 지칭 
#sin_addr : 호스트 IP주소이다. 주로 inet_aton(), inet_addr(), inet_makeaddr()과 같은 라이브러리가 제공하는
#		함수의 반환값이 저장되어야 한다.
#sin_zero : 8바이트의 더미 데이터이며, 반드시 모두 0으로 채워져 있어야 한다.
# sockaddr_in의 sin_zero를 제외한 크기는 8바이트 이므로 sin_zero를 추가하여 struct sockaddr 구조체와
# 크기를 일치 시키려는 목적이다.(패팅 바이트, 혹은 데이터)

#sockaddr_in 안에 구조체 in_addr

```c
struct in_addr{
	u_long s_addr; // unsigned long형, 4바이트 
};		
```

# 소켓의 주소정보 삽입 내용
```c
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

```

## hello_server.c의 메커니즘
#1.인자 전달 -> socket(PF_INET, SOCK_STREAM, 0); 으로 소켓 생성
#2.serv_addr.~ 을 통해 주소 정보 초기화
#3. bind()함수 호출로 주소 정보 할당 
#4. listen()함수로 연결 요청 상태로 변경 
#5.  accept() 함수로 클라이언트 연결 요청에 수락 대기 상태 및 수락 
#6. write() 함수로 클라이언트에게 메시지 전달 
#7. close() 함수로 소켓 메모리 해제   