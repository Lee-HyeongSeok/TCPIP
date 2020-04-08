## * 계정 및 암호가 포함된 패킷수집 구현   
#### * 출처 : 네트워크 패킷 포렌식   
#### * 목적 : 인증정보 수집을 위한 Data 조사   
#### 컴파일 환경 : 우분투 16ver
		* gcc pkt_dump.c -o pkt_dump -lpcap

### * 사용헤더   
		* pcap.h
		* unistd.h
		* time.h
		* stdlib.h
		* arpa/inet.h


#### * 주 사용 구조체   
		* mac_address   
	
		```c
		typedef struct mac_address{
			u_char byte1;
			u_char byte2;
			u_char byte3;
			u_char byte4;
			u_char byte5;
			u_char byte6;
		}mac_address;
		```   
		
		* ip_header   

		```c
		typedef struct ip_header{
			u_char ver_ihl;	// 버전 + 인터넷 헤더 길이 
			u_char tos;		// 서비스 타입
			u_short tlen;	// 전체 길이 
			u_short identification;	// identification
			u_short flags_fo;		// 플래그 + 플래그 오프셋
			u_char ttl;			// TTL값, 패킷 수명
			u_char proto;		// 상위 프로토콜
			u_short crc;			// 헤더 체크섬 
			ip_address saddr;		// 발신지 IP
			ip_address daddr;		// 수신지 IP
			u_int op_pad;		// 옵션 + 패딩 
		}ip_header;
		```   
		* tcp_header   

		```c
		typedef struct tcp_header{
			u_short sport;	// 송신자 포트번호
			u_short dport;	// 수신자 포트번호
			u_int seqnum;	// 데이터의 순서
			u_int acknum;	// 수신할 데이터 
			u_char th_off;	// TCP헤더 길이 제공 
			u_char flags;	// 통신상태 플래그 
			u_short win;		// 윈도우사이즈
			u_short crc;		// 체크섬 
			u_short urgptr;	// Urgent pointer
		}tcp_header;
		```   
		* udp_header   

		```c
		typedef struct udp_header{
			u_short sport;	// 송신자 포트번호
			u_short dport;	// 수신자 포트번호
			u_short len;		// UDP 데이터 길이
			u_short crc;		// 체크섬 
		}udp_header;
		```   

#### * 현재 시스템 시간 출력을 위한 pcap_pkthdr 구조체 사용

```c
struct pcap_pkthdr{
	struct timeval ts;	// 패킷이 캡쳐된 시간정보
	bpf_u_int32 caplen;	// 수집된 패킷 길이
	bpf_u_int32 len;	// 실제 패킷 길이 
}header;
```   


#### * 계정 및 암호가 포함된 데이터 출력   

** 1. header->caplen 구조체 필드정보를 이용하여 OS커널에서 사용자 모드로 복사된 패킷데이터 길이를 확인 가능
** 2. 54바이트(이더넷 헤더 + IP헤더 + TCP헤더) = 14바이트 + 20바이트 + 20바이트 까지는 프로토콜 헤더정보가 저장 
** 3. 55바이트 부터 계정 혹은 암호 값이 포함된 데이터 부분이다.
 
```c
for(i=55; (i < header->caplen + 1); i++){
	printf("%02x", pkt_data[i-1]);
}
	printf("\n\n");
for(i=55; (i < header->caplen + 1); i++){

	// ASCII 값으로 출력하기 위해 10진수 33에서 126까지 범위로 제한하였다.
	if((pkt_data[i-1] >= 33) && (pkt_data[i-1] <= 126)){
		printf(" %c", pkt_data[i-1]);
	}
	else{
		printf(" ");
	}
}
```   
