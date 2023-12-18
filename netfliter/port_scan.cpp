#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
int main()
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    fcntl(sockfd,F_SETFD,fcntl(sockfd,F_GETFD,0)|O_NONBLOCK);
    sockaddr_in addr_d;
    addr_d.sin_addr.s_addr=inet_addr("127.0.0.1");
    addr_d.sin_family=AF_INET;
    for(int i=0;i<1000;i++)
    {
        addr_d.sin_port=i+1000;
        connect(sockfd,(sockaddr*)&addr_d,sizeof(sockaddr_in));
    }
    return 0;
}