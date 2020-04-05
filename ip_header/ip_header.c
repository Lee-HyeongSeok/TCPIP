#include "pcap.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

typedef struct mac_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
}mac;

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

int main(){
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[]="";
	struct bpf_program fcode;

	if(pcap_findalldevs(&alldevs, errbuf) == -1){
		fprintf(stderr, "Error in pcap_findalldevs : %s \n", errbuf);
		exit(1);
	}
	for(d=alldevs; d; d=d->next){
		printf("%d. %s", ++i, d->name);
		if(d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}
	if(i==0){
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
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
		fprintf(stderr, "\nUnable to compile the packet filter. Check the Syntax. \n");
		return -1;
	}
	
	if(pcap_setfilter(adhandle, &fcode) < 0){
		fprintf(stderr, "\n Error setting the filter. \n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	printf("\nlistening on %s...\n", d->description);
	pcap_freealldevs(alldevs);
	pcap_loop(adhandle, 0, packet_handler, NULL);
	return 0;
}

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
		printf("ip version is %d\n", ih->ip_version);
		printf("\n");
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