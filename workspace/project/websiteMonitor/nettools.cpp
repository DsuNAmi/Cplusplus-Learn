#include "nettools.h"


NetTools::NetTools() : ipAddress(""), port("") 
{

}

NetTools::NetTools(std::string & ipAddress, std::string & port)
    : ipAddress(ipAddress), port(port)
{

}


bool NetTools::Setup()
{
    return !WSAStartup(MAKEWORD(2,2),&(this->wsadata));
}

bool NetTools::Endup()
{
    WSACleanup();
    return true;
}


bool NetTools::Connect()
{
    this->netSocket = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
    if(this->netSocket == SOCKET_ERROR)
    {
        printf("Failed to create socket!\n");
        return false;
    }

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_addr.s_addr = inet_addr(this->ipAddress.c_str());
    this->serverAddress.sin_port = htons(atoi(this->port.c_str()));

    if(0 != connect(this->netSocket,(SOCKADDR*)&(this->serverAddress),sizeof(this->serverAddress)))
    {
        printf("Failed to Connect:\n");
        closesocket(this->netSocket);
        printf("socket closed!\n");
        return false;
    }

    return true;
}

bool NetTools::Request(std::string & request)
{
    if(SOCKET_ERROR == send(this->netSocket, request.c_str(), static_cast<int>(request.size()),0))
    {
        printf("Send Failed!\n");
        closesocket(this->netSocket);
        printf("socket closed!\n");
        return false;
    }

    return true;
}


bool NetTools::RecvC_Verson(int & bytesReaded, std::string &response,int content)
{
    char * pageBuf = (char*)malloc(content);
    memset(pageBuf, 0, content);
    bytesReaded = 0;
    int recvBytes = 1;
    // printf("Readed::  \n");

    while(recvBytes > 0)
    {
        recvBytes = recv(this->netSocket, pageBuf + bytesReaded, content - bytesReaded, 0);
        if(recvBytes > 0)
        {
            bytesReaded += recvBytes;
        }   

        if(content - bytesReaded < 100)
        {
            //realloc
            content *= 2;
            pageBuf = (char*)realloc(pageBuf,content);
        }
    }

    pageBuf[bytesReaded] = '\0';
    response = pageBuf;
    free(pageBuf);
    // closesocket(this->netSocket);
    return true;
}

void NetTools::CreateRequest(std::string & out, const std::string & data, const std::string & host,const std::string connection,
                                    const std::string & accept, const std::string & acceptEncoding,
                                    const std::string & cacheControl,
                                    const std::string & cookie, const std::string & userAgent)
{
    
    out =  "GET " + data + " HTTP/1.1\r\n" + \
           "Host: " + host + "\r\n" + \
           "Connection: " + connection + "\r\n" + \
           "Accept: " + accept + "\r\n" + \
           "AcceptEncoding: " + acceptEncoding + "\r\n" + \
           "Cache-Control: " + cacheControl + "\r\n" + \
           "Cookie:" + cookie + "\r\n" + \
           "User-Agent: " + userAgent + "\r\n" + \
           "\r\n";
}

bool NetTools::SSLConnect()
{
    this->netSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->netSocket == SOCKET_ERROR)
    {
        printf("Failed to create socket!\n");
        return false;
    }

    //添加SSL的加密/HASH算法
    SSLeay_add_ssl_algorithms();
    //客户端，服务器端选择SSLv23
    const SSL_METHOD * meth = SSLv23_client_method();
    //建立新的SSL上下文
    this->ctx = SSL_CTX_new(meth);
    if(nullptr == this->ctx)
    {
        printf("CTX_NET failed!\n");
        return false;
    }

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_addr.s_addr = inet_addr(this->ipAddress.c_str());
    this->serverAddress.sin_port = htons(atoi(this->port.c_str()));

    if(0 != connect(this->netSocket,(SOCKADDR*)&(this->serverAddress),sizeof(this->serverAddress)))
    {
        printf("Failed to Connect:\n");
        closesocket(this->netSocket);
        printf("socket closed!\n");
        return false;
    }


    //建立SSL
    this->ssl = SSL_new(this->ctx);
    if(nullptr == this->ssl)
    {
        printf("Failed to create SSL\n");
        return false;
    }

    //将SSL与TCP socket链接
    SSL_set_fd(this->ssl, this->netSocket);
    if(SOCKET_ERROR == SSL_connect(this->ssl))
    {
        printf("SSL Accept Error!\n");
        return false;
    }

    return true;
}

bool NetTools::SSLRqueust(std::string & request)
{
    if(SOCKET_ERROR == SSL_write(this->ssl, request.c_str(),static_cast<int>(request.size())))
    {
        printf("Failed to Send SSL!\n");
        closesocket(this->netSocket);
        return false;
    }
    
    return true;
}

bool NetTools::SSLRecvC_Verson(int & bytesReaded, std::string &response, int content)
{
    char * pageBuf = (char*)malloc(content);
    memset(pageBuf, 0, content);
    bytesReaded = 0;
    int recvBytes = 1;
    // printf("Readed::  \n");

    while(recvBytes > 0)
    {
        recvBytes = SSL_read(this->ssl, pageBuf + bytesReaded, content - bytesReaded);
        if(recvBytes > 0)
        {
            bytesReaded += recvBytes;
        }   

        if(content - bytesReaded < 100)
        {
            //realloc
            content *= 2;
            pageBuf = (char*)realloc(pageBuf,content);
        }
    }

    pageBuf[bytesReaded] = '\0';
    response = pageBuf;
    free(pageBuf);
    // closesocket(this->netSocket);
    return true;
}

bool NetTools::SSLDisconnect()
{
    SSL_shutdown(this->ssl);
    SSL_free(this->ssl);
    SSL_CTX_free(this->ctx);
    closesocket(this->netSocket);

    return true;
}



