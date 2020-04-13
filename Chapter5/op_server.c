#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4
void error_handling(char *message);
int calculate(int opnum, int opnds[], char oprator);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char opinfo[BUF_SIZE];
	int result, opnd_cnt, i;
	int recv_cnt, recv_len;	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(serv_sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");	
	clnt_adr_sz=sizeof(clnt_adr);

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
	close(serv_sock);
	return 0;
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

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}