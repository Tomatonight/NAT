#include "connect.h"
Connect* Connect::this_connect=nullptr;
Connect* Connect::get()
{
    if(!Connect::this_connect)
    {
        Connect::this_connect=new Connect();
    }
    return Connect::this_connect;
}
Connect::Connect()
{
}
void Connect::init()
{
    sockfd = socket(AF_NETLINK, SOCK_RAW,NETLINK_FIREWALL);
    sockfd_test= socket(AF_NETLINK, SOCK_RAW,NETLINK_FIREWALL);
    if (sockfd <= 0||sockfd_test<=0)
    {
        edit->append("error: create socket");
    }

    bzero(&address_s_1, sizeof(sockaddr_nl));
    bzero(&address_s_2, sizeof(sockaddr_nl));
    bzero(&address_d, sizeof(sockaddr_nl));
    address_s_1.nl_family = AF_NETLINK;
    address_s_1.nl_pid = PORT_1;
    address_s_1.nl_groups = 0;
    address_s_2.nl_family = AF_NETLINK;
    address_s_2.nl_pid = PORT_2;
    address_s_2.nl_groups = 0;
    address_d.nl_family = AF_NETLINK;
    address_d.nl_groups = 0;
    address_d.nl_pid = 0;
    if (bind(sockfd, (sockaddr *)&address_s_1, sizeof(sockaddr_nl)) != 0)
    {
        edit->append("error: bind socket");
    }
    if (bind(sockfd_test, (sockaddr *)&address_s_2, sizeof(sockaddr_nl)) != 0)
    {
        edit->append("error: bind socket");
    }
    nlhdr = (nlmsghdr *)malloc(NLMSG_SPACE(NL_BUFFER));
    memset(nlhdr,0,NLMSG_SPACE(NL_BUFFER));
    nlhdr->nlmsg_flags = 0;
    nlhdr->nlmsg_seq = 0;
    nlhdr->nlmsg_type = 0;
    nlhdr->nlmsg_pid = PORT_1;
    nlhdr->nlmsg_len = NLMSG_SPACE(NL_BUFFER);
    skaddress_len = sizeof(sockaddr_nl);
    init_recv_thread();

    auto signal_cv=[](int signum)->void{

        Connect::get()->test_ddos();
        Connect::get()->test_scan();
        alarm(ONE_LOOP_TIME);
    };
    signal(SIGALRM,signal_cv);
    alarm(ONE_LOOP_TIME);

}
void Connect::send_msg_to_kernel(char *msg, int len)
{

    memcpy(NLMSG_DATA(nlhdr),msg,len);

    int size=sendto(sockfd,nlhdr,nlhdr->nlmsg_len,0,(sockaddr*)&address_d,skaddress_len);
    if(size<0)
    {
        edit->append("error: send msg");
    }
    bzero(NLMSG_DATA(nlhdr),len);
}
void Connect::receive_from_kernel()
{
    struct user_msg_info user;
    memset(&user,0,sizeof(user_msg_info));
    int size=recvfrom(sockfd,(void*)&user,sizeof(user_msg_info),0,(sockaddr*)&address_d,&skaddress_len);
    if(size<=0)return;
    char buffer[100]={0};
    switch(user.msg.type)
    {
    case BAN_IP:
        struct in_addr addr;
        addr.s_addr=user.msg.data;
        sprintf(buffer,"drop a packet from %s because ip:%s",user.msg.buf,inet_ntoa(addr));
        break;
    case BAN_PORT:
        sprintf(buffer,"drop a packet from %s because port:%d",user.msg.buf,user.msg.data);
    default:
        break;
    };
    edit->append(QString(buffer));
    QTextCursor cursor=edit->textCursor();
    cursor.movePosition(QTextCursor::End);
    edit->setTextCursor(cursor);
}
void Connect::receive_from_kernel_test()
{
    struct user_msg_info_test user;
    memset(&user,0,sizeof(user_msg_info_test));
    int size=recvfrom(sockfd_test,(void*)&user,sizeof(user_msg_info_test),0,(sockaddr*)&address_d,&skaddress_len);
    if(size<=0)return;
    in_addr addr;
    addr.s_addr=user.test.ip;
    printf("%s\n",inet_ntoa(addr));
    switch(user.test.type)
    {
    case ADD_DDOS:
    {
        ddos_add(user.test.ip);
        break;
    }
    case ADD_SCAN:
    {
        scan_add(user.test.ip,user.test.port);
        break;
    }
    default:
        break;
    }
}
Connect::~Connect()
{
    stop_thread=true;
    close(sockfd);
    close(sockfd_test);
    delete nlhdr;
    recv_msg_thread.join();
}
void Connect::init_recv_thread()
{
    timeval tv={0,200};
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval))<0)
    {

        //...
    }
    recv_msg_thread=move(std::thread([&](){
        while(!stop_thread&&sockfd>0)
        {
            this->receive_from_kernel();
        }

    }));
    test_thraed=move(std::thread([&](){
        while(!stop_thread&&sockfd_test>0)
        {
            while(!stop_thread&&sockfd_test>0)
            {
                this->receive_from_kernel_test();
            }
        }
    }));
}
int Connect::judge_port(std::string s)
{
    int t=0;
    if(s.size()==0)return -1;
    for(char c:s)
    {
        if(c>'9'||c<'0')
        {
            return -1;
        }
        t=t*10+(c-'0');
        \
            if(t>65535)return -1;
    }
    return t;
}
void Connect::test_ddos()
{
    std::unique_lock<std::mutex> l(mtx_ddos);
    for(std::unordered_map<uint32_t,uint32_t>::iterator it=test_ip_ddos.begin();it!=test_ip_ddos.end();it++)
    {
        if((*it).second >DDOS_MAX_REQUEST)
        {
            in_addr address;
            address.s_addr=(*it).first;
            char buffer[50]={0};
            sprintf(buffer,"ddos ip:%s times:%d",inet_ntoa(address),(*it).second);
            edit_danger_ip->append(buffer);
        }
    }
    test_ip_ddos.clear();
}
void Connect::test_scan()
{
     std::unique_lock<std::mutex> l(mtx_scan);
    for(std::unordered_map<uint32_t,std::unordered_set<uint16_t>>::iterator it=test_port_scan.begin();it!=test_port_scan.end();it++)
    {
        if((*it).second.size()>SCAN_MAX_REQUEST)
        {

            in_addr address;
            address.s_addr=(*it).first;
            char buffer[50]={0};
            sprintf(buffer,"scan ip:%s times:%d",inet_ntoa(address),(*it).second.size());
            edit_danger_ip->append(buffer);

        }

    }
    test_port_scan.clear();

}
void Connect::ddos_add(uint32_t ip)
{
      std::unique_lock<std::mutex> l(mtx_ddos);
    test_ip_ddos[ip]++;
}
void Connect::scan_add(uint32_t ip,uint16_t port)
{
    std::unique_lock<std::mutex> l(mtx_scan);

    test_port_scan[ip].insert(port);

}

