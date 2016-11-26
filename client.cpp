/**********************

     client.cpp

        by:sissel

**********************/
#include <iostream>
#include <cstdio>
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

#define BUF_SIZE 1000000
#define LOCAL_PORT 200
#define SERV_ADD "127.0.0.1"
#define SERV_PORT 300


using namespace std;

int main(){
    //初始化DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

//*****************此处为监听本地端口！****************
    //创建套接字
    SOCKET localSock = socket(AF_INET, SOCK_STREAM, 0);

    //绑定套接字
    sockaddr_in localSockAddr;
    memset(&localSockAddr, 0, sizeof(localSockAddr));
    localSockAddr.sin_family = PF_INET;  //使用IPv4地址
    localSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //监听的IP地址(本地)
    localSockAddr.sin_port = htons(LOCAL_PORT);  //监听的本地端口
    bind(localSock, (SOCKADDR*)&localSockAddr, sizeof(SOCKADDR));

    //进入监听状态
    listen(localSock, 20);

    //监听请求内容
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    char buffer[BUF_SIZE] = {0};  //缓冲区

//***************向服务器发起请求，以下均为服务端信息********
    sockaddr_in servSockAddr;
    memset(&servSockAddr, 0, sizeof(servSockAddr));
    servSockAddr.sin_family = PF_INET;
    servSockAddr.sin_addr.s_addr = inet_addr(SERV_ADD);//代理服务器地址
    servSockAddr.sin_port = htons(SERV_PORT);//代理服务器端口

    char bufRecv[BUF_SIZE] = {0};//缓存区

    while(1)
    {
        //监听本地内容
        SOCKET clntSock = accept(localSock, (SOCKADDR*)&clntAddr, &nSize);
        int str_len_local = recv(clntSock, buffer, BUF_SIZE, 0);

        cout << buffer; //查看监听的内容

        //创建套接字
        SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        connect(sock, (SOCKADDR*)&servSockAddr, sizeof(SOCKADDR));
        //获取用户输入的字符串并发送给服务器
        send(sock, buffer, strlen(buffer), 0);

        //接收服务器传回的数据
        int str_len = recv(sock, bufRecv, BUF_SIZE, 0);
        //输出接收到的数据
        send(clntSock, bufRecv, str_len, 0);  //将数据原样返回给其它应用
        cout << bufRecv;
        //重置缓冲区
        memset(bufRecv, 0, BUF_SIZE);
        memset(buffer, 0, BUF_SIZE);

        closesocket(clntSock);  //关闭监听的套接字
        closesocket(sock);  //关闭发送服务端的套接字
    }

    //关闭套接字
    closesocket(localSock);

    WSACleanup();  //终止使用 DLL
    return 0;
}
