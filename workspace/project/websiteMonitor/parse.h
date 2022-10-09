#ifndef __PARSE__
#define __PARSE__

#include <winsock2.h>
#include <string>

#define DEFAULT_STRING  ""
#define DEFAULT_HTTPTAG  "http://"
#define DEFAULT_HTTPSTAG  "https://"

enum class Protocol
{
    HTTP = 80,
    HTTPS = 443,
    None
};

class Parse
{
    private:
        std::string url;
        std::string host;
        std::string data;
        Protocol whichProtocol;
        std::string parseIpAddress;
        Protocol VaildUrl(const std::string & url);
        bool GetIpAddress(const std::string & url, Protocol pro);
    public:
        //Init
        Parse();
        Parse(std::string & url);

        //Work
        Protocol IsVaildUrl(const std::string & url = DEFAULT_STRING);

        bool UrlChangeIpAddress(Protocol pro, const std::string & url = DEFAULT_STRING);

        std::string IP() const {return this->parseIpAddress;}
        std::string HOST() const {return this->host;}
        std::string DATA() const {return this->data;}


        //delete
        Parse(const Parse & parse) = delete;
        Parse & operator=(const Parse & parse) = delete;
};


#endif