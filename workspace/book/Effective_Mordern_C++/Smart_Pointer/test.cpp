#include <iostream>
#include <memory>
#include <vector>


class Widget
{

};

void makeLogEntry(Widget * w);

auto loggingDel = [](Widget * pw)
{
    makeLogEntry(pw);
    delete pw;
};

//自定义析构器是其一部分
std::unique_ptr<Widget,decltype(loggingDel)> upw(new Widget, loggingDel);
//并非是其一部分
std::shared_ptr<Widget> spw(new Widget, loggingDel);

auto customDelete1 = [](Widget * pw){};
auto customDelete2 = [](Widget * PW){};

std::shared_ptr<Widget> pw1(new Widget, customDelete1);
std::shared_ptr<Widget> pw2(new Widget, customDelete2);

std::vector<std::shared_ptr<Widget>> vpw {pw1,pw2};


auto pw = new Widget;

std::shared_ptr<Widget> spw1(pw, loggingDel);
std::shared_ptr<Widget> spw2(pw, loggingDel);       //为pw创建了两个控制块。

auto spw = std::make_shared<Widget>();

std::weak_ptr<Widget> wpw(spw);


int main()
{

    return 0;
}