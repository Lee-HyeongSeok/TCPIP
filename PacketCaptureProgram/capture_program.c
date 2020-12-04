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
void packetFiltering(FILE *Filecapture, unsigned char *buffer, int dataSize);

void TCP_header_capture(FILE *Filecapture, unsigned char* buffer, int dataSize);
void UDP_header_capture(FILE *Filecapture, unsigned char *buffer, int Size);
void ICMP_header_capture(FILE *Filecapture, unsigned char *buffer, int Size);


void TCP_Header_print(FILE * Filecapture, struct tcphdr *tcpHeader, unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr *ipHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size);

void UDP_header_print(FILE * Filecapture, unsigned char *buffer, struct ethhdr * etherHeader, struct iphdr *ipHeader,
	struct udphdr * udpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size);

void ICMP_header_print(FILE * Filecapture, unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr * ipHeader, 
	struct icmphdr * icmpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size);

void Ethernet_header_print(FILE * Filecapture, struct ethhdr * etherHeader);
void IP_Header_print(FILE * Filecapture, struct iphdr * ipHeader, struct sockaddr_in src, struct sockaddr_in dst);
void change_hex_to_ascii(FILE * Filecapture, unsigned char *data, int Size);
void DNS_header_print(FILE *Filecapture, unsigned char *dnsHeader, int Size);

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

	// 파일 저장을 위한 파일 이름 변수 선언 
	char filename[40];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	// 파일 이름 지정
	// 로컬 시간을 파일 이름으로 지정한다. 
	sprintf(filename, "captureFile(%d-%d-%dT%d:%d:%d).txt", tm.tm_year-100, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	// 파일 포인터 지정
	// a+ : 이어서 쓰기 
	FILE *Filecapture = fopen(filename, "a+"); 
	if(Filecapture == NULL){
		printf("파일 열기 실패\n");
		exit(1);
	}

	while(1){
		if((dataSize = recvfrom(raw_sock, buffer, BUF_SIZE, 0, NULL, NULL)) == -1){
			printf("패킷 캡쳐 실패\n");
			continue;
		}
		// 패킷 캡쳐 성공
		packet_num++; // 받은 패킷 개수 카운팅 
		// 수집된 패킷을 필터링 한다.
		// 수집된 버퍼 자체를 넘김 
		packetFiltering(Filecapture, buffer, dataSize);
	}
	free(buffer);
	fclose(Filecapture);
}
// 수집된 ip 헤더를 필터링 한다. 
void packetFiltering(FILE * Filecapture, unsigned char *buffer, int dataSize){
	struct iphdr * ip_header = (struct iphdr*)(buffer + ETH_HLEN); // 받은 패킷의 ip 헤더 부분 얻어옴 
	
	switch(ip_header->protocol){
		// 수집한 패킷 헤더가 ICMP일 경우 
		case ICMP: 
			ICMP_header_capture(Filecapture, buffer, dataSize);
			break;
		// 수집한 패킷 헤더가 TCP일 경우 
		case TCP:
			TCP_header_capture(Filecapture, buffer, dataSize);
			break;
		// 수집한 패킷 헤더가 UDP일 경우 
		case UDP:
			UDP_header_capture(Filecapture, buffer, dataSize);
			break;
		default:
			break; // 그 외 패킷일 경우 무시 
	}
	
}

void Ethernet_header_print(FILE * Filecapture, struct ethhdr * etherHeader){
	fprintf(Filecapture, "\n-------------------------------------\n");
	fprintf(Filecapture, "-----------ethernet header-----------\n");

	// L3 패킷 타입
	// IPv4 : 0x0800, ARP : 0x0806, VLAN Tag : 0x8100
	fprintf(Filecapture, "Ethernet Type : 0x%02X00\n", etherHeader->h_proto);
	fprintf(Filecapture, "source MAC Addr : |");
	for(int i=0; i<6; i++){ // src mac 주소 출력 
		fprintf(Filecapture, "%02x ", etherHeader->h_source[i]); 
	}
	fprintf(Filecapture, "|");
	fprintf(Filecapture, "\n");
	fprintf(Filecapture, "dest Mac Addr : |");
	for(int i=0; i<6; i++){
		fprintf(Filecapture, "%02x ", etherHeader->h_dest[i]);
	}
	fprintf(Filecapture, "|\n");
}

void IP_Header_print(FILE * Filecapture, struct iphdr * ipHeader, struct sockaddr_in src, struct sockaddr_in dst){

	fprintf(Filecapture, "-------------------------------------\n");
	fprintf(Filecapture, "--------------IP header--------------\n\n");
	
	fprintf(Filecapture, "IP version : IPv%d\n", (unsigned int)ipHeader->version);
	fprintf(Filecapture, "IP Header Length : %d (%d Bytes)\n", (unsigned int)ipHeader->ihl, ((unsigned int)(ipHeader->ihl)) * 4);
	fprintf(Filecapture, "Type of Service : %d\n", (unsigned int)ipHeader->tos);
	fprintf(Filecapture, "IP total Length : %d Bytes\n", ntohs(ipHeader->tot_len));
	fprintf(Filecapture, "-------------------------------------\n");
	fprintf(Filecapture, "identification : %d\n", ntohs(ipHeader->id));
	fprintf(Filecapture, "-------------------------------------\n");
	fprintf(Filecapture, "Time To Live(TTL) : %d \n", (unsigned int)ipHeader->ttl);
	fprintf(Filecapture, "Protocol : %d\n", (unsigned int)ipHeader->protocol);
	fprintf(Filecapture, "Checksum : 0x%04X\n", ntohs(ipHeader->check));
	fprintf(Filecapture, "-------------------------------------\n");
	fprintf(Filecapture, "Source IP Address : %s\n", inet_ntoa(src.sin_addr));
	fprintf(Filecapture, "Dest IP Address : %s\n", inet_ntoa(dst.sin_addr));
	fprintf(Filecapture, "-------------------------------------\n");
}

void DNS_header_print(FILE *Filecapture, unsigned char *dnsHeader, int Size){
	char q = ' ';
	int idx=0;

	fprintf(Filecapture, "---------------------------------\n");
	fprintf(Filecapture, "-------------DNS Header----------\n");

	fprintf(Filecapture, " 0x");
	for(idx=0; idx<2; idx++){
		fprintf(Filecapture, "%02x", (unsigned char)dnsHeader[idx]);		
	} 

	// 질의, 응답 구별 
	int flags = (unsigned char)dnsHeader[idx];
	if(!(flags & 128)){
		q = '?';
	}
	idx += 5;
	
	int answerRR = (unsigned char)dnsHeader[idx];
	idx += 5;

	fprintf(Filecapture, " ");
	while(1){
		if(dnsHeader[idx] == 0)
			break;
		if(dnsHeader[idx] >= 32 && dnsHeader[idx] < 128)
			fprintf(Filecapture, "%c", (unsigned char)dnsHeader[idx]);
		else
			fprintf(Filecapture, ".");
		idx++;
	}
	idx += 2;
	
	int type = (unsigned char)dnsHeader[idx]; // 질의 타입 
	if(type == 1)
		fprintf(Filecapture, " A %c", q);
	else if(type == 28)
		fprintf(Filecapture, " AAAA %c", q);
	else if(type == 12)
		fprintf(Filecapture, "PTR %c", q);
	idx += 2;

	for(int i=0; i<answerRR; i++){
		while(1){
			if(dnsHeader[idx] == 0)
				break;
			idx++;
		}
		idx += 2;
		int type = (unsigned char)dnsHeader[idx];
		if(type == 1)
			fprintf(Filecapture, " A");
		else if(type == 28)
			fprintf(Filecapture, " AAAA ");
		else if(type == 12)
			fprintf(Filecapture, " PTR ");
		idx += 8;

		int length = (unsigned char)dnsHeader[idx];
		idx++;
		if(type == 1){
			for(int j=0; j<length; idx++, j++){
				int ip = (unsigned char)dnsHeader[idx];
				fprintf(Filecapture, "%d", ip);
				if(j != length-1)
					fprintf(Filecapture, ".");
			}
		}
		else if(type == 28){
			for(int j=0; j<length; idx++, j++){
				if((unsigned char)dnsHeader[idx] == 0)
					continue;
				fprintf(Filecapture, "%02X", (unsigned char)dnsHeader[idx]);
				if(j == 1 || j == 3 || j == 5)
					fprintf(Filecapture, ":");
				if(j == length-1)
					fprintf(Filecapture, "::%0x", (unsigned char)dnsHeader[idx]);
			}
		}
		else if(type == 12){
			for(int j=0; j<length; idx++, j++){
				if((unsigned char)dnsHeader[idx] == 0)
					continue;
				if(dnsHeader[idx] >= 32 && dnsHeader[idx] < 128)
					fprintf(Filecapture, "%c", (unsigned char)dnsHeader[idx]);
				else
					fprintf(Filecapture, ".");
			}
		}
		fprintf(Filecapture, " ");
	}
}

void TCP_header_capture(FILE *Filecapture, unsigned char *buffer, int dataSize){
	
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
	/*
	for(int i=0; i<(dataSize-ETH_HLEN); i++){
		//printf("\n%02X ", (buffer+ETH_HLEN)[i]); // 원 코드 
		//fprintf(stdout, "ethernet");
		change_hex_to_ascii(Filecapture, buffer, dataSize);

		//fprintf(stdout, "ip");
		change_hex_to_ascii(Filecapture, buffer+ETH_HLEN, (ip_header->ihl*4));

		//fprintf(stdout, "tcp");
		change_hex_to_ascii(Filecapture, buffer+ETH_HLEN+(ip_header->ihl*4), sizeof(tcp_header));

		//fprintf(stdout, "payload");
		change_hex_to_ascii(Filecapture, buffer+ETH_HLEN+(ip_header->ihl*4)+sizeof(tcp_header), (dataSize-sizeof(tcp_header)-(ip_header->ihl*4)-ETH_HLEN));
		
	}
	fprintf(Filecapture, "\n----------------------------------\n");
	*/

	TCP_Header_print(Filecapture, tcp_header, buffer, ether_header, ip_header, source, dest, dataSize);
}
void TCP_Header_print(FILE * Filecapture, struct tcphdr *tcpHeader, unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr *ipHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size){

	// ethernet header 출력
	Ethernet_header_print(Filecapture, etherHeader);
	
	// tcp header 출력 
	fprintf(Filecapture, "\n--------------------------------\n");
	fprintf(Filecapture, "-----------TCP header-----------\n");

	fprintf(Filecapture, "source port : %u\n", ntohs(tcpHeader->source));
	fprintf(Filecapture, "dest port : %u\n", ntohs(tcpHeader->dest));
	fprintf(Filecapture, "--------------------------------\n");
	fprintf(Filecapture, "Sequence Number : %u\n", ntohl(tcpHeader->seq));
	fprintf(Filecapture, "Acknowledge Number : %u\n", ntohl(tcpHeader->ack_seq));
	fprintf(Filecapture, "--------------------------------\n");
	fprintf(Filecapture, "OFFSET(Header Length) : %d DWORDS (%d Bytes)\n", (unsigned int)tcpHeader->doff,
		(unsigned int)tcpHeader->doff * 4);
	fprintf(Filecapture, "URG Flag : %d\n", (unsigned int)tcpHeader->urg);
	fprintf(Filecapture, "ACK Flag : %d\n", (unsigned int)tcpHeader->ack);
	fprintf(Filecapture, "PUSH Flag : %d\n", (unsigned int)tcpHeader->psh);
	fprintf(Filecapture, "RESET Flag : %d\n", (unsigned int)tcpHeader->rst);
	fprintf(Filecapture, "SYN Flag : %d\n", (unsigned int)tcpHeader->syn);
	fprintf(Filecapture, "FIN Flag : %d\n", (unsigned int)tcpHeader->fin);
	fprintf(Filecapture, "--------------------------------\n");
	fprintf(Filecapture, "Window size(rwnd) : %d\n", ntohs(tcpHeader->window));
	fprintf(Filecapture, "--------------------------------\n");
	fprintf(Filecapture, "Checksum : 0x%04x\n", ntohs(tcpHeader->check));
	fprintf(Filecapture, "Urgent Pointer : %d\n", tcpHeader->urg_ptr);
	fprintf(Filecapture, "--------------------------------\n");
	
	change_hex_to_ascii(Filecapture, buffer+ETH_HLEN+(ipHeader->ihl*4)+tcpHeader->doff*4, (Size-tcpHeader->doff*4-(ipHeader->ihl*4)-ETH_HLEN));
	fprintf(Filecapture, "--------------------------------\n");
}


void UDP_header_capture(FILE *Filecapture, unsigned char *buffer, int Size){
	struct ethhdr * etherHeader = (struct ethhdr*)buffer; // 버퍼에서 이더넷 정보 가져옴 
	struct iphdr * ipHeader = (struct iphdr*)(buffer + ETH_HLEN); // 버퍼에서 ip헤더 정보 가져옴 
	// 버퍼에서 udp 헤더 정보 가져옴 
	struct udphdr * udpHeader = (struct udphdr *)(buffer + ipHeader->ihl * 4 + ETH_HLEN);
	struct sockaddr_in src, dst;

	src.sin_addr.s_addr = ipHeader->saddr;
	dst.sin_addr.s_addr = ipHeader->daddr;

	// 출발지 포트가 dns면 질의 
	if(ntohs(udpHeader->source) == dns){
		fprintf(stdout, "%d %s : dns > \n", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr));
		fprintf(stdout, "%s:%u = UDP\n", inet_ntoa(dst.sin_addr), ntohs(udpHeader->dest));
		DNS_header_print(Filecapture, buffer+ETH_HLEN+(ipHeader->ihl*4)+sizeof(udpHeader), Size);
	}
	// 목적지 포트가 dns면 응답 
	else if(ntohs(udpHeader->dest) == dns){
		fprintf(stdout, "%d %s:%u > \n", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr), ntohs(udpHeader->source));
		fprintf(stdout, "%s:dns = UDP\n", inet_ntoa(dst.sin_addr));
		DNS_header_print(Filecapture, buffer+ETH_HLEN+(ipHeader->ihl*4)+sizeof(udpHeader), Size);
	}
	else{
		fprintf(stdout, "%d %s:%u > \n", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr), ntohs(udpHeader->source));
		fprintf(stdout, "%s:%u = UDP\n", inet_ntoa(dst.sin_addr), ntohs(udpHeader->dest));
	}
	fprintf(stdout, "length %d\n", Size);

	fprintf(stdout, "\n ethernet");
	change_hex_to_ascii(Filecapture, buffer, (ETH_HLEN));
	change_hex_to_ascii(Filecapture, buffer+ETH_HLEN, (ipHeader->ihl*4));
	change_hex_to_ascii(Filecapture, buffer+ETH_HLEN+(ipHeader->ihl*4), sizeof(udpHeader));
	change_hex_to_ascii(Filecapture, buffer+ETH_HLEN+(ipHeader->ihl*4)+sizeof(udpHeader), (Size-sizeof(udpHeader)-(ipHeader->ihl*4)-ETH_HLEN));

	UDP_header_print(Filecapture, buffer, etherHeader, ipHeader, udpHeader, src, dst, Size);
}

void UDP_header_print(FILE *Filecapture, unsigned char *buffer, struct ethhdr * etherHeader, struct iphdr *ipHeader,
	struct udphdr * udpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size){
	Ethernet_header_print(Filecapture, etherHeader);
	IP_Header_print(Filecapture, ipHeader, src, dst);
	fprintf(Filecapture, "\n--------------------------------------------------\n");
	fprintf(Filecapture, "---------------------UDP Header---------------------\n");
	fprintf(Filecapture, "source port %u\n", ntohs(udpHeader->source));
	fprintf(Filecapture, "destination port %u\n", ntohs(udpHeader->dest));
	fprintf(Filecapture, "----------------------------------------------------\n");
	fprintf(Filecapture, "UDP length %d\n", ntohs(udpHeader->len));
	fprintf(Filecapture, "UDP Checksum 0x%04x\n", ntohs(udpHeader->check));
	fprintf(Filecapture, "----------------------------------------------------\n");

	change_hex_to_ascii(Filecapture, buffer+(ipHeader->ihl*4)+sizeof(udpHeader), (Size-sizeof(udpHeader)-(ipHeader->ihl*4)-ETH_HLEN));
	fprintf(Filecapture, "\n----------------------------------------------------\n");
	
}

void ICMP_header_capture(FILE *Filecapture, unsigned char *buffer, int Size){

	struct ethhdr *etherHeader = (struct ethhdr*)buffer;
	struct iphdr *ipHeader = (struct iphdr*)(buffer + ETH_HLEN);
	struct icmphdr *icmpHeader = (struct icmphdr*)(buffer + ipHeader->ihl*4+ETH_HLEN);
	struct sockaddr_in src, dst;
	src.sin_addr.s_addr = ipHeader->saddr;
	dst.sin_addr.s_addr = ipHeader->daddr;

	fprintf(stdout, "IPv%d %s > ", (unsigned int)ipHeader->version, inet_ntoa(src.sin_addr));
	fprintf(stdout, "%s = ICMP [Type : %d / Code : %d] TTL = %d", inet_ntoa(dst.sin_addr), (unsigned int)icmpHeader->type, (unsigned int)icmpHeader->code, (unsigned int)ipHeader->ttl);
	fprintf(stdout, "length %d", Size);

	change_hex_to_ascii(Filecapture, buffer+ETH_HLEN, (Size-ETH_HLEN));
	ICMP_header_print(Filecapture, buffer, etherHeader, ipHeader, icmpHeader, src, dst, Size);
}


void ICMP_header_print(FILE * Filecapture, unsigned char *buffer, struct ethhdr *etherHeader, struct iphdr * ipHeader, 
	struct icmphdr * icmpHeader, struct sockaddr_in src, struct sockaddr_in dst, int Size){
	fprintf(Filecapture, "----------------------ICMP Packet----------------------\n");
	Ethernet_header_print(Filecapture, etherHeader);
	IP_Header_print(Filecapture, ipHeader, src, dst);
	fprintf(Filecapture, "-------------------------------------------------------\n");
	fprintf(Filecapture, "----------------------ICMP Header----------------------\n");
	fprintf(Filecapture, "-------------------------------------------------------\n");
	fprintf(Filecapture, "Type %d", (unsigned int)(icmpHeader->type));
	fprintf(Filecapture, "Code %d\n", (unsigned int)(icmpHeader->code));
	fprintf(Filecapture, "checksum 0x%04x\n", ntohs(icmpHeader->checksum));
	fprintf(Filecapture, "-------------------------------------------------------\n");
	change_hex_to_ascii(Filecapture, buffer+ETH_HLEN+(ipHeader->ihl*4)+sizeof(icmpHeader), (Size-sizeof(icmpHeader)-(ipHeader->ihl*4)-ETH_HLEN));
	fprintf(Filecapture, "-------------------------------------------------------\n");
}

void change_hex_to_ascii(FILE * Filecapture, unsigned char *data, int Size){
	fprintf(Filecapture, "\n\nDATA (payload)\n");
	for(int i=0; i<Size; i++){
		if(i != 0 && i % 16 == 0){ // 16개 데이터 출력, ascii 출력 이후 개행 
			fprintf(Filecapture, "\t\t"); // 16진수 데이터와 ascii 데이터 구분 
			for(int j=i-16; j<i; j++){ // 16진수 데이터를 ascii로 변환
				if(data[j] >= 32 && data[j] < 128)
					fprintf(Filecapture, "%c", (unsigned char)data[j]);
				else
					fprintf(Filecapture, ".");
			}
			fprintf(Filecapture, "\n");
		}
		if(i % 16 == 0)
			fprintf(Filecapture, "\t"); // 처음 오는 데이터 
		if(i == Size-1){
			for(int j=0; j<(15-(i%16)); j++)
				fprintf(Filecapture, "   ");
			fprintf(Filecapture, "\t\t");

			for(int j=(i-(i%16)); j<=i; j++){
				if(data[j] >= 32 && data[j] < 128)
					fprintf(Filecapture, "%c", (unsigned char)data[j]);
				else
					fprintf(Filecapture, ".");
			}
			fprintf(Filecapture, "\n");
		}
	}
}
void menu(){
	printf("\n----------------packet capture 프로그램----------------\n");
	printf("[ICMP 1] [TCP 6] [UDP 17]\n");
}