#include "pcap.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define ETHER_ADDR_LEN 6

// *pkt_data : 시작위치부터 각 필드 크기만큼 더해주면 각각의 위치 값을 알아낼 수 있다. 
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

typedef struct mac_address{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;

}mac;

struct ether_header{
	u_char ether_dhost[ETHER_ADDR_LEN]; // 목적지 주소, 6바이트 
	u_char ether_shost[ETHER_ADDR_LEN]; // 발신지 주소, 6바이트 
	u_short ether_type;		// 패킷 유형, 2바이트 
}eth;

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

	if(pcap_findalldevs(&alldevs, errbuf)==-1){
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		return -1;
	}
	for(d=alldevs; d; d=d->next){
		printf("%d. %s", ++i, d->name);
		if(d->description)
			printf(" (%s) \n", d->description);
		else
			printf(" (No description available)\n");
	}
	
	if(i==0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}
	
	printf("nic 카드를 선택하세요..(1-%d): ", i);
	scanf("%d", &inum);

	if(inum < 1 || inum > i){
		printf("\nInterface number out of range.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	for(d=alldevs, i=0; i<inum-1; d=d->next, i++);
	if((adhandle=pcap_open_live(d->name, 65536, 1, 1000, errbuf)) == NULL){
		fprintf(stderr, "\n %s isn't supported by winpcap \n", d->name);
		pcap_freealldevs(alldevs);
		return -1;
	}
	if(pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0){
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	// pcap_compile() 함수 내용을 적용하기 위해 사용되는 setfilter 함수 
	// pcap_compile()을 통해 결정된 bpf_program 구조체를 적용하여 프로토콜을 필터링하기 위해 사용됨 
	if(pcap_setfilter(adhandle, &fcode) < 0){
		fprintf(stderr, "\nError setting the filter. \n");
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

	unsigned int ptype;

	mac* srcmac;
	mac* destmac;
	destmac = (mac*)pkt_data;
	srcmac = (mac*)(pkt_data+6);

	struct ether_header *eth;
	eth = (struct ether_header*)pkt_data;
	ptype = ntohs(eth->ether_type);
	
	printf("***********Ethernet Frame Header *****************\n");
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
		printf("Next Protocol is IP HEADER(%04x)\n", ptype);
	}
	else if(ntohs(eth->ether_type) == ARP_HEADER){
		printf("Next Protocol is ARP_HEADER(%04x)\n", ptype);
	}
	else if(ntohs(eth->ether_type) == REVERSE_ARP_HEADER){
		printf("Next Protocol is REVERSE ARP HEADER(%04x)\n", ptype);
	}
	else{
		printf("Next Protocol is Unknown(%04x)\n", ptype);
	}
	printf("\n");
	printf("**************************************************\n");
	printf("\n");
	printf("\n");
	printf("\n");
}