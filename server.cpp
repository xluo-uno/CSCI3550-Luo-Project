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
main(int argc,char *argv[])
{
  if (argc != 3)
  {
	 std::cerr<<"ERROR: server <PORT> <FILE-DIR>\n"; 
  }
  else
  {
  char fileName[100];
  int pNum;
  stringstream a(argv[1]);
  a>>pNum;
  char* dest;
  dest = argv[2];
  FILE *file;
  int count = 0;
  
  bool result = (pNum >= 1024);
  if(!result)
    {
       std::cerr<<"ERROR:Port Number\n";
        return 7;
    }
	
	// create a socket using TCP IP
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    std::cerr<<"ERROR:setsockopt\n";
    return 1;
  }
  struct timeval tv;
  tv.tv_sec = 11;
  tv.tv_usec = 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
    std::cerr<<"ERROR:timeout\n";
    return 5;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(pNum);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    std::cerr<<"ERROR:bind\n";
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    std::cerr<<"ERROR:listen\n";
    return 3;
  }
  

while(true)
{
  // accept a new connection
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
	std::cerr<<"ERROR:accept\n";
	if (errno == EWOULDBLOCK)
	{
	char buf[] = "ERROR: no file received.";
	sprintf(fileName,"%s/%d.file",(dest),++count);
	file = fopen((fileName), ("wb"));
	fwrite (buf,sizeof(buf),1,file);
	std::cout<<"Connection: Done\n";
	fclose(file);
	close(clientSockfd);
	close(sockfd);
	}
    return 4;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;

  // read/write data from/into the connection
  count++;
  //bool isEnd = false;
  
  char buf[1024] = {0};
  //std::stringstream ss;
  
  
  sprintf(fileName,"%s/%d.file",(dest),count);
  
  file = fopen((fileName), ("wb"));
  
  if (file == 0)
  {
	  std::cerr<<"ERROR:create file failed\n";
	  return 8;
  }
  int r;
  for(;;) {
	
    memset(buf, '\0', sizeof(buf));
	/*if (recv(clientSockfd, buf, 20, 0) == -1) {
      std::cerr<<"ERROR:recv";
      return 5;
    }
	*/
	r = recv(clientSockfd, buf, 1024, 0);
	if (r != 0)
	{
		fwrite(buf,1,r,file);
	}
	else
	{
		break;
	}

    //ss << buf << std::endl;
    //std::cout << buf << std::endl;

    /*if (send(clientSockfd, buf, 20, 0) == -1) {
      std::cerr<<"ERROR:send";
      return 6;
    }
    */
    //if (ss.str() == "close\n")
      //break;

    //ss.str("");
  }
  std::cout<<"Done\n";
  fclose(file);
  close(clientSockfd);
}
close(sockfd);
}
  return 0;
}