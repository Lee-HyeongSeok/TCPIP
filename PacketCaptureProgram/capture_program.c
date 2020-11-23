#include <stdio.h>
#include <arpa/inet.h> // 네트워크 정보 변환
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <netinet/if_ether.h>


#define BUF_SIZE 65536

#define ICMP 1
#define TCP 6
#define UDP 17
#define dns 53
#define http 80

void menu();
void packetCapture(int raw_sock);
void packetFiltering(unsigned char *buffer, int dataSize);
void TCP_header_capture(unsigned char* buffer, int dataSize);
void TCP_Header_print(struct tcphdr *tcpHeader, unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr *ipHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size);
void UDP_header_capture(unsigned char *buffer, int Size);
void UDP_header_print(unsigned char *buffer, struct ethhdr * etherHeader, struct iphdr *ipHeader,
	struct udphdr * udpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size);
void ICMP_header_capture(unsigned char *buffer, int Size);
void ICMP_header_print(unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr * ipHeader, 
	struct icmphdr * icmpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size);
void Ethernet_header_print(struct ethhdr * etherHeader);
void IP_Header_print(struct iphdr * ipHeader, struct sockaddr_in src, struct sockaddr_in dst);
void change_hex_to_ascii(unsigned char *data, int Size);


int main(int argc, char *argv[]){
	int raw_sock;
	char msg[128];
	int str_len;
	int val=1;

	if((raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1){
		printf("socket 생성 실패\n");
		exit(1);
	}
	setsockopt(raw_sock, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val));

	// 메뉴 
	menu();

	// raw_socket 생성 성공, 캡쳐 시작
	packetCapture(raw_sock); // 패킷 캡쳐 함수 

	printf("************************* 패킷 캡쳐 완료 *************************\n");
	close(raw_sock);
	return 0;
}

// raw 소켓을 통해서 icmp, tcp, udp 패킷을 수집한다.
void packetCapture(int raw_sock){
	unsigned char *buffer = (unsigned char*)malloc(BUF_SIZE);
	int dataSize=0;
	int packet_num=0;

	while(1){
		if((dataSize = recvfrom(raw_sock, buffer, BUF_SIZE, 0, NULL, NULL)) == -1){
			printf("패킷 캡쳐 실패\n");
			continue;
		}
		// 패킷 캡쳐 성공
		packet_num++; // 받은 패킷 개수 카운팅 
		// 수집된 패킷을 필터링 한다.
		// 수집된 버퍼 자체를 넘김 
		packetFiltering(buffer, dataSize);
	}
	free(buffer);
}
// 수집된 ip 헤더를 필터링 한다. 
void packetFiltering(unsigned char *buffer, int dataSize){
	struct iphdr * ip_header = (struct iphdr*)(buffer + ETH_HLEN); // 받은 패킷의 ip 헤더 부분 얻어옴 
	
	
	switch(ip_header->protocol){
		// 수집한 패킷 헤더가 ICMP일 경우 
		case ICMP: 
			ICMP_header_capture(buffer, dataSize);
			break;
		// 수집한 패킷 헤더가 TCP일 경우 
		case TCP:
			TCP_header_capture(buffer, dataSize);
			break;
		// 수집한 패킷 헤더가 UDP일 경우 
		case UDP:
			UDP_header_capture(buffer, dataSize);
			break;
		default:
			break; // 그 외 패킷일 경우 무시 
	}
	
}

void TCP_header_capture(unsigned char *buffer, int dataSize){
	struct ethhdr *ether_header = (struct ethhdr*)buffer; // 버퍼에서 이더넷 헤더를 가져온다.
	struct iphdr * ip_header = (struct iphdr*)(buffer + ETH_HLEN); // 버퍼에서 ip 헤더를 가져온다.
	// 버퍼에서 tcp 헤더 정보를 가져온다.
	struct tcphdr * tcp_header = (struct tcphdr*)(buffer + (ip_header->ihl * 4)+ETH_HLEN); 

	struct sockaddr_in source, dest; // 출발, 목적지 ip 저장할 변수 

	source.sin_addr.s_addr = ip_header->saddr;
	dest.sin_addr.s_addr = ip_header->daddr;

	// payload 출력 
	// ip header + tcp header 
	// Hex로 출력 
	for(int i=0; i<(dataSize-ETH_HLEN); i++){
		printf("%02X ", (buffer+ETH_HLEN)[i]);
	}
	printf("-------------------------------\n");
	
	TCP_Header_print(tcp_header, buffer, ether_header, ip_header, source, dest, dataSize);
}

void UDP_header_capture(unsigned char *buffer, int Size){
	struct ethhdr * etherHeader = (struct ethhdr*)buffer; // 버퍼에서 이더넷 정보 가져옴 
	struct iphdr * ipHeader = (struct iphdr*)(buffer + ETH_HLEN); // 버퍼에서 ip헤더 정보 가져옴 
	// 버퍼에서 udp 헤더 정보 가져옴 
	struct udphdr * udpHeader = (struct udphdr *)(buffer + ipHeader->ihl * 4 + ETH_HLEN);
	struct sockaddr_in src, dst;

	src.sin_addr.s_addr = ipHeader->saddr;
	dst.sin_addr.s_addr = ipHeader->daddr;

	if(ntohs(udpHeader->source) == dns){
		printf("%d %s : dns > \n", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr));
		printf("%s:%u = UDP\n", inet_ntoa(dst.sin_addr), ntohs(udpHeader->dest));
	}
	else if(ntohs(udpHeader->dest) == dns){
		printf("%d %s:%u > \n", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr), ntohs(udpHeader->source));
		printf("%s:dns = UDP\n", inet_ntoa(dst.sin_addr));
	}
	else{
		printf("%d %s:%u > \n", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr), ntohs(udpHeader->source));
		printf("%s:%u = UDP\n", inet_ntoa(dst.sin_addr), ntohs(udpHeader->dest));
	}
	printf("length %d\n", Size);

	change_hex_to_ascii(buffer+ETH_HLEN, (Size-ETH_HLEN));
}

void UDP_header_print(unsigned char *buffer, struct ethhdr * etherHeader, struct iphdr *ipHeader,
	struct udphdr * udpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size){
	Ethernet_header_print(etherHeader);
	IP_Header_print(ipHeader, src, dst);
	printf("\n--------------------------------------------------\n");
	printf("---------------------UDP Header---------------------\n");
	printf("source port %u\n", ntohs(udpHeader->source));
	printf("destination port %u\n", ntohs(udpHeader->dest));
	printf("----------------------------------------------------\n");
	printf("UDP length %d\n", ntohs(udpHeader->len));
	printf("UDP Checksum 0x%04x\n", ntohs(udpHeader->check));
	printf("----------------------------------------------------\n");

	change_hex_to_ascii(buffer+(ipHeader->ihl*4)+sizeof(udpHeader), (Size-sizeof(udpHeader)-(ipHeader->ihl*4)-ETH_HLEN));
	printf("----------------------------------------------------\n");
	
}

void ICMP_header_capture(unsigned char *buffer, int Size){
	struct ethhdr *etherHeader = (struct ethhdr*)buffer;
	struct iphdr *ipHeader = (struct iphdr*)(buffer + ETH_HLEN);
	struct icmphdr *icmpHeader = (struct icmphdr*)(buffer + ipHeader->ihl*4+ETH_HLEN);
	struct sockaddr_in src, dst;
	src.sin_addr.s_addr = ipHeader->saddr;
	dst.sin_addr.s_addr = ipHeader->daddr;

	printf("IPv%d %s > ", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr));
	printf("%s = ICMP ", inet_ntoa(dst.sin_addr));
	printf("length %d", Size);

	change_hex_to_ascii(buffer+ETH_HLEN, (Size-ETH_HLEN));
		
}

void ICMP_header_print(unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr * ipHeader, 
	struct icmphdr * icmpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size){
	printf("----------------------ICMP Packet----------------------\n");
	Ethernet_header_print(etherHeader);
	IP_Header_print(ipHeader, src, dst);
	printf("-------------------------------------------------------\n");
	printf("----------------------ICMP Header----------------------\n");
	printf("-------------------------------------------------------\n");
	printf("Type %d", (unsigned int)(icmpHeader->type));
	printf("Code %d\n", (unsigned int)(icmpHeader->code));
	printf("checksum 0x%04x\n", ntohs(icmpHeader->checksum));
	printf("-------------------------------------------------------\n");
	change_hex_to_ascii(buffer+ETH_HLEN+(ipHeader->ihl*4)+sizeof(icmpHeader), (Size-sizeof(icmpHeader)-(ipHeader->ihl*4)-ETH_HLEN));
	printf("-------------------------------------------------------\n");
}
void Ethernet_header_print(struct ethhdr * etherHeader){

	printf("-----------ethernet header-----------\n");
	// L3 패킷 타입
	// IPv4 : 0x0800, ARP : 0x0806, VLAN Tag : 0x8100
	printf("Ethernet Type : 0x%02X00\n", etherHeader->h_proto);
	for(int i=0; i<6; i++){ // src mac 주소 출력 
		printf("source Mac Addr : | %02x ", etherHeader->h_source[i]); 
	}
	printf("\n");
	for(int i=0; i<6; i++){
		printf("dest Mac Addr : | %02x ", etherHeader->h_dest[i]);
	}
}

void IP_Header_print(struct iphdr * ipHeader, struct sockaddr_in src, struct sockaddr_in dst){

	printf("-------------------------------------\n\n");
	printf("-----------IP header-----------\n");
	printf("-------------------------------\n");
	printf("IP version : IPv%d\n", (unsigned int)ipHeader->version);
	printf("IP Header Length : %d (%d Bytes)\n", (unsigned int)ipHeader->ihl, ((unsigned int)(ipHeader->ihl)) * 4);
	printf("Type of Service : %d\n", (unsigned int)ipHeader->tos);
	printf("IP total Length : %d Bytes\n", ntohs(ipHeader->tot_len));
	printf("-------------------------------\n");
	printf("identification : %d\n", ntohs(ipHeader->id));
	printf("-------------------------------\n");
	printf("Time To Live(TTL) : %d \n", (unsigned int)ipHeader->ttl);
	printf("Protocol : %d\n", (unsigned int)ipHeader->protocol);
	printf("Checksum : 0x%04X\n", ntohs(ipHeader->check));
	printf("-------------------------------\n");
	printf("Source IP Address : %s\n", inet_ntoa(src.sin_addr));
	printf("Dest IP Address : %s\n", inet_ntoa(dst.sin_addr));
	printf("-------------------------------\n");
}

void TCP_Header_print(struct tcphdr *tcpHeader, unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr *ipHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size){

	// ethernet header 출력
	Ethernet_header_print(etherHeader);
	
	// tcp header 출력 
	printf("-----------TCP header-----------\n");
	printf("source port : %u\n", ntohs(tcpHeader->source));
	printf("dest port : %u\n", ntohs(tcpHeader->dest));
	printf("--------------------------------\n");
	printf("Sequence Number : %u\n", ntohl(tcpHeader->seq));
	printf("Acknowledge Number : %u\n", ntohl(tcpHeader->ack_seq));
	printf("--------------------------------\n");
	printf("OFFSET(Header Length) : %d DWORDS (%d Bytes)\n", (unsigned int)tcpHeader->doff,
		(unsigned int)tcpHeader->doff * 4);
	printf("URG Flag : %d\n", (unsigned int)tcpHeader->urg);
	printf("ACK Flag : %d\n", (unsigned int)tcpHeader->ack);
	printf("PUSH Flag : %d\n", (unsigned int)tcpHeader->psh);
	printf("RESET Flag : %d\n", (unsigned int)tcpHeader->rst);
	printf("SYN Flag : %d\n", (unsigned int)tcpHeader->syn);
	printf("FIN Flag : %d\n", (unsigned int)tcpHeader->fin);
	printf("--------------------------------\n");
	printf("Window size(rwnd) : %d\n", ntohs(tcpHeader->window));
	printf("--------------------------------\n");
	printf("Checksum : 0x%04x\n", ntohs(tcpHeader->check));
	printf("Urgent Pointer : %d\n", tcpHeader->urg_ptr);
	printf("--------------------------------\n");
	
	change_hex_to_ascii(buffer+ETH_HLEN+(ipHeader->ihl*4)+tcpHeader->doff*4, (Size-tcpHeader->doff*4-(ipHeader->ihl*4)-ETH_HLEN));
	printf("--------------------------------\n");
}

void change_hex_to_ascii(unsigned char *data, int Size){
	for(int i=0; i<Size; i++){
		if(data[i] >= 32 && data[i] < 128)
			printf("%c", (unsigned char)data[i]); // data가 ascii라면 출력 
		else if(data[i] == 13) // 캐리지 리턴 
			continue;
		else if(data[i] == 10)
			printf("\n"); // \n라면 개행 출력 
		else
			printf("."); // 그 외 데이터는 .으로 출력 
	}
}
void menu(){
	printf("----------------packet capture 프로그램----------------\n");
	printf("[ICMP 1] [TCP 6] [UDP 17]\n");
}