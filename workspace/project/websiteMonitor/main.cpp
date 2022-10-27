#include "parse.h"
#include "nettools.h"


#include <iostream>



int main()
{
    std::string startUrl = "https://www.tjh.com.cn/public.html";
    // std::string startUrl = "https://www.baidu.com/";
    // std::string startUrl = "https://leetcode.cn/";
    // std::string startUrl = "http://oa.xgb.nankai.edu.cn/";
    // std::string startUrl = "https://oreilly.com/";
    NetTools nettools;
    nettools.Setup();
    Parse urlParse(startUrl);
    if(urlParse.UrlChangeIpAddress(Protocol::HTTPS))
    {
        std::cout << "Get find IP: " << urlParse.IP() << std::endl;
        nettools.SetIP(urlParse.IP());
        nettools.SetPort(std::to_string(static_cast<int>(Protocol::HTTPS)));
        std::string request; 
        nettools.CreateRequest(request,urlParse.DATA(),
                               urlParse.HOST(),
                              "close",
                              "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9",
                              "",
                              "max-age=0",
                              "",
                              "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/106.0.0.0 Safari/537.36 Edg/106.0.1370.34");


        std::cout << request << std::endl;
        nettools.SSLConnect();
        nettools.SSLRqueust(request);
        int bytesReaded = 0;
        std::string response;
        nettools.SSLRecvC_Verson(bytesReaded, response);
        std::cout << response << std::endl;
        nettools.SSLDisconnect();
        // nettools.Connect();
        // nettools.Request(request);
        // int bytesReaded = 0; 
        // std::string response;
        // nettools.RecvC_Verson(bytesReaded, response);
        // std::cout << response << std::endl;
    }
    else
    {
        printf("Parse Failed");
    }
    
    nettools.Endup();
                    
    return 0;
}





// std::string request = "GET " + urlParse.DATA() + " HTTP/1.1\r\n" + \
//                       "Host: " + urlParse.HOST() + "\r\n" + \
//                       "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n" + \
//                       "Accept-Encoding: gzip, deflate, br\r\n" + \
//                       "Cache-Control: max-age=0\r\n" + \
//                       "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/106.0.0.0 Safari/537.36 Edg/106.0.1370.34\r\n" + \
//                       "Connection: close\r\n\r\n";
//                     //   "Cookie: BIGipServerwww.tjh.com.cn=1106618560.20480.0000\r\n" + \
//                     //   "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6" + \
//                     //   "sec-ch-ua: \"Chromium\";v=\"106\", \"Microsoft Edge\";v=\"106\", \"Not;A=Brand\";v=\"99\"\r\n" + \
//                     //   "sec-ch-ua-mobile: ?0\r\n" + \
//                     //   "sec-ch-ua-platform: \"Windows\"\r\n" + \
//                     //   "Sec-Fetch-Dest: document\r\n" + \
//                     //   "Sec-Fetch-Mode: navigate\r\n" + \
//                     //   "Sec-Fetch-Site: none\r\n" + \
//                     //   "Sec-Fetch-User: ?1\r\n" + \
//                     //   "Upgrade-Insecure-Requests: 1\r\n" + \ 