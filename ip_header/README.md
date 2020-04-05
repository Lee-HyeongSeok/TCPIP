## * IP헤더 출력구현   
#### * 출처 : 네트워크 패킷 포렌식   
#### * 사용 헤더   
		* pcap.h
		* unistd.h
		* arpa/inet.h
		* stdlib.h   

#### * 사용 환경   
#### 우분투 리눅스 (gcc ip_address.c -o ip_address -lpcap)   

## * 사용 구조체   

#### * typedef struct ip_header   
		* u_char ip_version:4 : IP 버전이 저장되어있으며, 현재는 버전 4가 사용된다.
		* u_char ip_leng:4 : 헤더 길이정보를 담고 있으며, 옵션 값에 따라 길이가 가변적일 수 있다.
		* u_char tos(type of service) : 서비스 종류를 나타낸다.
		* u_short tlen(total length) : 헤더와 데이터의 길이를 합한 값이며 최대 65,535 바이트이다.
		* u_short identification : 전송할 최대사이즈 초과하여 분할 시 어떤 패킷에 속한지 구분하는 고유번호 할당 
		* <u_short flags_fo> : 분할된 추가 패킷의 여부를 알려준다.
			* 플래그(3bit) + 플래그오프셋(13bit)
			* 플래그오프셋 : 수신지에서 재배열하는 과정에서 각 조각의 순서를 파악하는데 사용.
			* 0비트 : 예약 필드로, 무조건 0으로 세팅되어야 한다.
			* 1비트 : DF(Don't Fragment) 비트라고 하며, 파일의 분할 여부를 알린다.
			* 2비트 : MF(More Fragment) 비트라고 하며, 분할된 패킷의 여부를 알린다.
				* 0 세팅은 마지막 조각임을 알린다.
		* u_char ttl(Time to Live) : 패킷수명을 제한하기 위해 데이터그램이 통과하는 최대홉수를 지정한다.
		* u_char proto : IP헤더에 따라올 상위 프로토콜을 지정하는 것이다.(TCP, UDP, ICMP)
		* u_short crc : 헤더의 오류를 검증하기 위해 사용된다.
		* ip_address saddr(source address) : 발신지 IP
		* ip_address daddr(destination address) : 수신지 IP 
		* u_int op_pad(option) : 새로운 실험 혹은 헤더정보에 추가정보를 표시하기 위해 설계했다.   
		* 실제 IP헤더와 반대 순서를 가진 이유는 네트워크 패킷 전달 시 빅 엔디언 방식으로 전달되기 때문.   

```c
typedef struct ip_header{
	u_char ip_leng:4;
	u_char ip_version:4;
	u_char tos;	// 서비스 타입(type of service)
	u_short tlen;	// 전체 길이

	// 전송 최대 사이즈 초과 시 분할될 경우, 어떤 패킷에 속한지 구분하는 고유번호 할당  
	u_short identification;	

	u_short flags_fo;	// 플래그(3bits) + 플래그오프셋 (13bits)
	u_char ttl;		// TTL 값
	u_char proto;	// 상위프로토콜 
	u_short crc;		// 헤더 체크섬 
	ip_address saddr;	// 발신지 IP
	ip_address daddr;	// 수신지 IP 
	u_int op_pad;	// 옵션 + 패딩 
}ip_header;
```   

#### * typedef struct ip_address   
		* IP주소를 저장하기 위한 공간을 구조체로 선언했다.
		* 내부에는 IP주소 자리수를 위해 4개(byte1~byte4) 변수가 사용된다.   
		* ex) 192.68.1.1   
		* 총 24비트의 길이다.(4바이트)

```c
typedef struct ip_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

```   

#### * typedef struct mac_address
		* mac주소를 저장하기 위한 공간을 구조체로 선언했다.
		* 내부에는 mac주소 자리수를 위해 6개(byte1~byte6) 변수가 사용된다.
		* ex) 00-05-0D-f2-00-3D
		* 총 48비트의 길이.(6바이트)

```c
typedef struct mac_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
}mac;

```   

## * 주요 함수 (사용자 정의 함수)   

#### * void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)

		* <ih=(ip_header*)(pkt_data + 14)>
			* ih 구조체에 IP헤더정보를 저장한다.
			* 수신자 mac(6byte) + 송신자 mac(6byte) + 상위프로토콜(2byte) = 14byte
			* 이더넷 헤더가 끝나는 부분이자 IP헤더가 시작되는 부분이기 때문에 pkt_data+14를 가리키면 저장된 IP헤더를 추출해낼 수 있다.   
		* <ih->proto>   
			* 0x11(16진수) : UDP
			* 0x06(16진수) : TCP
			* 0x01(16진수) : ICMP

```c
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){
	#define IP_HEADER 0x0800
	#define ARP_HEADER 0x0806
	#define REVERSE_ARP_HEADER 0x0835
	
	#define ICMP 0x01
	#define TCP 0x06
	#define UDP 0x11

	unsigned int ptype;
	mac* srcmac;
	mac* destmac;
	destmac = (mac*)pkt_data;
	srcmac = (mac*)(pkt_data + 6);

	struct ether_header* eth;
	eth = (struct ether_header*)pkt_data;
	ptype = ntohs(eth->ether_type);
	
	ip_header *ih;
	ih = (ip_header*)(pkt_data + 14);

	printf("**************** Ethernet Frame Header *****************\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("Destination Mac Address : %02x.%02x.%02x.%02x.%02x.%02x \n", 
		destmac->byte1,
		destmac->byte2,
		destmac->byte3,
		destmac->byte4,
		destmac->byte5,
		destmac->byte6);
	printf("\n");
	printf("Source Mac Address : %02x.%02x.%02x.%02x.%02x.%02x \n",
		srcmac->byte1,
		srcmac->byte2,
		srcmac->byte3,
		srcmac->byte4,
		srcmac->byte5,
		srcmac->byte6);
	printf("\n");
	
	if(ntohs(eth->ether_type) == IP_HEADER){
		printf("Upper Protocol is IP HEADER(%04x)\n", ptype);
	}
	else if(ntohs(eth->ether_type) == ARP_HEADER){
		printf("Upper Protocol is ARP HEADER(%04x)\n", ptype);
	}
	else if(ntohs(eth->ether_type) == REVERSE_ARP_HEADER){
		printf("Upper Protocol is REVERSE HEADER(%04x)\n", ptype);
	}
	else{
		printf("Upper Protocol is Unknown(%04x)\n",ptype);
	}
	printf("\n");
	
	if(ntohs(eth->ether_type) == IP_HEADER){
		printf("********************* IP Header *********************\n");
		printf("\n");
		printf("\n");
		printf("ip version is %d\n", ih->ip_version); //ip버전 출력 
		printf("\n");

		// ip헤더길이 출력, 헤더길이는 32비트 형태이므로 4를 곱한다.(8*4bit = 32bit) 
		printf("ip lengh is %d\n", (ih->ip_leng)*4); 
		printf("\n");
		printf("Destination IP Address : %d.%d.%d.%d \n", 
			ih->daddr.byte1,
			ih->daddr.byte2,
			ih->daddr.byte3,
			ih->daddr.byte4);
		printf("\n");
		printf("Source IP Address : %d.%d.%d.%d \n",
			ih->saddr.byte1,
			ih->saddr.byte2,
			ih->saddr.byte3,	
			ih->saddr.byte4);
		printf("\n");
		
		// 상위 프로토콜정보를 출력한다. 
		// ex) ih->proto가 0x11(16진수)라면 UDP라는 문자열 출력 
		// 다음에 따라오는 프로토콜 정보를 얻는 방법 
		if(ih->proto == TCP){	// TCP : 0x06(16진수)
			printf("Upper Protocol is TCP\n");
			printf("\n");
		}
		else if(ih->proto == UDP){ // UDP : 0x11(16진수)
			printf("Upper Protocol is UDP\n");
			printf("\n");
		}
		else if(ih->proto == ICMP){ // ICMP : 0x01(16진수)
			printf("Upper Protocol is ICMP\n");
			printf("\n");
		}
		else{
			printf("Upper Protocol is Unknown\n");
			printf("\n");
		}
	}
	else{ // eth->ether_type이 IP_HEADER가 아니라면
		printf("************** NO IP Header **************\n");
		printf("\n");
		printf("\n");
	}
	printf("**************************************************\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
}
```