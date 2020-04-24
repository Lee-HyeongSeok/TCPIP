## * 우아한 종료 - TCP 기반의 Half close 기법   
#### * 사용환경 : 우분투 16ver   
#### * compile : gcc file_client/server.c -o fclient/fserver   
#### * 출처 : 윤성우 TCPIP   

#### * 기존의 close(), closesocket()   
			1. 소켓의 완전 소멸을 의미한다.
			2. 더 이상 입, 출력 불가   
			3. 일방적인 종료의 형태
			4. 상대 호스트가 보낸 데이터 송, 수신이 완료되지 않으면 문제 발생   

#### * 기존의 문제점을 보완하기 위한 Half-close 기법   
			1. Half-close : 송, 수신 버퍼를 닫는 시간 차이를 두는 것   
			2. 출력 스트림은 종료시켜도 무관하다.
			3. 상대방의 종료 의사는 모르므로 입력 스트림은 종료를 유보시킨다.
			4. shutdown(sock, howto) 함수를 통해 동작   

#### * shutdown 함수   
			* shutdown(int sock, int howto)
			* sock : 종료할 소켓 디스크립터
			* howto : 종료 방법에 대한 정보 전달
					* SHUT_RD : 입력 스트림 종료
					* SHUT_WR : 출력 스트림 종료
					* SHUT_RDWR : 입, 출력 스트림 종료   
			* 상대 호스트로 EOF(End Of File)가 전달된다.
			* 실제 전달되는 것이 아닌 연결이 끊김을 커널이 확인하고 상대 호스트에게 알려준다.

#### * 구조   
		* 목적 : 서버로부터 파일을 읽어들여 클라이언트 자신의 파일에 저장   
		* 클라이언트 접속 요청 -> 서버 승인
		* 서버에서는 자신이 가진 파일을 읽어들여 파일의 내용을 클라이언트에게 전송 / 클라이언트는 자신의 receive.dat에 저장
		* 모두 전송 후 서버는 shutdown(clnt_sd, SHUT_WR)로 출력 스트림 닫음 
				* clnt_sd 디스크립터에 해당하는 호스트에게 자신의 출력 스트림 종료를 알림 
		* 파일을 완전히 전송받은 클라이언트는 서버에 메시지를 전송 후 종료 
#### * 소스 

		* client.c

```c
	// 서버에 연결 요청 
	connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	
	// 서버로부터 파일을 읽어온다.
	// read() 함수로 30이 넘지 않도록 파일의 끝 까지 데이터를 읽어온다.
	while((read_cnt=read(sd, buf, BUF_SIZE ))!=0)
		fwrite((void*)buf, 1, read_cnt, fp); // 읽어옴과 동시에 파일에 써준다.
	
	puts("Received file data");
	
	// 모든 파일을 다 받은 후 서버에게 메시지 전송 
	write(sd, "Thank you", 10); // 서버에 문자열 전송 
```   

		* server.c
```c
bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	listen(serv_sd, 5); // 연결 요청 리스너 
	
	clnt_adr_sz=sizeof(clnt_adr);    

	// accept 함수로 클라이언트의 요청을 허락한다.
	// 파일디스크립터를 반환하여 클라이언트 소켓 파일 디스크립터에 복사한다.
	clnt_sd=accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
	while(1)
	{
		// fread를 통해 파일의 내용을 읽는다. -> 클라이언트로 받는 것 아님 
		// 내용을 버퍼에 입력 
		read_cnt=fread((void*)buf, 1, BUF_SIZE, fp);
		if(read_cnt<BUF_SIZE)
		{
			// 파일 내용을 가진 버퍼를 클라이언트에게 전송 
			write(clnt_sd, buf, read_cnt);
			break;
		}
		// 버퍼를 클라이언트에게 전송 
		write(clnt_sd, buf, BUF_SIZE);
	}
	// 서버의 데이터 전송은 끝난 상태 

	// shutdown 함수로 클라이언트 소켓에게 서버의 전송이 끝남을 알림 
	// 출력 스트림 닫음 
	// 소켓 재활용 불가 
	shutdown(clnt_sd, SHUT_WR);	
	read(clnt_sd, buf, BUF_SIZE); // 클라이언트로부터 버퍼 입력받는다.
	printf("Message from client: %s \n", buf);
```