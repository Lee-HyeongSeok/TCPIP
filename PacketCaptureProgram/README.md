## 컴퓨터네트워크 패킷캡쳐 프로그램 설계   
#### 출처 : 한국산업기술대학교   
#### 주제 : Wireshark를 모티브로 raw socket을 이용한 패킷 캡쳐 프로그램 설계   
***   

#### raw socket을 이용한 패킷 캡쳐 동작 구성   
		1. socket() 함수를 통해 raw socket 생성   
				* SOCK_RAW 옵션 사용   
				* htons(ETH_A_ALL) 옵션 사용   
		2. recvfrom() 함수를 통해 송신측으로부터 들어오는 패킷을 커널이 처리하기 전에 buffer에 저장   
				* 응용->전송->네트워크->링크->물리에서부터 물리->링크->네트워크->전송->응용 순으로 흐름   
				* 커널은 응용 계층과 전송계층 사이에 존재하여 수신데이터 전단에 위치한 헤더들을 조사하여 처리   
				* 응용 계층 측에서 패킷이 커널에 처리하기 전에 버퍼에 패킷 정보 저장   
				* SOCK_RAW 옵션을 사용했기 때문에 수신자 IP에 해당하는 패킷만 볼 수 있다.   		
		3. ip header에 저장된 destination port를 확인하여 tcp, udp, icmp로 필터링한다.   
		4. buffer에 저장된 데이터를 각 tcp, udp, icmp, ethernet header 길이를 통해서 데이터 전단에 할당된 헤더를 출력한다.   

#### raw socket 생성 시 사용하는 옵션(SOCK_RAW & SOCK_PACKET)   
		1. SOCK_RAW : 네트워크 계층으로 올라간 패킷들 중 수신자 IP가 자신의 IP에 해당하는 패킷만 볼 수 있다.   
		2. SOCK_PACKET : 데이터 링크 계층에서의 패킷인 시스템을 오가는 모든 패킷 확인 가능   
					* ethernet frame header   
					* MAC Address   
		
***   

[!설계 계획서](https://github.com/leehyeongseck/TCPIP/blob/master/PacketCaptureProgram/%ED%8C%A8%ED%82%B7%EC%BA%A1%EC%B3%90%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%A8_%EC%84%A4%EA%B3%84.pdf)
