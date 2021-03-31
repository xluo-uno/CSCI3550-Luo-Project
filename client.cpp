#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
using namespace std;
int
main(int argc, char *argv[])
{
  int pNum;
  char* fileName;
  char* hostName;
  hostName = argv[1];
  stringstream a(argv[2]);
  a>>pNum;
  fileName = argv[3];
   
  bool result = (pNum >= 1024);
  if(!result)
    {
       std::cerr<<"ERROR:connect";
       return 1;
    }
	if (strcmp(hostName, "127.0.0.1") != 0 && strcmp(hostName, "localhost") != 0)
  {
	  std::cerr<<"ERROR:connect";
      return 6;
  }
  if (strcmp(argv[1], "localhost") == 0)
  {
	  hostName = "127.0.0.1";
  }

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001 port);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1 hostname");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(pNum);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr(hostName);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    std::cerr<<"ERROR:connect";
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    std::cerr<<"ERROR:getsockname";
    return 3;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;


  // send/receive data to/from connection
  
  /*bool isEnd = feof(file);
  //std::string input;
  //std::stringstream ss;
  */
  
  char buf[20] = {0};
  
  FILE* file = fopen((fileName), ("rb"));
  if (file == NULL)
  {
	  std::cerr<<"ERROR:file";
	  return 7;
  }
  
  std::cout << "send: ";
  int r;
  while (!feof(file)) {
    memset(buf, '\0', sizeof(buf));
	/*int size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    */
    r = fread(buf, 1, 20, file);
    if (send(sockfd, buf, r, 0) == -1) 
	{
      std::cerr<<"ERROR";
      return 4;
    }
    /*if (recv(sockfd, buf, 20, 0) == -1) 
	{
      std::cerr<<"ERROR";
      return 5;
    }
    ss << buf << std::endl;
    std::cout << "echo: ";
    std::cout << buf << std::endl;
    if (ss.str() == "close\n")
      break;

    ss.str("");
	*/
  }
  fclose(file);
  close(sockfd);

  return 0;
}