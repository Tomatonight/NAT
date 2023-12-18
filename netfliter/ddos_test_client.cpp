#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<thread>
#include<arpa/inet.h>
#include<chrono>
const char* packet_http="test";
const char* server_ip="127.0.0.1";
const int server_port=8888;
int main()
{
int sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(sockfd<0)
{
printf("error");
}
sockaddr_in addr_d;
    addr_d.sin_addr.s_addr=inet_addr(server_ip);
    addr_d.sin_family=AF_INET;
    addr_d.sin_port=server_port;
connect(sockfd,(sockaddr*)&addr_d,sizeof(sockaddr));
printf("con");
for(int i=0;i<500;i++)
{
    write(sockfd,packet_http,strlen(packet_http));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

}
