## 컴퓨터네트워크 패킷캡쳐 프로그램 설계   
#### 출처 : 한국산업기술대학교   
#### 주제 : Wireshark 벤치마킹, raw socket을 이용한 패킷 캡쳐 프로그램 설계   

***

<br>

### :pushpin: raw socket을 이용한 패킷 캡쳐 동작 구성   

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

<br>



### :pushpin: raw socket 생성 시 사용하는 옵션(SOCK_RAW & SOCK_PACKET)   

		1. SOCK_RAW : 네트워크 계층으로 올라간 패킷들 중 수신자 IP가 자신의 IP에 해당하는 패킷만 볼 수 있다.   
		2. SOCK_PACKET : 데이터 링크 계층에서의 패킷인 시스템을 오가는 모든 패킷 확인 가능   
					* ethernet frame header   
					* MAC Address   

<br>



### :pushpin: 가상머신 네트워크 설정   

		1. 어댑터에 브릿지   
		2. 무작위 모드 : 모두 허용   
		3. sudo 명령어를 통해서 가상 머신 내부에서 사용하는 랜 카드 ipconfig [your LAN card] promisc 명령어로 무작위 모드 ON   
				* 무작위 모드를 실행시켜야 raw 소켓을 통해 패킷을 캡쳐 가능   
		4. 캡쳐는 윈도우와 윈도우 내부에 가상 머신의 리눅스 사이 패킷을 캡쳐 한다.   
				* 가상머신 리눅스는 어댑터에 브릿지를 통해서 하나의 독립적인 IP를 자동으로 부여받는다.   

<br>



### :pushpin: 패킷 캡쳐 내용   

		1. TCP : tcp를 통한 http, https 동작은 브라우저를 통해서 캡쳐되는지 확인   
		2. UDP : icmp, dns 같은 경우는 윈도우에서 nslookup, dig 명령어를 통해서 확인, 인터넷 연결 종료 상태일 때 캡쳐 프로그램 실행하면 icmp 패킷에 오류 내용 잡힘   

<br>

### :pushpin: 프로그램 실행

1. 캡처하려는 PC에서 자신의 네트워크 인터페이스 이름 확인
   - **ifconfig**
2. Normal Mode에서 Promiscuous Mode로 설정
   - **ifconfig [자신의 네트워크 인터페이스 이름] primisc** 
3. gcc Build 한 프로그램 실행
   - **프로젝트 git clone**
   - **make 명령어 수행**
   - **capture_program.exe 실행**

***

[패킷 캡쳐 프로그램 설계 계획서 pdf](https://github.com/leehyeongseck/TCPIP/blob/master/PacketCaptureProgram/%ED%8C%A8%ED%82%B7%EC%BA%A1%EC%B3%90%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%A8_%EC%84%A4%EA%B3%84.pdf)
