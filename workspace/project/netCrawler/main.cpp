#include <iostream>
#include <fstream>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <unordered_set>


const int DEFAULT_PAGE_BUF_SIZE = 1048576;
const char * imagePath = "img/";
const char * htmlPath = "html\\";

std::unordered_set<std::string> vis;    //是否访问过的链接


bool ParseURL(const std::string & url, std::string & host, std::string & resource);
bool GetHttpResponse(const std::string & url, char * &response, int & bytesRead);
void SaveFile(const std::string & response,const std::string & url);
void GetAllUrlInOnePage(std::vector<std::string> & urls, const std::string response);
void BFS(const std::string & url);

int main()
{
    printf("Welcome use my netCrawler!\n");

    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2), &wsadata))
    {
        return 0;
    }

    // CreateDirectory(L"img",nullptr);
    // CreateDirectory("img",nullptr);
    // CreateDirectory(L"html",nullptr);
    // CreateDirectory("html",nullptr);

    // std::string urlStart = "https://www.baidu.com/post=2345";
    std::string urlStart = "https://www.baidu.com/";
    // std::string urlStart = "https://www.bilibili.com/";

    

    // std::string host;
    // std::string resource;


    // ParseURL(urlStart, host, resource);


    // std::cout << host << std::endl;
    // std::cout << resource << std::endl;


    // char * response;
    // int bytes;
    // GetHttpResponse(urlStart, response, bytes);

    // std::cout << bytes << std::endl;

    BFS(urlStart);
    return 0;
}


/**
 * @brief 解析URL域名，但是不包括URL域名正确性检测，保证所有进来的域名合法
 * 
 * @param url  待解析的域名
 * @param host  主机名
 * @param resource 其资源部分
 * @return true 解析成功返回真
 * @return false 
 */
bool ParseURL(const std::string & url, std::string & host, std::string & resource)
{

    std::size_t urlen = url.size();

    if(urlen > 2000)
    {
        return false;
    }

    //直接找https://或者是http://,其实共同点应该是://
    std::size_t pos = url.find("://");

    if(pos != std::string::npos)
    {
        std::size_t hostStart = pos + 3;
        std::size_t hostEnd = url.find("/",hostStart);
        if(hostEnd != std::string::npos)
        {
            host = url.substr(hostStart, hostEnd - hostStart);
            resource = url.substr(hostEnd + 1); //直接找后面的资源，后面没有做区分
        }
    }
    else
    {
        return false;
    }

    return "" == host && "" == resource ? false : true;
}

/**
 * @brief 获取HTTP的相应的对象
 * 
 * @param url 待访问的链接
 * @param response 获取相应情况
 * @param bytesRead 
 * @return true 
 * @return false 
 */
bool GetHttpResponse(const std::string & url, char * &response, int & bytesRead)
{
    std::string host;
    std::string resource;

    if(!ParseURL(url, host, resource))
    {
        std::cout << "Can Not parse the url valid" << std::endl;
        return false;
    }


    //建立socket
    struct hostent * hp = gethostbyname(host.c_str());
    if(nullptr == hp)
    {
        std::cout << "Can not find host address!" << std::endl;
        return false;
    }



    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sock < 0)
    {
        std::cout << "Can not create socket!" << std::endl;
        return false;
    }

    //建立服务器地址
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*(in_addr*)hp->h_addr_list[0]));
    serverAddress.sin_port = htons(80);

    if(0 != connect(sock, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
    {
        std::cout << "Can not connect : " << url << std::endl;
        closesocket(sock);
        return false;
    }


    //准备发送数据
    // std::string request = "GET" + \
    //                       resource + \
    //                       " HTPP/1.1\r\nHost:" + \
    //                       host + "\r\nConnection:Keep-Alive\r\n\r\n";
    
    std::string request = "GET " + \
                          url + \
                        " HTTP/1.1\r\nHost:" + \
                        host + "\r\nConnection:Close\r\n\r\n";

    std::cout << request << std::endl;

    //发送数据
    if(SOCKET_ERROR == send(sock, request.c_str(),static_cast<int>(request.size()),0))
    {
        std::cout << "send error" << std::endl;
        closesocket(sock);
        return false;
    }

    //接受数据
    int m_nContentLength = DEFAULT_PAGE_BUF_SIZE;
    char * pageBuf = (char*)malloc(m_nContentLength);
    memset(pageBuf, 0, m_nContentLength);
    // char * pageBuf = new char[m_nContentLength];
    bytesRead = 0;
    int ret = 1;
    std::cout << "Read:: ";
    
    while(ret > 0)
    {
        ret = recv(sock, pageBuf + bytesRead, m_nContentLength - bytesRead, 0);
        if(ret > 0)
        {
            bytesRead += ret;
        }

        if(m_nContentLength - bytesRead < 100)
        {
            std::cout << "\nRealloc memory" << std::endl;
            m_nContentLength *= 2;
            pageBuf = (char*)realloc(pageBuf, m_nContentLength);
        }
        
        std::cout << ret << " ";
    }
    std::cout << std::endl;

    pageBuf[bytesRead] = '\0';
    response = pageBuf;
    closesocket(sock);

    std::cout << response << std::endl;
    return true;
}

void GetAllUrlInOnePage(std::vector<std::string> & urls, const std::string response)
{
    //找到所有链接
    std::string tag = "href=\"";
    std::size_t hrefPos = response.find(tag);
    while(hrefPos != std::string::npos)
    {
        hrefPos += tag.size();
        std::size_t endHrefPos = response.find("\"", hrefPos + 1);
        std::string tempUrl = response.substr(hrefPos + 2, endHrefPos - hrefPos);
        std::cout << tempUrl << std::endl;
        if(vis.find(tempUrl) == vis.end())
        {
            vis.insert(tempUrl);
            urls.emplace_back(tempUrl);
        }
        hrefPos = response.find(tag, endHrefPos);
    }
}

void SaveFile(const std::string & response, const std::string & url)
{
    if("" == response) 
    {
        std::cout << "Nothing in this url: " + url + ":::::end" << std::endl;
    }
    //通过每个url给定设定文件名
    std::string fileName;
    std::size_t poss = url.find("https://");
    std::size_t pos = url.find("http://");
    if(poss != std::string::npos)
    {
        fileName = url.substr(poss + 8);
    }
    else
    {
        fileName = url.substr(pos + 7);
    }
    fileName.pop_back();
    fileName += ".txt";

    //存储所有url

    std::vector<std::string> urls;
    
    GetAllUrlInOnePage(urls, response);
    
    // std::string FinalName = imagePath + fileName;
    std::ofstream netFile(htmlPath + fileName, std::ios::app);
    std::cout << htmlPath + fileName << std::endl;
    for(auto url : urls)
    {
        netFile << url << std::endl;
    }

    netFile.close();
}

void BFS(const std::string & url)
{
    char * response;
    int bytes;
    //get the web and store into "response"
    //获取相应的网页的响应
    if(!GetHttpResponse(url, response, bytes))
    {
        std::cout << "can not get the web response!" << std::endl;
        return;
    }


    std::string httpResponse = response;
    free(response);

    //把中的链接保存到文件中
    SaveFile(httpResponse, url);
}