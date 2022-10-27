#ifndef __WEBDEAL__
#define __WEBDEAL__


#include <string>
#include <vector>

class WebDeal
{
    private:
        std::string response;
    public:
        //init
        WebDeal();
        WebDeal(const std::string & response);


        //work
        void GetRespone(const std::string & response);
        std::vector<std::string> GetAllTag(const std::string & tag);
        std::string GetFirstTag(const std::string & tag);
        std::vector<std::string> GetPageLinks();

        //delete
        WebDeal(const WebDeal & webdeal) = delete;
        WebDeal & operator=(const WebDeal & webdeal) = delete;
};


#endif