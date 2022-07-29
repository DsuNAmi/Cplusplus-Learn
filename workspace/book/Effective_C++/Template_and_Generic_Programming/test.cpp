#include <iostream>
#include <vector>

using namespace std;


// template<typename T, std::size_t n>
// class SquareMatrix
// {
//     public:
//         void invert() {}            //求逆矩阵
// };


// class CompanyA
// {
//   	public:
//     	void sendCleartext(const std::string & msg);
//     	void sendEncrypted(const std::string & msg);
    
// };

// class CompanyB
// {
//     public:
//     	void sendCleartext(const std::string & msg);
//     	void sendEncrypted(const std::string & msg);
// };

// class CompanyZ
// {
//     public:
//         void sendEncrypted(const std::string * msg);
// };

// class MsgInfo {};

// template<typename Company>
// class MsgSender
// {
//     public:
//     void SendClear(const MsgInfo & info)
//     {
//         std::string msg;
//         //产生信息
//         Company c;
//         c.sendCleartext(msg);
//     }
//     void SendSecret(const MsgInfo & info) {}	//相同，传输加密
// };

// template<typename Company>
// class LoggingMsgSender : public MsgSender<Company>
// {
//     public:
//     void sendClearMsg(const MsgInfo & info)
//     {
//         //记录
//         this->SendClear(info);		//明确说明
//         //记录
//     }
// };

// template<>      //特化版声明
// class MsgSender<CompanyZ> //全特化，没有别的参数可供变化。
// {
//     public:
//         void SendSecret(const MsgInfo & info) {}
// };


template<typename IterT,  typename DistT>
void doAdvance(IterT & iter, DistT d, std::random_access_iterator_tag)
{
    cout << "randomaccess" << endl;
    //实现random_access迭代器
    iter += d;
    cout << *iter << endl;

}

template<typename IterT,  typename DistT>
void doAdvance(IterT & iter, DistT d, std::bidirectional_iterator_tag)
{
    cout << "Bidirectional" << endl;
    //实现bidirectional迭代器
    if(d >= 0) {while(d--) ++iter;}
    else {while(d++) --iter;}
    cout << *iter << endl;
}

template<typename IterT,  typename DistT>
void doAdvance(IterT & iter, DistT d, std::input_iterator_tag)
{
    cout << "Input" << endl;
    //实现Input迭代器
    if(d < 0)
    {
        throw std::out_of_range("Negative distance");
	}
    else
    {
        while(d--) ++iter;
    }
    cout << *iter << endl;
}

//这里就可以使用iterator_traits，从某种程度上在编译期进行了ifelse测试
template<typename IterT,  typename DistT>
void Advance(IterT& iter, DistT d)
{
    doAdvance(iter,d,typename std::iterator_traits<IterT>::iterator_category());
    //自动判断类型，调用不同的重载函数
}

int main()
{
    // LoggingMsgSender<CompanyA> zMsgSender;
    // MsgInfo msgData;
    // zMsgSender.sendClearMsg(msgData);
    // SquareMatrix<double,5> sm1;
    // sm1.invert();
    // SquareMatrix<double,10> sm2;
    // sm2.invert(); 
    vector<int> t({1,2,3,5});

    auto it = t.begin();

    Advance(it,2);

    return 0;
}