#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<arpa/inet.h>
const char* server_ip="127.0.0.1";
const int server_port=8888;
int main()
{
    int sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
sockaddr_in addr_s,addr_d;
    addr_s.sin_addr.s_addr=inet_addr(server_ip);
    addr_s.sin_family=AF_INET;
    addr_s.sin_port=server_port;
    
    if(bind(sockfd,(sockaddr*)&addr_s,sizeof(sockaddr)))
    {  printf("r");
    }
    listen(sockfd,100);
    socklen_t len=sizeof(addr_d);
    int sock=accept(sockfd,(sockaddr*)&addr_d,&len);
    printf("recv");
    char buffer[500];
    while(true)
    {
read(sock,buffer,499);
    }
}
