## * 윤성우 TCPIP Chapter4   
#### * 출처 : 윤성우 TCPIP   
#### * 목적 : Iterative 기반의 서버 및 클라이언트 구현   

#### * 주 사용 구조체   
		* struct sockaddr_in 구조체(struct in_addr 구조체)   
			* char sin_zero[8]
				* sockaddr과의 호환성을 위해 8바이트의 빈칸을 사용한다.
				* sockaddr_in 구조체의 크기를 16바이트로 맞췄다.

			* memset(&serv_adr, 0, sizeof(serv_adr)); 를 통해 0으로 초기화
			* <in.h> 헤더에 정의됨   
			* sin_family 주소체계 변수
				* AF_INET : IPv4 인터넷 주소체계, 소켓 생성 시 PF_INET으로 지정한 소켓에는 이 프로토콜 상수만 사용가능 					* AF_INET6 : IPv6 인터넷 주소체계
				* AF_UNIX : 유닉스 파일 주소체계 
				* AF_NS : XEROX 주소체계 

```c
struct sockaddr_in{
	short sin_family;	// 2바이트 주소체계
	unsigned short sin_port;	// 16비트 TCP/UDP 포트를 저장하는 2바이트 변수 
	struct in_addr sin_addr;	// 32비트 IPv4 주소를 저장하는 4바이트 구조체 
	char sin_zero[8];	// 사용되지 않는 패딩을 위한 배열 

struct in_addr{
	unsigned long s_addr;	// 32비트 IP주소를 저장할 구조체 
};
```   

#### * 메커니즘   

* server   
```c

// 연결 요청 대기열의 수를 5개로 지정했다.
if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	// 클라이언트 소켓의 주소정보 사이즈를 저장한다.
	clnt_adr_sz=sizeof(clnt_adr);

// 연결 요청 대기열에 따른 loop문 
for(i=0; i<5; i++)
	{
		// 클라이언트의 소켓 번호 : clnt_sock
		// accept함수를 통해 연결된 연결을 가져와 새로운 소켓을 만든다.
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);
		
		// 클라이언트로부터 전달된 메시지를 read()함수를 통해서 읽는다.
		// 읽어들이며 메시지의 길이를 저장한다.
		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
			write(clnt_sock, message, str_len); // write함수를 통해 전달된 메시지를 재전송한다.
		// 연결을 종료하며 accept함수를 통해 생성된 소켓을 해제한다.
		close(clnt_sock);
	}

```   

#### * client   
```c
	// connect() : 연결 대기중인 서버로 실제 연결을 요청하는 함수 
	// sock : 소켓 디스크립터 
	// serv_adr : 주소정보(접속하고자 하는 IP 및 server port정보)
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

// 위의 connect 함수를 통해 서버와 연결된 상태임 
// 사용자가 서버에 보낼 메시지로 q(Q)를 입력할 때 까지 loop 반복 
while(1) 
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin); // 메시지를 입력받는다.
		
		// strcmp함수로 입력한 문자가 끝내는 문자인 q나 Q인지 검사한다.
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;	// 맞으면 while문 종료 
		
		// 서버에 메시지 전송 
		write(sock, message, strlen(message));
	
		// 서버로부터 메시지를 읽는다.
		str_len=read(sock, message, BUF_SIZE-1);
		message[str_len]=0;	// 문자열 스트링에 끝을 의미하는 0을 넣어준다.
		printf("Message from server: %s", message);
	}
```   

