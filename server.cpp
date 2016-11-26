/***********************

     server.cpp

        by:sissel

***********************/

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

#define BUF_SIZE 1000000
#define SERV_PORT 300

using namespace std;

int get_requests_info(char *buffer, char *ip, int *port)
{
    int i;
    char m_port[] = "HTTP/";
    char m_http[] = "http://";
    char m_https[] = "https://";
    char m_host[] = "Host: ";
    char address[1000];//域名
    char host[500];
    char *ptr;
    *port = 0;
    //域名
    /*
    ptr = strchr(buffer, ' ');
    if(ptr==NULL)
    {
        cout << "error requests************************" <<endl;
        return 2;
    }
    else if(strstr(buffer,m_http)!=NULL)
        ptr += 7;
    else if(strstr(buffer,m_https)!=NULL)
        ptr += 8;

    for(i = 0, ptr+=1;*ptr != ' ';ptr++,i++)
    {
        address[i] = *ptr;
    }
    address[i] = '\0';
    */

    //HOST
    ptr = strstr(buffer, m_host);
    if(ptr==NULL)
    {
        cout << "error host----------do not have host:" << endl;
        return -1;
    }
    for(i = 0, ptr += 6;*ptr != '\r' && *ptr != ':';ptr++,i++)
    {
        host[i] = *ptr;
    }
    if(*ptr == ':')
    {
        for(ptr+=1;*ptr!='\r'&&*ptr!=' ';ptr++)
        {
            *port *= 10;
            *port += (*ptr-'0');
        }
    }
    host[i] = '\0';

    //端口
    ptr = strstr(buffer, m_port);
    if(*port==0)
        if(ptr!=NULL)
            *port = 80;
        else
            *port = 443;

    struct hostent *host_addr = gethostbyname(host);
    if (host_addr == NULL)
    {
        cout << "host error ----------- cannot use gethostbyname()" <<endl;
        return -1;
    }
    memset(host, 0, sizeof(host));
    strcpy(host, host_addr->h_name);
    strcpy(ip, inet_ntoa( *(struct in_addr*)host_addr->h_addr_list[0] ));

    return 1;
}

int main(){
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);

    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    sockAddr.sin_port = htons(SERV_PORT);  //服务器开启的用来监听的端口
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //进入监听状态
    listen(servSock, 20);

    //接收客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    char buffer[BUF_SIZE] = {0};  //缓冲区
    char buf_recv[BUF_SIZE] = {0};//缓存区

    while(1)
    {
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
        int str_len_recv = recv(clntSock, buffer, BUF_SIZE, 0);  //接收客户端发来的数据

        cout << buffer;

        char ip[30];
        int port;

        if(get_requests_info(buffer, ip, &port)!=1&&port!=80)
        {
            send(clntSock, "https ----- ignore\n", 30 , 0);
        }
        else
        {
            //服务器发送请求
            SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            cout << "****************ip:" << ip <<"\tport:" <<port<<endl;

            sockaddr_in servSockAddr;
            memset(&servSockAddr, 0, sizeof(servSockAddr));
            servSockAddr.sin_family = PF_INET;
            servSockAddr.sin_addr.s_addr = inet_addr(ip);
            servSockAddr.sin_port = htons(80);

            connect(sock, (SOCKADDR*)&servSockAddr, sizeof(SOCKADDR));
            send(sock, buffer, strlen(buffer), 0);
            //接收服务器传回的数据
            int str_len = recv(sock, buf_recv, BUF_SIZE, 0);

            cout << buf_recv;
            //将数据返回给本地
            send(clntSock, buf_recv, str_len, 0);

            //关闭套接字
            closesocket(sock);
        }
        closesocket(clntSock);
        //重置缓冲区
        memset(buffer, 0, BUF_SIZE);
        memset(buf_recv, 0, BUF_SIZE);
        memset(ip, 0, 30);
    }

    //关闭套接字
    closesocket(servSock);

    //终止 DLL 的使用
    WSACleanup();

    return 0;
}
