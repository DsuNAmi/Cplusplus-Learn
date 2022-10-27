#include "webdeal.h"



WebDeal::WebDeal() : response("")
{

}

WebDeal::WebDeal(const std::string & response) : response(response)
{

}

std::vector<std::string> WebDeal::GetAllTag(const std::string & tag)
{

}

std::string WebDeal::GetFirstTag(const std::string & tag)
{
    
}

std::vector<std::string> WebDeal::GetPageLinks()
{

}

void WebDeal::GetRespone(const std::string & response)
{
    if(0 != this->response.size())
    {
        printf("already set response!");
        return;
    }

    this->response = response;

    return;
}


