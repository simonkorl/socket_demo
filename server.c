#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#define PORT 1500//端口号 
#define BACKLOG 5/*最大监听数*/ 

struct pseudohdr{
    struct in_addr saddr;
    struct in_addr daddr;
    u_char zero;
    u_char protocol;
    u_short length;
    struct tcphdr tcpheader;
} pseudoheader; //计算TCP校验和字段时需要加上伪首部

char buffer[40]; // 注意40个字节，前半段是ip头部，后半段是tcp头部
struct iphdr *ipheader = (struct iphdr *)buffer;  // iphdr在<netinet/ip.h>中定义
struct tcphdr *tcpheader = (struct tcphdr *)(buffer + sizeof(struct iphdr)); // tcphdr在<netinet/tcp.h>中定义

// checksum()定义
u_short checksum(u_short *data, u_short length){
    register long value; // register……不用了吧……
    u_short i;
    for(int i = 0; i < (length >> 1); i++){
        value += data[i];
    }
    if(1 == (length &1)){
        value += (data[i] << 8);
    }
    value = (value & 65535) + (value >> 16);
    return (~value);
}

int main(){
    int sockfd,new_fd;/*socket句柄和建立连接后的句柄*/
    struct sockaddr_in my_addr;/*本方地址信息结构体，下面有具体的属性赋值*/
    struct sockaddr_in their_addr;/*对方地址信息*/
    
    int sin_size;

    sockfd = socket(AF_INET,SOCK_RAW, 255);//建立socket 
    if(sockfd == -1){
        printf("socket failed:%d",errno);
        return -1;
    }

    memset(buffer, 0, sizeof(struct iphdr) + sizeof(struct tcphdr));

    ipheader->version = 4;
    ipheader->ihl = sizeof(struct iphdr) / 4;
    ipheader->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
    ipheader->id = htons(0xF1C);
    ipheader->ttl = 255;
    ipheader->protocol = 6; // tcp 协议号
    ipheader->saddr = inet_addr("127.0.0.1");
    ipheader->daddr = inet_addr("127.0.0.1");

    tcpheader->source = htons(PORT);
    tcpheader->dest = htons(PORT + 200);
    tcpheader->seq = htonl(0xF1C);
    tcpheader->syn = 1;
    tcpheader->doff = sizeof(struct tcphdr) / 4;
    tcpheader->window = htons(2048);

    // 计算TCP首部校验和字段，IP首部校验和字段没设
    memset(&pseudoheader, 0, 12 + sizeof(struct tcphdr));
    pseudoheader.saddr.s_addr = inet_addr("127.0.0.1"); // 上面的源IP地址
    pseudoheader.daddr.s_addr = inet_addr("127.0.0.1"); // 上面的目的IP地址
    pseudoheader.protocol = 6;
    pseudoheader.length = htons(sizeof(struct tcphdr));
    bcopy((char *)tcpheader, (char *)&pseudoheader.tcpheader, sizeof(struct tcphdr));
    tcpheader->check = checksum((u_short *)&pseudoheader, 12 + sizeof(struct tcphdr));

    my_addr.sin_family=AF_INET;/*该属性表示接收本机或其他机器传输*/
    my_addr.sin_port=htons(PORT);/*端口号*/
    my_addr.sin_addr.s_addr=htonl(INADDR_ANY);/*IP，括号内容表示本机IP*/

    bzero(&(my_addr.sin_zero),8);/*将其他属性置0*/
    if(-1 == sendto(sockfd, buffer, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, &my_addr, sizeof(my_addr))){
        // error handler;
        printf("error in sendto\n");
        return -1;
    }
    close(sockfd);
    return 0;
} 

int raw(int argc,char* argv[]) {
    int port = 3490;
    int sockFd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    struct sockaddr_in my_addr; 
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(my_addr.sin_zero), 8);
    bind(sockFd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    return 0;
}