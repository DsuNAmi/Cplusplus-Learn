#include "parse.h"


Parse::Parse()
:url(""), host(""), data(""), parseIpAddress("none"),whichProtocol(Protocol::None)
{

}


Parse::Parse(std::string & url)
:url(url), host(""), data(""),parseIpAddress("none")
{
    this->whichProtocol = IsVaildUrl();
}


Protocol Parse::VaildUrl(const std::string & url)
{
    std::size_t httpPos = url.find(DEFAULT_HTTPTAG);
    std::size_t httpsPos = url.find(DEFAULT_HTTPSTAG);
    if(httpPos == std::string::npos)
    {
        //https
        return Protocol::HTTPS;
    }
    else if(httpsPos == std::string::npos)
    {
        //http
        return Protocol::HTTP;
    }
    else
    {
        printf("Invalid url, change your url");
        return Protocol::None;
    }
}

bool Parse::GetIpAddress(const std::string & url,Protocol pro)
{
    if(pro == Protocol::None) 
    {
        printf("Invalid Protocol!");
        return false;
    }

    std::size_t pos;
    int interval = -1;
    if(pro == Protocol::HTTP)
    {
        pos = url.find("http://");
        interval = 7;
    }
    else if(pro == Protocol::HTTPS)
    {
        pos = url.find("https://");
        interval = 8;
    }

    std::size_t endPos = url.find("/", pos + interval);
    
    if(endPos == std::string::npos)
    {
        this->host = url.substr(pos + interval);
    }
    else
    {
        this->host = url.substr(pos + interval, endPos - pos - interval);
        this->data = url.substr(endPos);
    }

    hostent * hp = gethostbyname(this->host.c_str());
    if(nullptr == hp)
    {
        printf("Lookup IP failed!\n");
        return false;
    }

    this->parseIpAddress = std::string(inet_ntoa(*(in_addr*)hp->h_addr_list[0]));

    return this->parseIpAddress == "none" ? false : true;
}

Protocol Parse::IsVaildUrl(const std::string & url)
{
    //https:// or http://->
    if(0 == url.size())
    {
        return VaildUrl(this->url);
    }
    else
    {
        return VaildUrl(url);
    }
}

bool Parse::UrlChangeIpAddress(Protocol pro, const std::string & url)
{

    if(this->whichProtocol == Protocol::None)
    {
        printf("Invaild Protocol\n");
        return false;
    }

    if(0 == url.size())
    {
        //parse itself
        return GetIpAddress(this->url,pro);
    }
    else
    {
        //parse other url
        return GetIpAddress(url,pro);
    }
}