#include "pcap.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>

typedef struct mac_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
}mac_address;

#define ETHER_ADDR_LEN 6
struct ether_header{
	u_char ether_dhost[ETHER_ADDR_LEN];
	u_char ether_shost[ETHER_ADDR_LEN];
	u_short ether_type;
}eth;

typedef struct ip_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

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

typedef struct udp_header{
	u_short sport;	// 송신자 포트번호
	u_short dport;	// 수신자 포트번호
	u_short len;		// UDP 데이터 길이
	u_short crc;		// 체크섬 
}udp_header;

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){
	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	tcp_header *th;
	udp_header *uh;
	u_int ip_len;
	int i=0;
	time_t local_tv_sec;

	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	printf("=====EthernetPacket=====\n");
	printf("\n");
	printf("%s.%.6d \n", timestr, header->ts.tv_usec);
	printf("\n");
	
	mac_address *srcmac;
	mac_address *destmac;
	destmac = (mac_address *)pkt_data;
	srcmac = (mac_address *)(pkt_data+6);

	printf("%02x.%02x.%02x.%02x.%02x.%02x -> %02x.%02x.%02x.%02x.%02x.%02x\n",
			srcmac->byte1,
			srcmac->byte2,
			srcmac->byte3,
			srcmac->byte4,
			srcmac->byte5,
			srcmac->byte6,

			destmac->byte1,
			destmac->byte2,
			destmac->byte3,
			destmac->byte4,
			destmac->byte5,
			destmac->byte6);
	printf("\n");

	ih = (ip_header*)(pkt_data + 14);	
	ip_len = (ih->ver_ihl & 0xf) * 4;	// 헤더길이만을 저장하기 위해 0000ffff와 ver_ihl를 and시킨다.
	th = (tcp_header*)((u_char*)ih + ip_len);
	uh = (udp_header*)((u_char*)ih + ip_len);

	if(ih->proto ==6){
		printf("%d.%d.%d.%d (%d)-> %d.%d.%d.%d (%d), flag: %04x\n",
			ih->saddr.byte1,
			ih->saddr.byte2,
			ih->saddr.byte3,
			ih->saddr.byte4,
			ntohs(th->sport),

			ih->daddr.byte1,
			ih->daddr.byte2,
			ih->daddr.byte3,
			ih->daddr.byte4,
			ntohs(th->dport),
			th->flags);
		printf("\n");
		printf(" TCP Protocol\n");
		printf("\n");
		
		/*
			 header->caplen : 구조체 필드정보를 이용하여 os커널에서 사용자 모드로 복사된 
			 패킷데이터 길이를 확인할 수 있다.
			 55 : 55바이트부터가 계정 혹은 암호 값이 포함된 데이터 부분이다.
		*/
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
		printf("\n\n");
		printf("===== TheEnd =====\n");
	}
	else if(ih->proto == 11){
		printf("Upper Protocol is UDP\n");
		printf("\n");
		
		printf(" UDP Header \n\n\n");
		printf(" Destination port number : %d\n", ntohs(uh->dport));
		printf(" Source port number : %d\n", ntohs(uh->sport));
	}
	else if(ih->proto == 1){
		printf("Upper Protocol is ICMP\n");
		printf("\n");
	}
	else{
		printf("Upper Protocol is Unknown\n");
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
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	for(d=alldevs; d; d=d->next){
		printf("%d. %s", ++i, d->name);
		if(d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description avilable)\n");
	}
	if(i==0){
		printf("\nNo interfaces found ! Make sure WinPcap is installed.\n");
		return -1;
	}
	printf("Enter the interface number(1-%d):", i);
	scanf("%d", &inum);

	if(inum < 1 || inum > i){
		printf("\nAdapter number out of range.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	for(d=alldevs, i=0; i < inum-1; d=d->next, i++);

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