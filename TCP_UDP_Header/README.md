## * TCP/UDP 헤더 출력구현   
#### * 출처 : 네트워크 패킷 포렌식   
#### * 사용헤더   
	* <pcap.h>
	* <unistd.h>
	* <arpa/inet.h>
	* <stdlib.h>   

#### 프로그래밍 환경 : 우분투 16ver   
#### 컴파일 : gcc source.c -o exe -lpcap


#### * TCP헤더의 각 필드 내용   
			* Source Port Number(16bit) : 송신자의 포트번호가 할당된다.
			* Destination Port Number(16bit) : 목적지 대상 서버 포트번호가 할당된다.
			* Sequence Number(32bit) : 데이터의 순서를 나타내는 번호가 할당되며 분할된 패킷의 경우 데이터 재조합에 사용된다.
			* Acknowledge Number(32bit) : 다음에 수신할 데이터 번호이며 해당 번호로 몇 번째 데이터 인지를 파악하게 된다.
			* Offset(Header Length) : TCP헤더길이를 제공한다.
			* Control Flags : 6개 비트로 구성되어 비트 조합을 통해 통신 시작 및 해제, 데이터 전송 등의 통신상태를 제공한다.
			* Windows Size : 한 번에 받을 수 있는 패킷 사이즈를 의미하며, 사이즈가 서로 다를 시 작은 사이즈에 맞추어 데이터를 송수신한다.
			* TCP Checksum : 헤더 값의 에러발생여부를 검사하기 위해 사용한다.
			* Urgent pointer : 플래그 필드에 URG 플래그가 세팅된 패킷에서만 유효하다.
			* Options : 상세한 조정기능을 위해 예약되어 있다.
			* Padding : 헤더가 가변적이기 때문에 헤더의 크기를 일정하게 맞추기 위해 사용한다.   

```c
typedef struct tcp_header{
	u_short sport;	// 전송지 포트(16비트)
	u_short dport;	// 목적지 포트 (16비트)
	u_int seqnum;	// 데이터 순서를 나타내는 번호(32비트)
	u_int acknum;	// 다음에 수신할 데이터 번호(32비트) 
	u_char th_off;	// offset, tcp 헤더 길이 제공
	u_char flags;	// 통신 상태 제공 플래그(6개 비트로 구성) 
	u_short win;		// 윈도우 사이즈 
	u_short crc;		// tcp 체크섬 비트
	u_short urgptr;	// Urgent pointer
}tcp_header;
```   


#### * Control Flags   

| 값 | 의미 |
|---|:---:|
| URG | 긴급 요청 |
| ACK | 응답 메시지 |
| PSH | 수신한 데이터를 어플리케이션계층으로 즉시 전달 |
| RTS | 강제 세션 종료 |
| SYN | 연결 요청 | 
| FIN | 정상 세션 종료 |   

```c
		// TCP 플래그를 확인하는 코드 
		if((th->flags) == SYN){
			printf("Flags : SYN\n");
			printf("\n");
			printf("\n");
		}
		else if((th->flags) == PUSH){
			printf("Flags : PUSH\n");
			printf("\n");
			printf("\n");
		}
		else if((th->flags) == ACK){
			printf("Flags : ACK\n");
			printf("\n");
			printf("\n");
		}	
		else if((th->flags) == SYN_ACK){
			printf("Flags : SYN, ACK\n");
			printf("\n");
			printf("\n");
		}
		else if((th->flags) == PUSH_ACK){
			printf("Flags : PUSH, ACK\n");
			printf("\n");
			printf("\n");
		}
		else if((th->flags) == FIN_ACK){
			printf("Flags : FIN, ACK\n");
			printf("\n");
			printf("\n");
		}	
		else{
			printf("Flags(Unknown) : %04x\n", th->flags);
		}
```
#### * TCP 통신 메커니즘(3-Way-HandShaking)   
			1. 통신 시작(세션 연결)
					- SYN, SYN+ACK, ACK의 Flags 조합을 통해 상대방의 요청 확인 및 승낙 작업 수행 
			2. 데이터 송.수신
					- 사용자 : 홈 페이지 초기 파일을 요청한다.
					- 웹 서버 : 요청한 초기파일을 만들어 다시 사용자에게 전달한다.
					+ PUSH를 통해 요청한 데이터에 대한 빠른 처리를 요청한다.
			3. 통신 종료
					- 통신을 시작한 쪽에서 FIN+ACK의 Flags 조합으로 요청한다.
					- 수신자는 동일 방식으로 FIN+ACK를 보내 상호간 합의하는 식으로 통신을 종료한다.


#### * UDP헤더의 각 필드 내용   
			* Source Port : 송신측 포트번호를 세팅한다.
			* Destination Port : 수신측 포트번호를 세팅한다.
			* Length : 헤더와 데이터의 사이즈를 세팅한다.
			* Checksum : 데이터의 훼손유무를 확인한다.

```c
typedef struct udp_header{
	u_short sport;	// 송신자 포트번호
	u_short dport;	// 수신자 포트번호
	u_short len;		// UDP 데이터길이
	u_short crc;		// 체크섬 
}udp_header;
```
   

#### * UDP 통신 메커니즘   
			1. 데이터 정상전달
				- 상대방의 수신준비를 위한 사전논의(3-way-handshaking)없이 데이터를 전달한다.
			2. 전달 중 데이터 손실 발생
				- 해당 데이터를 폐기하고 재요청하지 않는다.
			3. 잘못된 포트 접속시도
				- 대상 서버가 제공하지 않는 포트 접속 시 ICMP 프로토콜이 대신 처리한다.
				- UDP에는 에러처리기능이 없다.
				- Port Unreachable 메시지를 상대방에게 전달한다.