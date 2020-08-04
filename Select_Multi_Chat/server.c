#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#define BUF_SIZE 100
#define MAX_CLIENT 10
#define SWAP(a, b){int t=a; a=b; b=t;}
int check=0;
void error_handling(char *message);

/* 자료구조 추가하기 */
// 단순연결 리스트 
typedef char element[100];
typedef struct ListNode{
	int fd;
	element data;
	struct ListNode *link;
}ListNode;

ListNode * insert(ListNode * head, element data, int new_fd){
	ListNode *newNode = (ListNode*)malloc(sizeof(ListNode));
	strcpy(newNode->data, data);
	newNode->fd = new_fd;
	
	newNode->link = head;
	head = newNode;
	return head;
}

void print_List(ListNode *head){
	for(ListNode *cur = head; cur != NULL; cur = cur->link){
		printf("%s -> ", cur->data);
		printf("FD : %d \n", cur->fd);
	}
}
int GetListLength(ListNode *head){
	ListNode *cur = head;
	cur = cur->link;
	int cnt=1;
	for(; cur != head; cur = cur->link){
		++cnt;
	}
	return cnt;
}
ListNode * Delete(ListNode *head){
	ListNode *removed;
	if(head == NULL) return NULL;
	
	removed = head;
	head = removed->link;
	free(removed);
	return head;
}

// 사용자로부터 전송받은 인증정보 확인 
int SearchListNode(ListNode *head, char *Log_buf, int clnt_fd){
	
	for(ListNode *cur = head; cur != NULL; cur = cur->link){
		if(!strcmp(cur->data, Log_buf))	// 찾는 아이디와 패스워드가 일치하고
			if(cur->fd == clnt_fd) // 파일 디스크립터(해당 사용자)가 맞다면
				return 1;
			else
				return 2;
	}
	return 0; // 아무것도 없으면 
}

void sorting(int *a, int num_chat){
	for(int i=0; i<MAX_CLIENT-1; i++){
		for(int j=i+1; j<MAX_CLIENT; j++){
			if(a[i] < a[j])
				SWAP(a[i], a[j]);
		}
	}
}

void set_zero(int *arr){
	for(int i=0; i<MAX_CLIENT; i++)
		arr[i]=0;
}

int counting_arr(int *a){
	int cnt=0;
	for(int i=0; i<MAX_CLIENT; i++)
	{
		if(a[i] > 0)
			cnt++;
	}
	return cnt;
}
int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	socklen_t adr_sz;

	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	char chat_buf[BUF_SIZE];
	int client_List[MAX_CLIENT];
	int num_chat=0;
	
	set_zero(client_List);

	// 로그인/회원가입 인스턴스
	char Login[BUF_SIZE];
	ListNode *head = NULL;
	int compare=0;
	int List_cnt=0;

	if(argc !=2){
		printf("Usage : %s <Port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while(1){
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec=5000;
	
		// 소켓 포함, 모든 파일 디스크립터를 대상으로 수신된 데이터의 존재 여부  검사 
		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;
		// timeout 발생 시
		if(fd_num == 0)
			continue;
		for(i=0; i<fd_max+1; i++){
			if(FD_ISSET(i, &cpy_reads)){
				// 서버 소켓 디스크립터에 변화가 있을 때, 연결 요청 
				if(i==serv_sock){
					adr_sz = sizeof(clnt_adr);
					clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					
					if(clnt_sock == -1)
						error_handling("accept() error");

					
					while(1){
						strcpy(buf, "로그인(1), 회원가입(2)");
						write(clnt_sock, buf, strlen(buf));
						read(clnt_sock, buf, BUF_SIZE);

						if(buf[0] == '1'){	// 로그인
							write(clnt_sock, "1", strlen("1"));// 사용자 선택 번호 리턴 
							read(clnt_sock, Login, BUF_SIZE); // 사용자 로그 받기 
							// 리스트에 있는지 검사 
							compare = SearchListNode(head, Login, clnt_sock);
							
							if(compare == 1){	// 로그인 성공 
								// 로그인 결과 전달 
								strcpy(buf, "3. 로그인 성공, 채팅방 입장");
								write(clnt_sock, buf, strlen(buf));

								client_List[num_chat] = clnt_sock;
								sorting(client_List, num_chat);
								num_chat = counting_arr(client_List);

								//clnt_sock 디스크립터를 읽기 전용으로 set 
								FD_SET(clnt_sock, &reads);

								if(fd_max < clnt_sock)
								fd_max = clnt_sock;

								check=1;
							}		
							else{
								// 로그인 결과 전달 
								strcpy(buf, "4. 로그인 실패, 다시 접속하세요\n");
								write(clnt_sock, buf, strlen(buf));
								FD_CLR(clnt_sock, &reads);
								close(clnt_sock);
								check=0;
							}
						}
						else if(buf[0] == '2'){	// 회원가입
							write(clnt_sock, "2", strlen("2")); // 사용자 선택 번호 리턴 
							read(clnt_sock, Login, BUF_SIZE);
							fputs(Login, stdout); // 들어온 로그정보 확인 

							// 채팅 리스트에 추가 
							client_List[num_chat] = clnt_sock;
							sorting(client_List, num_chat);
							num_chat = counting_arr(client_List);
						
							//clnt_sock 디스크립터를 읽기 전용으로 set 
							FD_SET(clnt_sock, &reads);
		
							head = insert(head, Login, clnt_sock);
				
							strcpy(buf, "5. 회원가입 성공, 채팅방 입장\n");
							write(clnt_sock, buf, strlen(buf));
				
							if(fd_max < clnt_sock)
							fd_max = clnt_sock;
	
							check=1;
						}
						if(check == 1)
							break;
					}

					
				
					
					
					printf("connected client : %d \n", clnt_sock);
					printf("현재 인원 수 : %d \n", num_chat);
					printf("\n[[접속된 로그 리스트]]\n");
					print_List(head);


				}
				// 서버 소켓이 아닌 연결 요청되었던 다른 클라이언트들 중 하나로부터 입력 신호 감지했을 때
				else if(i != serv_sock){
					str_len = read(i, chat_buf, BUF_SIZE);

					if(str_len==0){	// str_len == 0
						
						for(int z=0; z<num_chat; z++){
							if(client_List[z] == i){	// 삭제하려는 소켓 디스크립터를 찾으면 	
								client_List[z] = 0;
								break;
							}
						}
						
						FD_CLR(i, &reads);
						close(i);
						printf("-------------나간사람 : %d-------------\n\n", i);
					}
					else{
						for(int k=0; k<num_chat; k++){
							if(client_List[k] != 0)
								write(client_List[k], chat_buf, str_len);
						}
					}
					
				}
			}
		}
	}
	List_cnt = GetListLength(head);
	for(int i=0; i<List_cnt; i++)
		head = Delete(head);
	close(serv_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}