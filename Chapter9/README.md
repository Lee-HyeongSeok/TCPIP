## * Chapter9 소켓의 다양한 옵션, Time-Wait, Nagle 알고리즘   
#### * 출처 : 윤성우 TCP/IP   
#### * 사용 환경 : 우분투 16ver   

#### * 소켓의 옵션   
			- 소켓의 특성을 변경시킬 때 사용한다.
			- 소켓의 옵션은 계층별로 분류 된다.
					* IPPROTO_IP level : IP 프로토콜에 관련된 사항들, set/get 둘 다 가능 
								* IP_TOS
								* IP_TTL
								* IP_MULTICAST_TTL
								* IP_MULTICAST_LOOP
								* IP_MULTICAST_IF
					* IPPROTO_TCP level : TCP 프로토콜에 관련된 사항들, set/get 둘 다 가능
								* TCP_KEEPALIVE
								* TCP_NODELAY
								* TCP_MAXSEG 
					* SOL_SOCKET level : 소켓에 대한 가장 일반적인 옵션들 
								<set/get 둘 다 가능>
								* SO_SNDBUF
								* SO_RCVBUF
								* SO_REUSEADDR
								* SO_KEEPALIVE
								* SO_BROADCAST
								* SO_DONTROUTE
								* SO_OOBINLINE
								<get만 가능>
								* SO_ERROR
								* SO_TYPE

#### * int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen)
			- sock : 옵션 확인을 위한 소켓의 파일 디스크립터 전달 
			- level : 확인할 옵션의 프로토콜 레벨 전달
			- optname : 확인할 옵션의 이름 전달
			- optval : 확인 결과의 저장을 위한 버퍼의 주소 값 전달
			- optlen : optval로 전달된 주소 값의 버퍼 크기를 담고 있는 변수의 주소 값 전달 

#### * int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen)
			- sock : 옵션 변경을 위한 소켓의 파일 디스크립터 전달
			- level : 변경할 옵션의 프로토콜 레벨 전달
			- optname : 변경할 옵션의 이름 전달
			- optval : 변경할 옵션 정보를 저장할 버퍼의 주소 값 전달
			- optlen : optval로 전달된 옵션 정보의 바이트 단위 크기 전달

#### * set_buf.c에서 설정한 버퍼의 크기와 다른 크기가 나오는 이유   
			- 입출력 버퍼 둘 다 3K로 설정했지만 다르게 출력된다.
			- 커널이 여유 공간을 가지고 있는 만큼 자동적으로 입출력 버퍼를 할당해주기 때문이다.

```c
	// 출력 버퍼 : 3K, 입력 버퍼 : 3K로 설정 
	int snd_buf=1024*3, rcv_buf=1024*3;
```

```c
	// 입출력 버퍼의 크기를 위에서 저장한대로 설정한다.

	// setsockopt 메소드에 SO_RCVBUF 옵션을 전달하여 rcv_buf의 버퍼 크기를 설정하는 동작 
	state=setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));
	if(state)
		error_handling("setsockopt() error!");
	// setsockopt 메소드에 SO_SNDBUF 옵션을 전달하여 snd_buf의 버퍼 크기를 설정하는 동작 
	state=setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));
	if(state)
		error_handling("setsockopt() error!");
```
#### * Time-wait
			- TCP 소켓에서 연결의 종료를 목적으로 4-way-handshaking을 시도한다.
			- 이 과정에서 첫 번째 메시지를 전달하는 호스트 A는 Time-wait 상태를 거친다.
			- Time-wait 상태 동안에는 해당 소켓이 소멸되지 않기 때문에 할당 받은 포트를 재사용 불가 -> bind error의 원인이 된다.
			- A 호스트의 마지막 ACK가 전송 중 소멸되면 호스트 B는 FIN 메시지를 A의 ACK를 받을 때 까지 A에게 전송한다.
			- 주소 재할당을 통해서 이 문제를 해결한다.   

#### * 주소 재할당   
			- Time-wait 상태에 있는 포트도 재할당이 가능하도록 소켓 상태를 변경해야 한다.
			- 통상 서버 프로그램에 한하여 설정한다.
			- 서버가 비정상 종료되어 재실행 해도 소켓을 재사용 가능하기 때문에 bind error를 해결한다.   

```c
	// 주소 재할당 설정
	// option을 True로 설정
	// setsockopt 메소드에 SO_REUSEADDR 특성 값을 전달하여 주소를 재할당하는 것을 세팅 
	optlen=sizeof(option);
	option=TRUE;	
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, optlen);
```

#### * Nagle 알고리즘
			- 인터넷의 과도한 트래픽으로 인한 전송속도 저하를 차단하기 위해 디자인되었다.
			- 앞서 전송한 데이터에 대해 상대로부터 ACK 메시지를 받아야만 다음 전송이 이루어지는 알고리즘이다.
			
			* 특징
						- 목적이 분명한 경우가 아니라면 중단하지 않아야 한다.
						- 필요에 의해 ON, OFF
						- 소켓은 기본적으로 Nagle 알고리즘이 ON되어있다.
						- 전송 데이터가 많지 않을 경우 가능한 조금씩 여러번이 아닌 한번에 많이 보낸다는 원칙을 기반으로 한다.   

```c
	// Nagle 알고리즘의 중단(OFF)을 명령 
	// TCP_NODELAY 옵션을 전달하여 Nagle 알고리즘의 설정을 변경하는 동작이다.
	opt_val = 1; // opt_val을 1로 설정 
	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val)))
		error_handling("setsockopt() error");
	else
		printf("setsockopt: TCP_NODELAY : opt_val = %d \n", (int)opt_val);

	// 변경된 Nagle 알고리즘의 상태를 확인하는 코드 
	// getsockopt 메소드에 TCP_NODELAY 특성 값을 전달하여 Nagle 알고리즘의 on, off 상태를 확인한다.
	if(getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &opt_len))
		error_handling("setsockopt() error");
	else
		printf("getsockopt : TCP_NODELAY : opt_val = %d \n", (int)opt_val);
```

