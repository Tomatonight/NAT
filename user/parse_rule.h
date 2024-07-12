#pragma once
#include<iostream>
#include<string>
#include<regex>
#include"connect.h"
void parse_str(std::string str,netlink_connect* connect);
void print_msg(MSG_RULE *msg);