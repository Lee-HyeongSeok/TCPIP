## * 기존의 echo_client의 문제와 계산기 프로그램 구현(어플리케이션 프로토콜)   
#### * 사용환경 : 우분투 16ver
#### * 출처 : 윤성우 TCPIP   

#### * 기존의 echo_client의 문제점
			* 서버는 받은 데이터를 그대로 다시 전송하기 때문에 문제가 없다.	
			* 기존의 echo_client는 전송하는 데이터 길이와 서버로부터 받는 데이터가 다를 수 있다.
			* 전송 시 100바이트였다면, 단편화로 인해 받을 때 10x10바이트로 나누어져 전송될 수 있다.   

#### * 기존의 echo_client를 보완한 코드 
			* 이를 해결하기 위해 데이터를 바로 받지 않는다.
			* recv_len을 통해 서버로부터 받은 데이터의 길이를 모두 더한다.
			* 마지막에 문자열의 끝을 의미하는 0을 넣어줌으로서 모든 데이터의 길이를 받았다는 표시를 해준다.
			* 이 과정을 통해 전송받은 데이터를 출력한다.
```c
while(1) 
{
	fputs("Input message(Q to quit): ", stdout);
	fgets(message, BUF_SIZE, stdin);
		
	if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
		break;
		
	// 데이터 전송의 단편화를 보완하기 위한 코드 
	// str_len : 보낸 데이터의 길이 
	str_len=write(sock, message, strlen(message));
		
	recv_len=0;	// 받고자 하는 데이터의 길이를 계속 더해간다는 뜻이다.
	while(recv_len<str_len) // 받은 데이터가 보낸 데이터보다 작으면 
	{
		recv_cnt=read(sock, &message[recv_len], BUF_SIZE-1);
		if(recv_cnt==-1) // 상대방이 통신을 끊었을 때 
			error_handling("read() error!");
		recv_len+=recv_cnt; // 서버로 부터 받은 길이를 더한다.
	}
		
	message[recv_len]=0; // 문자열의 끝을 나타내는 0을 표시해준다.
	printf("Message from server: %s", message); // 보낸 데이터 출력 
}

```   


#### * 계산기 프로그램 구현(어플리케이션 프로토콜)
			* 서버는 클라이언트로부터 여러 개의 숫자와 연산자 정보를 전달받는다.
			* 서버는 전달받은 숫자를 바탕으로 사칙연산한 결과를 클라이언트에게 전달한다.
			* ex) client: 3, 5, 9, +     server: 3+5+9의 연산결과 전달 
			* 피연산자의 개수정보를 1바이트 정수형태로 전달
			* 정수 하나는 4바이트로 표현
			* 연산 정보는 1바이트로 전달
			* 문자 +, -, * 중 하나를 선택하여 전달
			* 연산결과는 4바이트의 정수의 형태로 클라이언트에게 전달 
			* 연산결과를 얻은 클라이언트는 서버와 연결 종료 

#### * op_server.c 주요 코드 

```c
// 5명의 클라이언트 순차 접속과 처리 가능(Iterative)
for(i=0; i<5; i++)
{
	opnd_cnt=0; // 피연산자의 개수를 받는 변수 
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	// 클라이언트 소켓으로 부터 피연산자의 개수를 1바이트 읽어들인다.	
	read(clnt_sock, &opnd_cnt, 1);
		
	recv_len=0; // 수신 길이를 0부터 늘려간다. 

	// OPSZ : 2바이트 
	// recv_len이 계속 늘어나면서 클라이언트로부터 전송된 데이터를 전송받는다.
	// OPSZ뒤에 +1은 연산 기호를 말한다.
	while((opnd_cnt*OPSZ+1)>recv_len)
	{
		// opinfo[recv_len] 버퍼에서부터 누적해서 쌓아간다.
		recv_cnt=read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1); 
		recv_len+=recv_cnt;	//받아야할 데이터를 계속 더한다.
	} // 데이터를 다 받았을 때 
		
	// opnd_cnt : 피연산자 개수 
	// (int*)opinfo : 버퍼들의 위치 
	// opinfo[recv_len-1] : 연산자(+, -, *)
	result=calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);
	
	// 클라이언트에게 전송한다.
	// (char*) : write함수에는 버퍼가 char*형이어야 하기 때문에 강제 형변환을 실행한다.
	// int형 result를 char*형으로 형변환한다.
	write(clnt_sock, (char*)&result, sizeof(result));
	close(clnt_sock); // 클라이언트 해제 
}

// 피연산자 개수,  피연산자 배열, 연산자 
int calculate(int opnum, int opnds[], char op)
{
	int result=opnds[0], i;
	
	// opnum보다 작을 때 지정된 연산 수행 
	switch(op)
	{
	case '+':
		for(i=1; i<opnum; i++) result+=opnds[i];
		break;
	case '-':
		for(i=1; i<opnum; i++) result-=opnds[i];
		break;
	case '*':
		for(i=1; i<opnum; i++) result*=opnds[i];
		break;
	}
	return result; // 결과 반환 
}
```   

#### * op_client.c 주요 코드   

```c
fputs("Operand count: ", stdout);
scanf("%d", &opnd_cnt); // 피연산자가 몇개인지 입력받는다.
opmsg[0]=(char)opnd_cnt; // 전송하기 위해 1바이트로 형변환 
	
for(i=0; i<opnd_cnt; i++) // 전송 데이터 형식을 채우는 코드 
{
	printf("Operand %d: ", i+1); 
	// OPSZ뒤에 +1 : 피연산자의 개수 입력받은 뒤부터 채운다는 의미이다.
	scanf("%d", (int*)&opmsg[i*OPSZ+1]); // 피연산자를 입력받는다.
}
// 앞에서 입력했던 내용 중 입력한 피연산자가 아닌 다른 버퍼는 가져오지 않는다.
// fgetc : 스트림으로부터 하나의 문자만 가져온다.
fgetc(stdin);

// 연산자를 입력받는다.
fputs("Operator: ", stdout);

// 3개를 받았을 때 3*4+1 = 13위치에 입력한 연산자가 저장된다.
scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);

// 서버소켓으로 write한다.
// opnd_OPSZ : 피연산자들 
// +2 : 피연산자 개수, 맨 뒤에 연산자를 보내기 위한 길이이다.
write(sock, opmsg, opnd_cnt*OPSZ+2);
read(sock, &result, RLT_SIZE);
```