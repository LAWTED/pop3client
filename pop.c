// a pop3 client
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
int main()
{
  printf("Welecom to pop3 client\n");
  // send message by socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in servaddr;
  if (sockfd < 0)
  {
    printf("socket error\n");
    return 1;
  }
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(110);
  // gethostbyname() returns a structure of type hostent
  struct hostent *host = gethostbyname("pop.163.com");
  if (host == NULL)
  {
    printf("gethostbyname error\n");
    return 1;
  }
  printf("address: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr));
  servaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)host->h_addr));
  int i = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (i < 0)
  {
    printf("connect error\n");
    return 1;
  }
  char buf[1024];
  memset(buf, 0, sizeof(buf));
  read(sockfd, buf, sizeof(buf));
  printf("%s\n", buf);
  return 0;
}
