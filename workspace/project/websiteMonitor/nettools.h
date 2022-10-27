#ifndef __NETTOOLS__
#define __NETTOOLS__

#include <winsock2.h>
#include <string>
#include <openssl/err.h>
#include <openssl/ssl.h>


constexpr int DEFAULT_CONTENT = 1048576;

class NetTools
{
    private:
        //connect information
        std::string ipAddress;
        std::string port;
        //socketNetTools
        WSADATA wsadata;
        SOCKET netSocket;
        SOCKADDR_IN serverAddress;
        SSL * ssl;
        SSL_CTX * ctx;
    public:
        //Init
        NetTools();
        NetTools(std::string & ipAddress, std::string & port);
        void SetIP(const std::string & ipAddress) {this->ipAddress = ipAddress;}
        void SetPort(const std::string & port) {this->port = port;}

        //Work
        bool Setup();
        bool Connect();
        bool Request(std::string & request);
        bool RecvC_Verson(int & bytesReaded, std::string &response, int content = DEFAULT_CONTENT);
        bool Endup();

        bool SSLConnect();
        bool SSLRqueust(std::string & request);
        bool SSLRecvC_Verson(int & bytesReaded, std::string &response, int content = DEFAULT_CONTENT);
        bool SSLDisconnect();

        void CreateRequest(std::string & out, const std::string & data, const std::string & host, const std::string connection,
                                  const std::string & accept = "", const std::string & acceptEncoding = "",
                                  const std::string & cacheControl = "",
                                  const std::string & cookie = "", const std::string & userAgent = "");
    



        //delete
        NetTools(const NetTools & netTools) = delete;
        NetTools & operator=(const NetTools & netTools) = delete;
};




#endif