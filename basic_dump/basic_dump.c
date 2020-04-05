#include "pcap.h"

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int main(){
	pcap_if_t *alldevs;
	pcap_if_t *d;

	int inum;
	int i=0;
	pcap_t *adhandle;

	char errbuf[PCAP_ERRBUF_SIZE];

	if(pcap_findalldevs(&alldevs, errbuf)== -1){
		fprintf(stderr, "Error in pcap_findalldevs: %s \n", errbuf);
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
		printf("\nNo interfaces found! Make sure Winpcap is installed.\n");
		return -1;
	}
	
	printf("selection nic card..(1-%d): ", i);
	scanf("%d", &inum);

	if(inum < 1 || inum > i){
		printf("\nInterface number out of range. \n");
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	for(d=alldevs, i=0; i<inum-1; d=d->next, i++);
	if((adhandle=pcap_open_live(d->name, 65536, 1, 1000, errbuf)) == NULL){
		fprintf(stderr, "\n %s isn't supported by winpcap \n", d->name);
		return -1;
	}
	printf("\nlistening on %s...\n", d->description);
	pcap_freealldevs(alldevs);
	pcap_loop(adhandle, 0, packet_handler, NULL);
	pcap_close(adhandle);
	return 0;
}

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){
	int count=0;
	printf("%d : ethernet packet exists \n", ++count);
}