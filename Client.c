// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <semaphore.h>

#define MAXIN 20
#define MAXOUT 20

struct packet{
  int sockfd;
};

sem_t semvar;


void *send_t(void *p) {
  char sndbuf[MAXIN];/* Get request char stream */
  printf("IN sender\n");
  struct packet *p1 = (struct packet *)p;
  while(1)
  { 
    scanf("%s",sndbuf);
    write(p1->sockfd, sndbuf, strlen(sndbuf)); /* send */
  }
}
void *receive_t(void *p) {
   int n;
  char rcvbuf[MAXOUT];/* Get request char stream */
  printf("IN receiver\n");
  struct packet *p1 = (struct packet *)p;
  while(1)
  {
    memset(rcvbuf,0,MAXOUT);               /* clear */
    n=read(p1->sockfd, rcvbuf, MAXOUT-1);      /* receive */
    if(n>0)
    {
      sem_wait(&semvar);
      printf("Recvd msg from sender:%s\n", rcvbuf);
      sem_post(&semvar);
      //write(STDOUT_FILENO, rcvbuf, n);        /* echo */
    }
  }
}

void client(int sockfd) {
  char rcvbuf[MAXOUT];
  pthread_t tid_send,tid_receive;

   struct packet *p_r = (struct packet*)malloc(sizeof(struct packet));
   p_r->sockfd = sockfd;
   int r_r = pthread_create(&tid_receive, NULL,&send_t,p_r);
  
    struct packet *p_s = (struct packet*)malloc(sizeof(struct packet));
    p_s->sockfd = sockfd;
    int r_s = pthread_create(&tid_send, NULL,&receive_t,p_s);

   while(1){ 
    
   }
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
	char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
 }


int main() {
	//Client protocol
	char *serverIP = "10.8.12.48";
	int sockfd, portno = 5033;
	struct sockaddr_in serv_addr;
	sem_init(&semvar, 0, 1);
	buildServerAddr(&serv_addr, serverIP, portno);
//semaphore intialization



	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect to server on port */
	if(!connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
	   printf("Connected to %s:%d\n",serverIP, portno);

	/* Carry out Client-Server protocol */
	client(sockfd);

	/* Clean up on termination */
	close(sockfd);
}