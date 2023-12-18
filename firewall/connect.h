#ifndef CONNECT_H
#define CONNECT_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include<arpa/inet.h>
#include<time.h>
#include<unordered_set>
#include<QTextCursor>
#include<set>
#include<QTextEdit>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<fcntl.h>
#include<signal.h>
#define NL_BUFFER 50
#define PORT_1 100
#define PORT_2 101
#define DDOS_MAX_REQUEST 40
#define SCAN_MAX_REQUEST 300
#define ONE_LOOP_TIME 5//seconds
enum recv_msg_type
{
    BAN_IP=0,
    BAN_PORT,
    ADD_DDOS,
    ADD_SCAN,
};
struct recv_msg_test
{
    enum recv_msg_type type;
    unsigned int ip;
     unsigned short port;
};

struct recv_msg
{
    enum recv_msg_type type;
    unsigned int data;
    char buf[20]; // name of dev_name
};
struct user_msg_info
{
    struct nlmsghdr hdr;
    recv_msg msg;
};
struct user_msg_info_test
{
     struct nlmsghdr hdr;
    struct recv_msg_test test;
};

struct send_msg{
    send_msg(char c1,char c2,uint32_t v)
    {
        ctl[0]=c1;
        ctl[1]=c2;
        ip=v;
    }
    send_msg(char c1,char c2,uint16_t port_l,uint16_t port_r)
    {
        ctl[0]=c1;
        ctl[1]=c2;
        ports[0]=port_l;
        ports[1]=port_r;
    }
    char ctl[2];
    union
    {
        uint32_t ip;
        uint16_t ports[2];
    };
};
class Connect
{
public:
    static Connect* get();
    ~Connect();
    void init();
    void send_msg_to_kernel(char *msg,int len);
    void receive_from_kernel();
        void receive_from_kernel_test();
    int judge_port(std::string);
    void set_out(QTextEdit* edit_,QTextEdit* edit_danger_ip_)
    {
        edit=edit_;
        edit_danger_ip=edit_danger_ip_;
    }
    void add_ip(uint32_t ip)
    {
        ips.insert(ip);
    }
    void delete_ip(uint32_t ip)
    {
        ips.erase(ip);
    }
    bool find_ip(uint32_t ip)
    {
        return ips.find(ip)!=ips.end();
    }

    void add_port(uint16_t port_l,uint16_t port_r)
    {

        ports.insert({port_l,port_r});
    }
    void delete_port(uint16_t port_l,uint16_t port_r)
    {

        ports.erase({port_l,port_r});
    }
    bool find_port(uint16_t port_l,uint16_t port_r)
    {

        return ports.find({port_l,port_r})!=ports.end();
    }
    void reset_rules()
    {
        ips.clear();
        ports.clear();
        stop_defence=1;
    }

    void test_ddos();
    void test_scan();
    void ddos_add(uint32_t);
    void scan_add(uint32_t,uint16_t);

    std::unordered_set<uint32_t> ips;
    std::set<std::pair<uint16_t,uint16_t>> ports;
    std::unordered_map<uint32_t,std::unordered_set<uint16_t>> test_port_scan;
    std::unordered_map<uint32_t,uint32_t> test_ip_ddos;
  // QString buffer;
private:
     void init_recv_thread();
     Connect();
   static Connect* this_connect;
    int sockfd = 0;
   int sockfd_test=0;
    sockaddr_nl address_s_1,address_s_2, address_d;
    nlmsghdr *nlhdr = nullptr;
    socklen_t skaddress_len;
    QTextEdit* edit;
    QTextEdit* edit_danger_ip;
    std::thread recv_msg_thread;
    std::thread test_thraed;
    bool stop_thread=0;
    bool stop_defence=1;
    std::mutex mtx_ddos;
    std::mutex mtx_scan;
   // std::condition_variable cv;

};
#endif // CONNECT_H
