## * 이더넷 헤더 출력 구현   
#### * 출처 : 네트워크 패킷 포렌식   
#### * 목적 : 이더넷프레임의 이더넷 헤더 조사   

#### * Mac 주소란?   
		* 랜카드에 할당된 고유한 번호를 의미한다.
		* 전체 길이 48비트가 8비트씩 6개로 묶인 구성이다.
		* 00-13-77-00-8D-7D
		* 00-13-77 : 제조회사를 식별하는 고유번호, IEEE에서 할당한다.
		* 00-8D-7D : 제조회사가 할당한 일련번호다.   

#### * 이더넷 헤더부분   
		* 목적지 MAC주소, ether_dhost[ETHER_ADDR_LEN] : 6바이트
		* 발신지 MAC주소, ether_shost[ETHER_ADDR_LEN] : 6바이트
		* 패킷 유형, ether_type : 2바이트   

#### * 수집된 패킷의 각 위치 파악   
		* *pkt_data는 수진자 MAC주소, +6만큼 더한 곳은 수신자 MAC주소의 끝이자 발신지 주소의 위치 
		* void packet_handler()함수의 3번 째 옵션인 const u_char *pkt_data   

#### * 주 구조체   
	* mac_address : mac 형태를 가지는 구조체를 선언, 해당 구조체를 이용하여 mac주소 출
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
	* ether_header : 이더넷 모형을 가지는 구조체를 eth라고 지칭  
	```c
	struct ether_header{
	u_char ether_dhost[ETHER_ADDR_LEN]; // 목적지 주소, 6바이트 
	u_char ether_shost[ETHER_ADDR_LEN]; // 발신지 주소, 6바이트 
	u_short ether_type;		// 패킷 유형, 2바이트 
	}eth;
	```
#### * 주요 함수     
		* main()
			* char packet_filter[] = "": tcp를 정의할 경우 tcp만 수집한다.
			* <필터링 옵션>
				* host foo : foo 호스트
				* net 128.3 : 네트워크 대역 128.3
				* port 20 : 포트 20번   
				* src foo : 발신지 foo
				* dst net 128.3 : 목적지 네트워크 대역 128.3
				* src or dst port ftp-data : 발신 혹은 목적지 포트 ftp-data(20번)
				* ether src foo : 이더넷 헤더 발신지 foo
				* arp net 128.3 : ARP 프로토콜 네트워크 대역 128.3
				* ip or idp : IP또는 UDP
				* tcp port 53 : tcp 포트 53번   

			* fcode : 특정 프로토콜만을 캡쳐하기 위한 정책정보가 저장된다.
		* int pcap_compile(pcap_t *p, struct bpf_program *fp, char *str, int optimize, bpf_u_int32 netmask);
			* pcap_t *p : 패킷 캡쳐 방식을 정의한 정보를 가지고 있다.
			* struct bpf_program *fp : 사용자가 입력한 필터링 룰이 저장되는 공간 
			* char *str : 사용자가 입력한 필터링 룰을 표현하는 변수정보
			* int optimize : 결과 코드를 수행할 때 최적화 여부를 의미한다. 값 1 사용 
			* bpf_u_int32 netmask : IP주소에 사용되는 넷마스크 값이다.

		* pcap_compile() 함수 
			* pcap_t *p : 패킷 캡쳐 방식을 정의한 정보가 사용된다.
			* struct bpf_program *fp : pcap_compile에서 전달된 필터링 정책 


#### void packet_handler() 

```c
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
```   

#### main()   
```c
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
```   
