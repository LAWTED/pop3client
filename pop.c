#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <b64/cencode.h>
#include <b64/cdecode.h>
#include <regex.h>
// PASS VPHCBMGOEYSSIRLW
#define SIZE 100

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"
void go_back(int sockfd);

void clrscr()
{
  system("@cls||clear");
}

char *send_command(int sockfd, char *command)
{
  char buf[1000000];
  memset(buf, 0, sizeof(buf));
  write(sockfd, command, strlen(command));
  read(sockfd, buf, sizeof(buf));
  char *ret = malloc(strlen(buf) + 1);
  strcpy(ret, buf);
  return ret;
}

void get_user(int sockfd)
{
  char command[1024] = "USER ";
  char tmp[1024];
  printf("Enter your username: ");
  scanf("%s", tmp);
  strcat(tmp, "\r\n");
  strcat(command, tmp);
  send_command(sockfd, command);
  memset(command, 0, sizeof(command));
}

void get_pass(int sockfd)
{
  char command[1024] = "PASS ";
  char tmp[1024];
  char *password = getpass("Enter your password: ");
  strcpy(tmp, password);
  strcat(tmp, "\r\n");
  strcat(command, tmp);
  send_command(sockfd, command);
  memset(command, 0, sizeof(command));
}

void test_get_user(int sockfd)
{
  send_command(sockfd, "USER lawted0605\r\n");
}

void test_get_pass(int sockfd)
{
  send_command(sockfd, "PASS VPHCBMGOEYSSIRLW\r\n");
}

void get_list(int sockfd)
{
  char command[1024] = "LIST\r\n";
  char *msg = send_command(sockfd, command);
  printf("%s", msg);
  memset(command, 0, sizeof(command));
  go_back(sockfd);
}

void get_stat(int sockfd)
{
  char command[1024] = "STAT\r\n";
  char *msg = send_command(sockfd, command);
  printf("%s\n", msg);
  memset(command, 0, sizeof(command));
  go_back(sockfd);
}

char *decode(const char *input)
{
  /* set up a destination buffer large enough to hold the encoded data */
  char *output = (char *)malloc(SIZE);
  /* keep track of our decoded position */
  char *c = output;
  /* store the number of bytes decoded by a single call */
  int cnt = 0;
  /* we need a decoder state */
  base64_decodestate s;

  /*---------- START DECODING ----------*/
  /* initialise the decoder state */
  base64_init_decodestate(&s);
  /* decode the input data */
  cnt = base64_decode_block(input, strlen(input), c, &s);
  c += cnt;
  /* note: there is no base64_decode_blockend! */
  /*---------- STOP DECODING  ----------*/

  /* we want to print the decoded data, so null-terminate it: */
  *c = 0;

  return output;
}

int reg_matches(const char *str, const char *pattern)
{
    regex_t re;
    int ret;

    if (regcomp(&re, pattern, REG_EXTENDED) != 0)
        return 0;

    ret = regexec(&re, str, (size_t) 0, NULL, 0);
    regfree(&re);

    if (ret == 0)
        return 1;

    return 0;
}

void get_retr(int sockfd)
{
  char command[1024] = "RETR ";
  char file_num[1024];
  printf("Enter the file index: ");
  scanf("%s", file_num);
  strcat(command, file_num);
  strcat(command, "\r\n");
  char *msg = send_command(sockfd, command);
  char *p, *tmp;
  p = strtok_r(msg, "\n", &tmp);
  int flag = 0;
  do
  {
    if (flag == 2) {
      p = decode(p);
      flag = 0;
    }
    if (strstr(p, "Content-Transfer-Encoding: base64") != NULL)
    {
      flag = 1;
    }
    if (*p == '\r' && flag == 1)
    {
      flag = 2;
    }
    printf("%s\n", p);
  } while ((p = strtok_r(NULL, "\n", &tmp)) != NULL);
  memset(command, 0, sizeof(command));
  go_back(sockfd);
}

void save_file(char *str, char *filename)
{
  FILE *fp;
  fp = fopen(filename, "w");
  fprintf(fp, "%s", str);
  fclose(fp);
}

void download(int sockfd)
{
  char command[1024] = "RETR ";
  char file_num[1024];
  printf("Enter the file index: ");
  scanf("%s", file_num);
  strcat(command, file_num);
  strcat(command, "\r\n");
  char *msg = send_command(sockfd, command);
  save_file(msg, strcat(file_num, ".eml"));
  memset(command, 0, sizeof(command));
  char command2[1024] = "DELE ";
  strcat(command2, file_num);
  strcat(command2, "\r\n");
  send_command(sockfd, command2);
  go_back(sockfd);
}

void search(int sockfd)
{
  char command[1024] = "LIST\r\n";
  char *msg = send_command(sockfd, command);
  memset(command, 0, sizeof(command));
  char *p, *tmp;
  p = strtok_r(msg, "\n", &tmp);
  int lines = 0;
  do {
    lines += 1;
  } while ((p = strtok_r(NULL, "\n", &tmp)) != NULL);
  lines -= 2;
  for (int i = 0 ; i < lines ; i++)
  {
    char command2[1024] = "RETR ";
    char file_num[1024];
    sprintf(file_num, "%d", i + 1);
    strcat(command2, file_num);
    strcat(command2, "\r\n");
    char *msg = send_command(sockfd, command2);
    memset(command2, 0, sizeof(command2));
    char *p, *tmp;
    p = strtok_r(msg, "\n", &tmp);
    int flag = 0;
    do
    {
      if (flag == 2) {
        p = decode(p);
        flag = 0;
      }
      if (strstr(p, "Content-Transfer-Encoding: base64") != NULL)
      {
        flag = 1;
      }
      if (*p == '\r' && flag == 1)
      {
        flag = 2;
      }
      if (strstr(p, "Subject:") != NULL)
      {
        printf("%s\n", p);
      }
    } while ((p = strtok_r(NULL, "\n", &tmp)) != NULL);
  }
  go_back(sockfd);
}

void menu(int sockfd)
{
  int choice;
  printf("********************************************************\n");
  printf(RED "1. Quit\n" RESET);
  printf("2. Get a list of messages and size\n");
  printf("3. Get mail status\n");
  printf("4. Display a message in detail\n");
  printf("5. Search text in all mails<not valid>\n");
  printf("6. Display by subject\n");
  printf("7. Download the email\n");
  printf("********************************************************\n");
  printf("Please enter your choice: \n");
  printf(">>");
  scanf("%d", &choice);

  switch (choice)
  {
  case 1:
    printf("Quit\n");
    exit(0);
    break;
  case 2:
    printf("Get a list of messages and size\n");
    printf(GRN "********************************************************\n" RESET);
    get_list(sockfd);
    break;
  case 3:
    printf("Get mail status\n");
    printf(GRN "********************************************************\n" RESET);
    get_stat(sockfd);
    break;
  case 4:
    printf("Display a message in detail\n");
    printf(GRN "********************************************************\n" RESET);
    get_retr(sockfd);
    break;
  case 5:
    printf("Search text in all mails\n");
    break;
  case 6:
    printf("Display by subject\n");
    search(sockfd);
    break;
  case 7:
    printf("Download the email\n");
    printf(GRN "********************************************************\n" RESET);
    download(sockfd);
    break;
  }
}

void go_back(int sockfd)
{
  int choice;
  printf(GRN "********************************************************\n" RESET);
  printf(RED "1.Quit\n" RESET);
  printf("2.Back to Menu\n");
  printf("Please enter your choice: \n");
  printf(">>");
  scanf("%d", &choice);
  switch (choice)
  {
  case 1:
    exit(0);
  case 2:
    clrscr();
    menu(sockfd);
    break;
  }
}

// get content of email

int main()
{
  char buf[1024];
  printf("Welecom to pop3 client\n");
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in servaddr;
  if (sockfd < 0)
  {
    printf("socket error\n");
    return 1;
  }
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(110);

  struct hostent *host = gethostbyname("pop.163.com");
  if (host == NULL)
  {
    printf("gethostbyname error\n");
    return 1;
  }
  printf("address: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr));
  servaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)host->h_addr));
  int i = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  read(sockfd, buf, sizeof(buf));
  memset(buf, 0, sizeof(buf));
  if (i < 0)
  {
    printf("connect error\n");
    return 1;
  }

  printf("Socket Connected\n");

  get_user(sockfd);
  get_pass(sockfd);

  // test_get_user(sockfd);
  // test_get_pass(sockfd);

  clrscr();
  menu(sockfd);
  return 0;
}
