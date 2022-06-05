#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
// PASS VPHCBMGOEYSSIRLW

// clear screen
void clrscr()
{
  system("@cls||clear");
}
// print the menu of pop3client
int menu()
{
  int choice;
  printf("********************************************************\n");
  printf("1. Quit\n");
  printf("2. Get a list of messages and size\n");
  printf("3. Get mail status\n");
  printf("4. Display a message in detail\n");
  printf("5. Search text in all mails");
  printf("6. Display by subject\n");
  printf("7. Download the email\n");
  printf("********************************************************\n");
  printf("Please enter your choice: \n");
  printf(">>");
  scanf("%d", &choice);
  return choice;
}

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
  char tmp[1024];
  memset(buf, 0, sizeof(buf));
  read(sockfd, buf, sizeof(buf));
  printf("\nUSER: ");
  scanf("%s", tmp);
  strcat(tmp, "\r\n");

  memset(buf, 0, sizeof(buf));
  strcpy(buf, "USER ");
  strcat(buf, tmp);
  write(sockfd, buf, strlen(buf));
  memset(buf, 0, sizeof(buf));
  read(sockfd, buf, sizeof(buf));
  printf("\nPASS: ");
  scanf("%s", tmp);
  strcat(tmp, "\r\n");

  memset(buf, 0, sizeof(buf));
  strcpy(buf, "PASS ");
  strcat(buf, tmp);
  write(sockfd, buf, strlen(buf));
  memset(buf, 0, sizeof(buf));
  read(sockfd, buf, sizeof(buf));
  printf("\nLogin Sucessful\n");
  // printf("\n%s", buf);
  sleep(1);
  clrscr();
  int choice = menu();
  return 0;
}
