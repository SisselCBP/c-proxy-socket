/*************

   server.c

*************/

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

#define BUF_SIZE 1000000
#define SERV_PORT 300

using namespace std;

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

    while(1){
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
        int str_len_recv = recv(clntSock, buffer, BUF_SIZE, 0);  //接收客户端发来的数据
        cout << buffer;
        //send(clntSock, buffer, strLen, 0);  //将数据原样返回

        //服务器发送请求
        SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        sockaddr_in servSockAddr;
        memset(&servSockAddr, 0, sizeof(servSockAddr));
        servSockAddr.sin_family = PF_INET;
        servSockAddr.sin_addr.s_addr = inet_addr("121.42.187.97");
        servSockAddr.sin_port = htons(80);

        connect(sock, (SOCKADDR*)&servSockAddr, sizeof(SOCKADDR));
        send(sock, buffer, strlen(buffer), 0);

        //接收服务器传回的数据
        int str_len = recv(sock, buf_recv, BUF_SIZE, 0);
        //输出接收到的数据

        send(clntSock, buf_recv, str_len, 0);  //将数据返回给本地

        closesocket(sock);
        closesocket(clntSock);  //关闭套接字
        memset(buffer, 0, BUF_SIZE);  //重置缓冲区
        memset(buf_recv, 0, BUF_SIZE);
    }

    //关闭套接字
    closesocket(servSock);

    //终止 DLL 的使用
    WSACleanup();

    return 0;
}
