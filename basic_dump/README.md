## * WinPcap library를 이용한 basic_dump.c 구현   
## * 출처 : 네트워크 패킷 포렌식   

#### * 사용환경 : 우분투   
		* gcc basic_dump.c -o basic_dump -lpcap
#### * 목적 : 네트워크 상에 이더넷프레임 존재 시 카운팅과 함께 "이더넷 패킷이 존재한다는 메시지" 를 화면에 출력한다.   

* 주요 함수   
	* pcap_findalldevs() 함수   
		* pc에 존재하는 네트워크 디바이스 전체를 검색하여 리스트를 만드는 함수 
		* 성공 시 디바이스 정보 저장, 실패 시 -1 리턴
	* pcap_freealldevs() 함수   
		* 랜카드 정보 리스트를 해제하는 역할을 한다.   
	* pcap_open_live() 함수   
		* pcap_findalldevs() 함수를 통해 선택한 랜카드를 이용하여 패킷 수집 방법을 설정한다.   
		* 첫 번째 옵션 : 선택된 랜카드 사용된다.
		* 두 번째 옵션 : 선택된 랜카드로 수신할 패킷 길이를 정의한다.
		* 세 번째 옵션 : 모든 네트워크 패킷을 수집하는 옵션인 Promiscuous모드를 위해 1값으로 set   
		* 네 번째 옵션 : 여러 패킷을 한 번에 처리하기 위한 시간을 설정한다.
		* 다섯 번째 옵션 : 에러 발생 시 에러정보를 저장하는 공간 설정   

	* pcap_loop() 함수   
		* 선택된 랜카드로 패킷을 연속적으로 수집하는 역할을 한다.   
		* 첫 번째 인자 : pcap_open_live()를 통해 할당받은 랜카드 특성을 정의한 값을 사용   
		* 두 번째 인수 : 무한루프를 돌면서 패킷을 캡쳐   
		* 세 번째 함수 : 패킷 처리를 위한 핸들러를 정의하는 곳이다.
		* 마지막 인수 : 패킷 데이터 포인터를 정의하는데, 일반적으로 NULL 값을 사용한다.   
	* pcap_handler() 함수   
		* pcap_loop() 함수의 세 번째 함수로 사용된 값이다.
		* 수집된 패킷을 사용자가 정의하는 곳이다.   
		* u_char *param : pcap_dispatch()나 pcap_loop()를 호출할 때 전달하는 u_char 포인터 
		* const struct *pcap_pkthdr : 패킷의 정보를 가지고 있는구조체를 의미한다.
		* const u_char *pkt_data : 패킷의 데이터를 가리키고 있는 u_char 포인터다.   
	


```c
#include "pcap.h"


void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int main(){
	pcap_if_t *alldevs;
	pcap_if_t *d;

	int inum;
	int i=0;
	pcap_t *adhandle;

	char errbuf[PCAP_ERRBUF_SIZE];

	// pcap_findalldevs() 함수를 이용해서 랜 카드 검색 시 발생하는 에러처리를 위한 구간   
	// 에러발생 시 에러 출력 및 프로그램 종료 
	if(pcap_findalldevs(&alldevs, errbuf)== -1){
		fprintf(stderr, "Error in pcap_findalldevs: %s \n", errbuf);
		return -1;
	}
	
	// pcap_findalldevs() 함수로 확인된 랜카드 정보를 반복문을 통해 랜카드 종류 검색 및 출력
	// d->next : 랜카드 종류 검색
	// d->description : 출력 
	for(d=alldevs; d; d=d->next){
		printf("%d. %s", ++i, d->name);
		if(d->description)
			printf(" (%s) \n", d->description);
		else
			printf(" (No description available)\n");
	}

	if(i==0)
	{
		printf("\nNo interfaces found! Make sure Winpcap is installed.\n");
		return -1;
	}
	
	// 화면에 출력된 랜 카드 중 1개를 입력받는다.
	printf("selection nic card..(1-%d): ", i);
	scanf("%d", &inum);

	// 입력된 번호가 1보다 작거나 pc가 가진 랜카드 수보다 높을 경우 에러메시지 출력 및 종료 
	if(inum < 1 || inum > i){
		printf("\nInterface number out of range. \n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	// 선택된 번호가 pcap_open_live() 함수 옵션의 랜카드이름으로 사용된다.
	// NULL과 같은 에러발생 시 지원되지 않는 랜카드 메시지를 화면에 출력 후 프로그램 종료 
	for(d=alldevs, i=0; i<inum-1; d=d->next, i++);
	if((adhandle=pcap_open_live(d->name, 65536, 1, 1000, errbuf)) == NULL){
		fprintf(stderr, "\n %s isn't supported by winpcap \n", d->name);
		return -1;
	}
	
	// 선택한 랜카드 정보와 해당 랜카드로 listening한다는 메시지를 화면에 출력한다. 
	printf("\nlistening on %s...\n", d->description);

	pcap_freealldevs(alldevs);	// 수집된 모든 랜카드 정보를 해제한다.

	// 이 함수로 packet_handler() 함수를 호출하여 패킷 수집 
	pcap_loop(adhandle, 0, packet_handler, NULL); 

	// 프로그램 종료 시 이 함수 호출 	
	pcap_close(adhandle);
	return 0;
}

// 수집한 패킷을 사용자가 원하는 형태로 출력하도록 구성하는 사용자 정의공간이다. 
// 이더넷 패킷이 보일 때마다 카운트, 패킷 존재 여부 메시지를 화면에 출력한다.
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){
	int count=0;
	printf("%d : ethernet packet exists \n", ++count);
}
```