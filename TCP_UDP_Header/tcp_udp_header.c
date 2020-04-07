#include "pcap.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

// ip주소를 저장할 구조체를 정의 
typedef struct ip_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

// 이더넷 정보를 저장할 구조체를 정의 
#define ETHER_ADDR_LEN 6
struct ether_header{
	u_char ether_dhost[ETHER_ADDR_LEN];
	u_char ether_shost[ETHER_ADDR_LEN];
	u_short ether_type;
}eth;

// ip헤더를 위한 구조체를 정의 
typedef struct ip_header{
	u_char ver_ihl; // version(4비트) + 인터넷 헤더 길이(4비트)
	u_char tos;	// 서비스 타입 
	u_short tlen; // 총 길이 
	u_short identification; // 고유 식별번호
	u_short flags_fo; // flags(3비트) + Fragment offset(13bit)
	u_char ttl; // Time To live(패킷 수명)
	u_char proto; // 프로토콜
	u_short crc; // 헤더 체크섬 비트 
	ip_address saddr; // 송신지 주소
	ip_address daddr; // 목적지 주소 
	u_int op_pad; // option+padding 
}ip_header;

// tcp 헤더를 저장할 구조체를 정의 
typedef struct tcp_header{
	u_short sport;	// 전송지 포트(16비트)
	u_short dport;	// 목적지 포트 (16비트)
	u_int seqnum;	// 데이터 순서를 나타내는 번호(32비트)
	u_int acknum;	// 다음에 수신할 데이터 번호(32비트) 
	u_char th_off;	// offset, tcp 헤더 길이 제공
	u_char flags;	// 통신 상태 제공 플래그(6개 비트로 구성) 
	u_short win;		
	u_short crc;		// tcp 체크섬 비트
	u_short urgptr;	// urg : 긴급 요청 
}tcp_header;

// UDP 헤더를 저장할 구조체 정의 
typedef struct udp_header{
	u_short sport;	// 송신자 포트번호
	u_short dport;	// 수신자 포트번호
	u_short len;		// UDP 데이터길이
	u_short crc;		// 체크섬 
}udp_header;

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){
	#define IP_HEADER 0x0800
	#define ARP_HEADER 0x0806
	#define REVERSE_ARP_HEADER 0x0835
	
	#define ICMP 0x01
	#define TCP 0x06
	#define UDP 0x11
	
	unsigned int ptype;
	struct ether_header *eth;
	eth = (struct ether_header*)pkt_data;
	ptype = ntohs(eth->ether_type);

	ip_header *ih;
	u_int ip_len;
	ih = (ip_header*)(pkt_data+14);

	// version과 헤더 길이 필드를 ver_ihl이라는 필드이름으로 합쳐서 사용하는 이유
	// -> 버전정보 필드는 제거하고, 헤더길이 필드만을 추출하기 위한 방식이다.
	// ih->ver_ihl에 0xf(00001111)을 AND 조건으로 계산하면 앞의 4자리 비트는 0으로 세팅되고, 나머지 4자리만 남는다.
	ip_len = (ih->ver_ihl & 0xf) * 4;
	
	if(ntohs(eth->ether_type) == IP_HEADER){
		printf("Upper Protocol is IP HEADER(%04x)\n", ptype);
	

		printf("*********************IP Header******************\n");
		printf("\n");
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

		if(ih->proto == 6){
			printf("Upper Protocol is TCP\n");
			printf("\n");
			
			// TCP헤더정보를 저장할 공간을 th로 선언하고 있다.
			tcp_header *th;

			// ih에 IP헤더길이인 ip_len길이를 더하면 TCP헤더 시작위치가 된다.
			th = (tcp_header *)((u_char*)ih+ip_len);
			
			// 플래그 비트조합 정의 
			#define SYN 0x02
			#define PUSH 0x08
			#define ACK 0x10
			#define SYN_ACK 0x12 // SYN_ACK(0x12) = SYN(0x02)+ACK(0x10)
			#define PUSH_ACK 0x18 // PUSH_ACK(0x18) = PUSH(0x08)+ACK(0x10)
			#define FIN_ACK 0x11  // FIN_ACK(0x11) = FIN(0x01)+ACK(0x10)

			printf("*********************TCP Header***********************\n");
			printf("\n");
			printf("\n");
			printf("Destination port number : %d \n", ntohs(th->dport));
			printf("Source port number : %d \n", ntohs(th->sport));
	
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
		}
		else if(ih->proto == 11){
			printf("Upper Protocol is UDP\n");
			printf("\n");
			
			udp_header *uh;

			// ih(IP헤더의 시작위치)에 ip_len(IP헤더길이)를 더하면 UDP헤더 시작위치를 알 수 있다.
			uh = (udp_header*)((u_char*)ih+ip_len);

			printf("********************UDP Header************************\n");
			printf("\n");
			printf("\n");
		
			printf("Destination port number : %d\n", ntohs(uh->dport));
			printf("Source port number : %d\n", ntohs(uh->sport));
		}
		else if(ih->proto == 1){
			printf("Upper Protocol is ICMP\n");
			printf("\n");
		}
		else{
			printf("Upper Protocol is Unknown\n");
			printf("\n");
		}
	}
	else{
		printf("*******************NO IP Header***********************\n");
		printf("\n");
		printf("\n");
	}
}

int main(){
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "";
	struct bpf_program fcode;
	
	if(pcap_findalldevs(&alldevs, errbuf) == -1){
		fprintf(stderr, "Error in pcap_findalldevs : %s \n", errbuf);
		exit(1);
	}
	for(d=alldevs; d; d=d->next){
		printf("%d. %s", ++i, d->name);
		if(d->description){
			printf(" (%s)\n", d->description);
		}
		else{
			printf(" (No Descriptoin available)\n");
		}
	}
	if(i == 0){
		printf("\nNo interfaces found! Make sure WinPcap is installed. \n");
		return -1;
	}
	printf("Enter the interface number (1-%d):", i);
	scanf("%d", &inum);
	
	if(inum < 1 || inum > i){
		printf("\nAdapter number out of range.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	for(d=alldevs, i=0; i<inum-1; d=d->next, i++);
	
	if((adhandle=pcap_open_live(d->name, 65536, 1, 1000, errbuf)) == NULL){
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		pcap_freealldevs(alldevs);
		return -1;
	}
	if(pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0){
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	if(pcap_setfilter(adhandle, &fcode) < 0){
		fprintf(stderr, "\nError setting the filter.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	printf("\nlistening on %s...\n", d->description);
	pcap_freealldevs(alldevs);
	pcap_loop(adhandle, 0, packet_handler, NULL);

	return 0;
}