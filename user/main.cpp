#include <string>
#include<iostream>
#include"connect.h"
#include"parse_rule.h"
int main()
{
    netlink_connect* connect=new netlink_connect;
    connect->init();
    while (true)
    {
        std::string str;
        std::getline(std::cin,str);
        if(str=="q")break;
        parse_str(str,connect);
    }
    return 0;
}