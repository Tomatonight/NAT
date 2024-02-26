#主界面
![](png/a.png)
#包过滤
![](png/b.png)

支持tcp udp icmp 及 input output forward 三个hook点

匹配规则（ip_a&mask）=(ip_b&mask)

端口为0则为全匹配

#动态nat
![](png/c.png)

开两网卡，禁止第二个网卡流量进出（hook：input output），让第二个网卡做第一个网卡的nat ip地址



